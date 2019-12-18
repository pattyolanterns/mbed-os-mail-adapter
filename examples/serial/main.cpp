#include <mbed.h>

#include "SerialManager.h"
#include "SerialTX.h"
#include "SerialRX.h"

// I opted to make this global, but it can also be a singleton.
SerialTX g_com;

// The normal main thread is not large enough to store the serial mailbox
Thread g_mainThread(osPriorityNormal, 32 * 1024);

void mainThreadLoop();

int main(void) {
    g_mainThread.start(&mainThreadLoop);
    return 0;
}

void mainThreadLoop() {
    // Serial Port
    RawSerial serial(USBTX, USBRX, 115200);
    
    // Serial Mail Manager
    SerialManager serialManager(&eventManager, &serial);

    // Set Serial TX manager here since it's a global, and we don't need
    // our serial manager to be global
    g_com.setManager(&serialManager);

    // Serial RX
    SerialRX _serialRX(&serial, &serialManager);
    g_com.println("Start.");
    
    ThisThread::flags_wait_any(0, true);    // this waits forever
}