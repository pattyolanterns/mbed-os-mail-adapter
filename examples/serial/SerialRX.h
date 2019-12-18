#pragma once

#include <mbed.h>
#include "SerialManager.h"
#include "CircularBuffer.h"

class SerialRX {
public:
    SerialRX(RawSerial* serial, SerialManager* manager);
    
    /* adds new chars to a ring buffer, and creates mail on newline
     */
    void handleChar();

private:
    RawSerial*      _serial;
    SerialManager*  _serialManager;

    CircularBuffer<char, SERIAL_BUFFER_SIZE>    _buffer;
};

#endif