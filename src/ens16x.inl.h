#include "ens16x.h"


#define hasAnyFlag(a, b)    (((a) & (b)) != 0)
#define hasFlag(a, b)       (((a) & (b)) == (b))

inline ENS16x::~ENS16x() { }

inline ENS16x::ENS16x()
{
    partId          = 0;
    operatingMode   = ENS16X_OPERATING_MODE_DEEP_SLEEP;
    configuration   = ENS16X_CONFIGURATION_DEFAULT;
    interruptPin    = NOT_A_PIN;
    autoMisr        = false;

    debugStream     = nullptr;
}

void ENS16x::begin(TwoWire* wire, unsigned char address)
{
    i2cConfig           = { 0 };
    i2cConfig.wire      = wire;
    i2cConfig.address   = address;

    io.read             = ScioSense_Arduino_I2c_Read;
    io.write            = ScioSense_Arduino_I2c_Write;
    io.wait             = ScioSense_Arduino_I2c_Wait;
    io.config           = &i2cConfig;
}

void ENS16x::begin(SPIClass* spi, uint8_t chipSelect, const SPISettings settings)
{
    spiConfig               = { 0 };
    spiConfig.spi           = spi;
    spiConfig.csPin         = chipSelect;
    spiConfig.settings      = settings;
    spiConfig.useSpiSettings= true;

    io.read             = ScioSense_Arduino_Spi_Read;
    io.write            = ScioSense_Arduino_Spi_Write;
    io.wait             = ScioSense_Arduino_Spi_Wait;
    io.config           = &spiConfig;

    //disable i2c and enable spi by setting CSn line low;
    pinMode(spiConfig.csPin, OUTPUT);
    digitalWrite(spiConfig.csPin, LOW);
    delay(1);
    digitalWrite(spiConfig.csPin, HIGH);
}

bool ENS16x::init()
{
    Ens16x_Init(this);

    debug("PART_ID  : ", partId);
    if (this->debugStream)
    {
        this->debugStream->print("Firmware: ");
        this->debugStream->print(firmwareVersion[0]);
        this->debugStream->print(".");
        this->debugStream->print(firmwareVersion[1]);
        this->debugStream->print(".");
        this->debugStream->println(firmwareVersion[2]);
    }

    return isConnected();
}

Result ENS16x::reset()
{
    Result result = Ens16x_Reset(this);

    debug(__func__, result);
    return result;
}

Result ENS16x::setOperatingMode(const Ens16x_OperatingMode& mode)
{
    return Ens16x_SetOperatingMode(this, mode);
}

Result ENS16x::invoke(const Ens16x_Command& command, unsigned long ms)
{
    return Ens16x_Invoke(this, command, ms);
}

Result ENS16x::startStandardMeasure()
{
    return setOperatingMode(ENS16X_OPERATING_MODE_STANDARD);
}

Result ENS16x::writeCompensation(const uint16_t& temperature, const uint16_t& relativeHumidity)
{
    return Ens16x_WriteCompensation(this, temperature, relativeHumidity);
}

Result ENS16x::writeConfiguration(const Ens16x_Configuration& configuration)
{
    return Ens16x_WriteConfiguration(this, configuration);
}

void ENS16x::waitPolled()
{
    Ens16x_Wait(this);
}

void ENS16x::waitInterrupt()
{
    if (hasFlag(configuration, ENS16X_CONFIGURATION_INTERRUPT_POLARITY))
    {
        while (digitalRead(interruptPin) != HIGH); // wait until high
    }
    else
    {
        while (digitalRead(interruptPin) != LOW); // wait until low
    }
}

void ENS16x::wait()
{
    if (hasFlag(configuration, ENS16X_CONFIGURATION_INTERRUPT_ENABLE))
    {
        waitInterrupt();
    }
    else
    {
        waitPolled();
    }
}

