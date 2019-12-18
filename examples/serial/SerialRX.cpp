#include "SerialRX.h"

SerialRX::SerialRX(RawSerial* serial, SerialManager* manager) :
        _serial(serial),
        _serialManager(manager)
{
    _serial->attach(callback(this, &SerialRX::handleChar), Serial::RxIrq);
}

void SerialRX::handleChar() {
    char c = _serial->getc();

    // dont bother keeping a carriage return
    if (c != 0xD) {
        _buffer.push(c);
    }

    // add line to mailbox and wake up consumer thread
    if (c == '\n') {
    
        SerialPacket* packet = _serialManager->alloc();

        if (packet != nullptr) {
            uint16_t str_i = 0;
            char c = '\0';
            packet->buffer[0] = c;

            while (c != '\n') {
                _buffer.pop(c);
                packet->buffer[str_i++] = c;
            }
            packet->buffer[--str_i] = '\0';
            packet->type = SerialPacketType::RX;

            _serialManager->push(packet);
        }
    }
}