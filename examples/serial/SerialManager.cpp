#include "SerialManager.h"
#include "SerialTX.h"

SerialManager::SerialManager(RawSerial* serial) :
        _serial(serial)
{
    _mailManager.start(callback(this, &SerialManager::onSerial));
}

SerialPacket* SerialManager::alloc() {
    return _mailManager.alloc();
}

void SerialManager::push(SerialPacket* packet) {
    _mailManager.push(packet);
}

void SerialManager::onSerial(SerialPacket* packet) {
    // TX MAIL
    if (packet->type == SerialPacketType::TX) {
        _serial->printf("%s", packet->buffer);
    }
    // RX MAIL
    else if (packet->type == SerialPacketType::RX) {
        // generate serial event if valid
        logln("Got Line: %s", packet->buffer);
    }
}