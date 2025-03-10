#ifndef SCIOSENSE_IO_INTERFACE_ARDUINO_I2C_H
#define SCIOSENSE_IO_INTERFACE_ARDUINO_I2C_H

#include <Arduino.h>
#include <Wire.h>

//// simple example IO Interface implementation

typedef struct ScioSense_Arduino_I2c_Config
{
    TwoWire* wire;
    unsigned char address;
} ScioSense_Arduino_I2c_Config;

static inline int8_t ScioSense_Arduino_I2c_Read(void* config, const uint16_t address, uint8_t* data, const size_t size)
{
    const uint8_t MAX_CHUNK_SIZE= 32;
    TwoWire* wire               = ((ScioSense_Arduino_I2c_Config*)config)->wire;
    unsigned char slaveAddress  = ((ScioSense_Arduino_I2c_Config*)config)->address;

    size_t len = 0;
    while (len < size)
    {
        wire->beginTransmission(slaveAddress);
        wire->write((uint8_t)(address + len));

        if (wire->endTransmission(false) == 0) // 0 == success
        {
            size_t bytesToRequest = (MAX_CHUNK_SIZE < (size - len) ? MAX_CHUNK_SIZE : (size - len));
            wire->requestFrom(slaveAddress, bytesToRequest);
            size_t n = wire->readBytes(data + len, bytesToRequest);
            len += n;

            if (n == 0)
            {
                break;
            }
        }

        if (len == size)
        {
            return 0; // RESULT_OK;
        }
    }

    return 1; // RESULT_IO_ERROR;
}

static inline int8_t ScioSense_Arduino_I2c_Write(void* config, const uint16_t address, uint8_t* data, const size_t size)
{

    TwoWire* wire               = ((ScioSense_Arduino_I2c_Config*)config)->wire;
    unsigned char slaveAddress  = ((ScioSense_Arduino_I2c_Config*)config)->address;

    wire->beginTransmission(slaveAddress);
    wire->write((uint8_t)address);
    wire->write(data, size);
    if (wire->endTransmission() == 0) // 0 == success
    {
        return 0; // RESULT_OK;
    }

    return 1; // RESULT_IO_ERROR;
}

static inline void ScioSense_Arduino_I2c_Wait(uint32_t ms)
{
    delay(ms);
}

#endif // SCIOSENSE_IO_INTERFACE_ARDUINO_I2C_H