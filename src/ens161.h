#ifndef SCIOSENSE_ENS161_H
#define SCIOSENSE_ENS161_H

#include "ens16x.h"
#include "lib/ens16x/ScioSense_Ens161.h"

class ENS161 : public ENS16x
{
public:
    ENS161();
    bool isConnected() override;                                    // checks if the read PART_ID matches the expected value; returns true, if so.

public:
    Result startLowPowerMeasure();                                  // Starts the LOW POWER operating mode.
    Result startUltraLowPowerMeasure();                             // Starts the ULTRA LOW POWER operating mode.

public:
    void waitPolled() override;                                     // Wait the operation mode specific response time
    inline uint16_t  getAirQualityIndex_ScioSense();                // Calculate and return the Relative Air Quality Index according to ScioSense
};

#include "ens161.inl.h"

#endif //SCIOSENSE_ENS161_H