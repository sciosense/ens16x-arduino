#ifndef SCIOSENSE_ENS161_C_INL
#define SCIOSENSE_ENS161_C_INL

#include "ScioSense_Ens161.h"


static inline Result Ens161_StartLowPowerMeasure(ScioSense_Ens16x* ens16x)
{
    return Ens16x_SetOperatingMode(ens16x, ENS161_OPERATING_MODE_LOWPOWER);
}

static inline Result Ens161_StartUltraLowPowerMeasure(ScioSense_Ens16x* ens16x)
{
    return Ens16x_SetOperatingMode(ens16x, ENS161_OPERATING_MODE_ULTRALOWPOWER);
}

static inline bool Ens161_IsConnected(ScioSense_Ens16x* ens16x)
{
    return ens16x->partId == 0x161;
}

static inline uint16_t  Ens161_GetAirQualityIndex_ScioSense(ScioSense_Ens16x* ens16x)
{
    return (ens16x->dataBuffer[ENS161_BUFFER_INFO_AQIS_INDEX + 1] << 8) + ens16x->dataBuffer[ENS161_BUFFER_INFO_AQIS_INDEX];
}

static inline Result Ens161_Wait(ScioSense_Ens16x* ens16x)
{
    switch (ens16x->operatingMode)
    {
        case ENS16X_OPERATING_MODE_STANDARD        : ens16x->io.wait(ENS16X_SYSTEM_TIMING_STANDARD_MEASURE);      break;
        case ENS161_OPERATING_MODE_LOWPOWER        : ens16x->io.wait(ENS161_SYSTEM_TIMING_LOWPOWER_MEASURE);      break;
        case ENS161_OPERATING_MODE_ULTRALOWPOWER   : ens16x->io.wait(ENS161_SYSTEM_TIMING_ULTRALOWPOWER_MEASURE); break;
        default: return RESULT_NOT_ALLOWED;
    }

    return RESULT_OK;
}


#endif // SCIOSENSE_ENS161_C_INL