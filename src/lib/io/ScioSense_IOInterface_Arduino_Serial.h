#ifndef SCIOSENSE_IO_INTERFACE_ARDUINO_SERIAL_H
#define SCIOSENSE_IO_INTERFACE_ARDUINO_SERIAL_H

#include <Arduino.h>
#include <Stream.h>

//// simple example IO Interface implementation

typedef struct ScioSense_Arduino_Serial_Config
{
    Stream* serial;
} ScioSense_Arduino_Serial_Config;

static inline int8_t ScioSense_Arduino_Serial_Read(void* config, const uint16_t address, uint8_t* data, const size_t size)
{
    Stream* serial = ((ScioSense_Arduino_Serial_Config*)config)->serial;

    if (serial->readBytes(data, size) == size)
    {
        return 0; // RESULT_OK
    }

    return 1; // RESULT_IO_ERROR;
}

static inline int8_t ScioSense_Arduino_Serial_Write(void* config, const uint16_t address, uint8_t* data, const size_t size)
{
    Stream* serial = ((ScioSense_Arduino_Serial_Config*)config)->serial;

    if (serial->write(data, size) == size)
    {
        return 0; // RESULT_OK
    }

    return 1; // RESULT_IO_ERROR;
}

static inline int8_t ScioSense_Arduino_Serial_Clear(void* config)
{
    Stream* serial = ((ScioSense_Arduino_Serial_Config*)config)->serial;

    uint8_t buf[8];
    serial->flush();
    while (serial->available())
    {
        serial->readBytes(buf, 8);
    }

    return 0; // RESULT_OK
}

static inline void ScioSense_Arduino_Serial_Wait(uint32_t ms)
{
    delay(ms);
}

#endif // SCIOSENSE_IO_INTERFACE_ARDUINO_SERIAL_H