Result ENS16x::update()
{
    Result result;
    result = Ens16x_Update(this);
    if(autoMisr)
    {
        result = Ens16x_ValidateDataIntegrity(this);
    }

    return result;
}

AirQualityIndex_UBA ENS16x::getAirQualityIndex_UBA()
{
    return Ens16x_GetAirQualityIndex_UBA(this);
}

uint16_t ENS16x::getEco2()
{
    return Ens16x_GetEco2(this);
}

uint16_t ENS16x::getTvoc()
{
    return Ens16x_GetTvoc(this);
}

uint32_t ENS16x::getRs0()
{
    return Ens16x_GetRs0(this);
}

uint32_t ENS16x::getRs1()
{
    return Ens16x_GetRs1(this);
}
uint32_t ENS16x::getRs2()
{
    return Ens16x_GetRs2(this);
}
uint32_t ENS16x::getRs3()
{
    return Ens16x_GetRs3(this);
}

Ens16x_DeviceStatus ENS16x::getDeviceStatus()
{
    return deviceStatus;
}

bool ENS16x::hasNewData()
{
    return (deviceStatus & ENS16X_DEVICE_STATUS_NEW_DATA) == ENS16X_DEVICE_STATUS_NEW_DATA;
}

bool ENS16x::hasNewGeneralPurposeData()
{
    return (deviceStatus & ENS16X_DEVICE_STATUS_NEW_GENERAL_PURPOSE_DATA) == ENS16X_DEVICE_STATUS_NEW_GENERAL_PURPOSE_DATA;
}

uint8_t* ENS16x::getFirmwareVersion()
{
    return firmwareVersion;
}

uint8_t* ENS16x::getDataRaw()
{
    return dataBuffer;
}

void ENS16x::setInterruptPin(int p)
{
    pinMode(p, INPUT);
    interruptPin = p;
}

Result ENS16x::read(Ens16x_RegisterAddress address, uint8_t* data, size_t size)
{
    if (this->io.read)
    {
        return this->io.read(this->io.config, address, data, size);
    }

    return RESULT_IO_ERROR;
}

Result ENS16x::write(Ens16x_RegisterAddress address, uint8_t* data, size_t size)
{
    if (this->io.write)
    {
        return this->io.write(this->io.config, address, data, size);
    }

    return RESULT_IO_ERROR;
}

void ENS16x::enableAutoDataIntegrityValidation(bool enable)
{
    autoMisr = enable;
}

void ENS16x::enableDebugging(Stream& debugStream)
{
    this->debugStream = &debugStream;
}

void ENS16x::disableDebugging()
{
    debugStream = nullptr;
}

void ENS16x::debug(const char* msg)
{
    if (debugStream)
    {
        debugStream->print(debugPrefix);
        debugStream->println(msg);
    }
}

void ENS16x::debug(const char* msg, Result& result)
{
    debug(msg, nullptr, 0, result);
}

void ENS16x::debug(const char* msg, uint8_t* data, size_t size, Result& result)
{
    if (debugStream)
    {
        debugStream->print(debugPrefix);
        debugStream->print(msg);

        for (size_t i = 0; i < size; i++)
        {
            debugStream->print(" 0x");
            debugStream->print(data[i], HEX);
        }

        debugStream->print(" status: ");
        debugStream->println(toString(result));
    }
}

template<class T>
void ENS16x::debug(const char* msg, T data)
{
    if (debugStream)
    {
        debugStream->print(debugPrefix);
        debugStream->print(msg);
        debugStream->print(" 0x");
        debugStream->print(data, HEX);
        debugStream->println();
    }
}

const char* ENS16x::toString(const Result& result)
{
    switch (result)
    {
        case RESULT_NOT_ALLOWED     : return "not-allowed";
        case RESULT_IO_ERROR        : return "io-error";
        case RESULT_CHECKSUM_ERROR  : return "checksum-error";
        case RESULT_INVALID         : return "data-invalid";
        case RESULT_OK              : return "ok";
        default                     : return "unknown-status";
    }
}
