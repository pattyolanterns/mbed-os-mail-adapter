#include "SerialTX.h"
#include "SerialManager.h"

SerialTX::SerialTX() :
        _serialManager(nullptr)
{

}

void SerialTX::setManager(SerialManager* manager) {
    _serialManager = manager;
}

uint8_t SerialTX::println(const char* format, ...) {
    if (_serialManager != nullptr) {
        char buffer[256];

        // put formatted string into buffer
        va_list argp;
        va_start(argp, format);
        vsnprintf(buffer, 255, format, argp);
        va_end(argp);
        strcat(buffer, "\n");

        // if the buffer is larger than our mail buffer, partition the buffer across
        // multiple mail packets
        uint16_t buffer_i = 0;
        while (strlen(buffer + buffer_i) > 0) {
            SerialPacket* packet = _serialManager->alloc();

            if (packet != nullptr) {
                packet->type = SerialPacketType::TX;
                packet->buffer[0] = '\0';
                strncat(packet->buffer, buffer + buffer_i, SERIAL_BUFFER_SIZE-1);
                if (strlen(buffer + buffer_i) > SERIAL_BUFFER_SIZE-1) {
                    buffer_i += SERIAL_BUFFER_SIZE-1;
                }
                else {
                    buffer_i += strlen(packet->buffer);
                }
                _serialManager->push(packet);
            }
            else {
                // only get here in isr, do we call a back up serial tx?
                return 1;
            }
        }
        return 0;
    }
    return 1;
}