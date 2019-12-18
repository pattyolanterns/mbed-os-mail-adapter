#pragma once

#include <mbed.h>

/* MailManager: an adapter for mbed's Mail RTOS object that allows
 * mail to be produced in a CPU-efficient mannor. AKA we push mail
 * to the mailbox, and signal the consumer thread to wake up and
 * handle the new mail.
 */
template<typename T, uint32_t queue_sz>
class MailManager {
public:
    MailManager();

/********** Consumption Methods **********/

public:

    /* starts the threads loop function that waits for mail 
     *
     * @param func - the callback to call when mail is available
     */ 
    void start(Callback<void(T*)> func);

private:

    /* the threads loop function, it waits for mail then calls the
     * callback function provided to handle the mail, then free's
     * the mail.
     */
    void loop();

    /* sleeps the the thread if there is no mail, is woken up if signaled
     * by MailProducer
     */
    void wait();

    /* gets mail from the mailbox
     *
     * @return T* - pointer to the mail
     */
    T* get();

    /* frees the mail from the mailbox
     *
     * @param T* mail - the mail to free
     */
    void free(T* mail);

/********** Production Methods **********/

public:

    /* allocates memory for mail in the mailbox
     *
     * @return T* - pointer to the mail type
     * @return nullptr - if mailbox is full
     * 
     * @note - this is a non-blocking function, and can be called from
     * the ISR
     */
    T* alloc();

    /* puts mail into the mailbox, and signals the consumer thread to
     * wake up and handle the mail.
     * 
     * @param mail - the mail to push
     * 
     * @note mail must be allocated using alloc(), 
     */
    void push(T* mail);

private:
    Mail<T, queue_sz>   _mailbox;
    Thread              _thread;
    uint32_t            _flag;
    Callback<void(T*)>  _callback;
};

template<typename T, uint32_t queue_sz>
MailManager<T, queue_sz>::MailManager() :
        _flag(0x1),
        _callback(nullptr)
{

}

template<typename T, uint32_t queue_sz>
void MailManager<T, queue_sz>::start(Callback<void(T*)> func) {
    _callback = func;
    _thread.start(callback(this, &MailManager::loop));
}

template<typename T, uint32_t queue_sz>
void MailManager<T, queue_sz>::loop() {
    while (1) {
        T* mail = get();
        if (mail != nullptr) {
            if (_callback != nullptr) {
                _callback(mail);
            }
            free(mail);
        }
    }
}

template<typename T, uint32_t queue_sz>
void MailManager<T, queue_sz>::wait() {
    if (_mailbox.empty()) {
        ThisThread::flags_wait_any(_flag, true);
    }
}

template<typename T, uint32_t queue_sz>
T* MailManager<T, queue_sz>::get() {
    wait();
    osEvent evt = _mailbox.get(0);
    if (evt.status == osEventMail) {
        return (T*)evt.value.p;
    }
    return nullptr;
}

template<typename T, uint32_t queue_sz>
void MailManager<T, queue_sz>::free(T* mail) {
    _mailbox.free(mail);
}

template<typename T, uint32_t queue_sz>
T* MailManager<T, queue_sz>::alloc() {
    T* mail;
    
    // yield this thread so consumer thread (_thread) can empty mailbox before
    // putting more mail in, only do this if not in ISR, get_id returns nullptr
    if (ThisThread::get_id() != nullptr) {
        while ((mail = _mailbox.alloc()) == nullptr) {
            ThisThread::yield();
        }
    }
    else {
        mail = _mailbox.alloc();
    }

    return mail;
}

template<typename T, uint32_t queue_sz>
void MailManager<T, queue_sz>::push(T* mail) {
    _mailbox.put(mail);

    // this can error if the thread has not started
    _thread.flags_set(_flag);
}