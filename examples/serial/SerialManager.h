#pragma once

#include <mbed.h>
#include "MailManager.h"
#include "SerialPacket.h"

#define SERIAL_MAILBOX_SIZE     64

/*
 * SerialManager wraps MailManager to handle both tx and rx serial that is
 * both thread safe and isr safe.
 */
class SerialManager {
public:
    SerialManager(RawSerial* serial);
    SerialPacket* alloc();
    void push(SerialPacket* packet);

private:
    void onSerial(SerialPacket* packet);

    RawSerial*      _serial;
    MailManager<SerialPacket, SERIAL_MAILBOX_SIZE>  _mailManager;
};