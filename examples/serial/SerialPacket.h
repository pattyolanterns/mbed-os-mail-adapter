#pragma once

#include <mbed.h>

#define SERIAL_BUFFER_SIZE      64

enum SerialPacketType {
    RX = 0,
    TX = 1
};

struct SerialPacket {
    char buffer[SERIAL_BUFFER_SIZE];
    SerialPacketType type;
};

#endif