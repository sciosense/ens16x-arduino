#include "ens161.h"
#include "lib/ens16x/ScioSense_Ens161.h"

ENS161::ENS161() : ENS16x()
{
    debugPrefix = "ENS161 debug -- ";
}

bool ENS161::isConnected()
{
    return Ens161_IsConnected(this);
}

Result ENS161::startLowPowerMeasure()
{
    return setOperatingMode(ENS161_OPERATING_MODE_LOWPOWER);
}

Result ENS161::startUltraLowPowerMeasure()
{
    return setOperatingMode(ENS161_OPERATING_MODE_ULTRALOWPOWER);
}

void ENS161::waitPolled()
{
    Ens161_Wait(this);
}

uint16_t ENS161::getAirQualityIndex_ScioSense()
{
    return Ens161_GetAirQualityIndex_ScioSense(this);
}
