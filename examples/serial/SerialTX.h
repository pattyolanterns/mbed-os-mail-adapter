#pragma once

#include <mbed.h>

class SerialManager;

class SerialTX {
public:
    SerialTX();

    /* formats the string and puts it into mail, wakes up the serial thread
     * so that it is handled. 
     */
    uint8_t println(const char* format, ...);

    void setManager(SerialManager* manager);

private:
    SerialManager* _serialManager;
};

// global communicator, this is defined in main.cpp, and can be used by
// anyone who includes this file
extern SerialTX g_com;

// #define VERBOSE_1
#define VERBOSE_2
// #define VERBOSE_3
// #define VERBOSE_ALL

#define LOGS
#ifdef LOGS
    #define logln(f_, ...) g_com.println("D: " f_ , ##__VA_ARGS__)
#else
    #define logln(f_, ...)
#endif

#endif