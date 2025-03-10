#ifndef SCIOSENSE_IO_INTERFACE_ARDUINO_SPI_H
#define SCIOSENSE_IO_INTERFACE_ARDUINO_SPI_H

#include <Arduino.h>
#include <SPI.h>

typedef struct ScioSense_Arduino_Ens16x_Spi_Config
{
    SPIClass*   spi;
    uint8_t     csPin;
    bool        useSpiSettings;
    SPISettings settings;
} ScioSense_Arduino_Ens16x_Spi_Config;

static inline int8_t ScioSense_Arduino_Spi_Read(void* config, const uint16_t address, uint8_t* data, const size_t size)
{
    ScioSense_Arduino_Ens16x_Spi_Config* _config = ((ScioSense_Arduino_Ens16x_Spi_Config*)config);

    if (_config->useSpiSettings)
    {
        _config->spi->beginTransaction(_config->settings);
    }

    digitalWrite(_config->csPin, LOW);
    {
        _config->spi->transfer(((uint8_t)address << 1) + 1);
        for (size_t i = 0; i < size; i++)
        {
            data[i] = _config->spi->transfer(0xff);
        }
    }
    digitalWrite(_config->csPin, HIGH);

    if (_config->useSpiSettings)
    {
        _config->spi->endTransaction();
    }

    return 0; // RESULT_OK;
}

static inline int8_t ScioSense_Arduino_Spi_Write(void* config, const uint16_t address, uint8_t* data, const size_t size)
{
    ScioSense_Arduino_Ens16x_Spi_Config* _config = ((ScioSense_Arduino_Ens16x_Spi_Config*)config);

    if (_config->useSpiSettings)
    {
        _config->spi->beginTransaction(_config->settings);
    }

    digitalWrite(_config->csPin, LOW);
    {
        for (size_t i = 0; i < size; i++)
        {
            _config->spi->transfer(((uint8_t)address + i) << 1);
            _config->spi->transfer(data[i]);
        }
    }
    digitalWrite(_config->csPin, HIGH);

    if (_config->useSpiSettings)
    {
        _config->spi->endTransaction();
    }

    return 0; // RESULT_OK;
}

static inline void ScioSense_Arduino_Spi_Wait(uint32_t ms)
{
    delay(ms);
}

#endif // SCIOSENSE_IO_INTERFACE_ARDUINO_SPI_H