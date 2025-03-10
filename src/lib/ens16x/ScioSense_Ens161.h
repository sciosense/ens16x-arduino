#ifndef SCIOSENSE_ENS161_C_H
#define SCIOSENSE_ENS161_C_H

#include "ScioSense_Ens160.h"

// ENS161 specific Register Address in addition to the base ENS16x defines
#define ENS161_REGISTER_ADDRESS_DATA_AQIS                  (0x26)                   // This 2-byte register reports the calculated relative Air Quality Index, proprietary to ScioSense, in arbitrary units between 0 and 500

// ENS161 specific Operating Modes (OPMODE) in addition to the base ENS16x defines
#define ENS161_OPERATING_MODE_LOWPOWER                     (0x03)                   // Low power Gas Sensing Mode
#define ENS161_OPERATING_MODE_ULTRALOWPOWER                (0x04)                   // Ultra low power Gas Sensing Mode

//// ENS161 specific System Timing Characteristics in addition to the base ENS16x defines
#define ENS161_SYSTEM_TIMING_LOWPOWER_MEASURE       (2000)
#define ENS161_SYSTEM_TIMING_ULTRALOWPOWER_MEASURE  (60000)

//// ENS161 specific internally used buffer and data indices and sizes (in bytes) in addition to the base class (ENS16x::BufferInfo)
#define ENS161_BUFFER_INFO_AQIS_INDEX                      (ENS161_REGISTER_ADDRESS_DATA_AQIS    - ENS16X_REGISTER_ADDRESS_DATA_AQI)


static inline Result      Ens161_StartLowPowerMeasure         (ScioSense_Ens16x* ens16x);         // Starts the LOW POWER operating mode.
static inline Result      Ens161_StartUltraLowPowerMeasure    (ScioSense_Ens16x* ens16x);         // Starts the ULTRA LOW POWER operating mode.

static inline bool        Ens161_IsConnected                  (ScioSense_Ens16x* ens16x);         // Tests whether a device is connected; returns true if ReadPartId() returned 0x161
static inline uint16_t    Ens161_GetAirQualityIndex_ScioSense (ScioSense_Ens16x* ens16x);         // Calculate and return the Relative Air Quality Index according to ScioSense

#include "ScioSense_Ens161.inl.h"
#endif // SCIOSENSE_ENS161_C_H
