#include "ens161.h"

namespace ScioSense
{
    ENS161::ENS161() : ENS16x()
    {
        debugPrefix = "ENS161 debug -- ";
    }

    bool ENS161::isConnected()
    {
        return partId == 0x0161;
    }

    ENS161::Result ENS161::setOperatingMode(const OperatingMode& mode)
    {
        if (ENS16x::operatingMode != ENS16x::OperatingMode::Idle)
        {
            ENS16x::setOperatingMode(ENS16x::OperatingMode::Idle);
        }

        Result result = write(RegisterAddress::OPMODE, (uint8_t*)&mode, sizeof(mode));

        if (result == Result::Ok)
        {
            ENS16x::operatingMode   = ENS16x::OperatingMode::Unknown;
            operatingMode           = mode;
        }

        return result;
    }

    ENS161::Result ENS161::startLowPowerMeasure()
    {
        return setOperatingMode(OperatingMode::LowPower);
    }

    ENS161::Result ENS161::startUltraLowPowerMeasure()
    {
        return setOperatingMode(OperatingMode::UltraLowPower);
    }

    void ENS161::waitPolled()
    {
        if (ENS16x::operatingMode != ENS16x::OperatingMode::Unknown)
        {
            ENS16x::waitPolled();
        }
        else
        {
            if (operatingMode == OperatingMode::LowPower)
            {
                delay(SystemTiming::LowPowerMeasure);
            }
            else if (operatingMode == OperatingMode::UltraLowPower)
            {
                delay(SystemTiming::UltraLowPowerMeasure);
            }
        }
    }

    uint16_t ENS161::getAirQualityIndex_ScioSense()
    {
        return endian::littleTo<uint16_t>(dataBuffer + BufferInfo::AqisIndex);
    }
}