#include "ens16x.h"
#include "utils.h"

using namespace ScioSense::Utils;

namespace ScioSense
{
    void ENS16x::wait()
    {
        if (hasFlag(configuration, Configuration::InterruptEnable))
        {
            waitInterrupt();
        }
        else
        {
            waitPolled();
        }
    }

    ENS16x::AirQualityIndex_UBA  ENS16x::getAirQualityIndex_UBA()
    {
        return (AirQualityIndex_UBA)(dataBuffer[BufferInfo::AqiIndex] & (uint8_t)0b00000111);
    }

    uint16_t ENS16x::getEco2()
    {
        return endian::littleTo<uint16_t>(dataBuffer + BufferInfo::Eco2Index);
    }

    uint16_t ENS16x::getTvoc()
    {
        return endian::littleTo<uint16_t>(dataBuffer + BufferInfo::TvocIndex);
    }

    inline uint32_t calculateResistance(const uint16_t& x)
    {
        return pow(2, (float)(x) / 2048);
    }

    uint32_t ENS16x::getRs0()
    {
        return calculateResistance(endian::littleTo<uint16_t>(resistanceBuffer + BufferInfo::Rs0Index));
    }

    uint32_t ENS16x::getRs1()
    {
        return calculateResistance(endian::littleTo<uint16_t>(resistanceBuffer + BufferInfo::Rs1Index));
    }
    uint32_t ENS16x::getRs2()
    {
        return calculateResistance(endian::littleTo<uint16_t>(resistanceBuffer + BufferInfo::Rs2Index));
    }
    uint32_t ENS16x::getRs3()
    {
        return calculateResistance(endian::littleTo<uint16_t>(resistanceBuffer + BufferInfo::Rs3Index));
    }

    ENS16x::DeviceStatus ENS16x::getDeviceStatus()
    {
        return deviceStatus;
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

    ENS16x::Result ENS16x::read(RegisterAddress address, uint8_t* data, size_t size)
    {
        if (ioInterface)
        {
            return ioInterface->read(address, data, size);
        }

        return Result::IOError;
    }

    ENS16x::Result ENS16x::write(RegisterAddress address, uint8_t* data, size_t size)
    {
        if (ioInterface)
        {
            return ioInterface->write(address, data, size);
        }

        return Result::IOError;
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

    const char* ENS16x::toString(const ENS16x::Result& result)
    {
        switch (result)
        {
            case Result::NotAllowed     : return "not-allowed";
            case Result::IOError        : return "io-error";
            case Result::ChecksumError  : return "checksum-error";
            case Result::Invalid        : return "data-invalid";
            case Result::Ok             : return "ok";
            default                     : return "unknown-status";
        }
    }
}