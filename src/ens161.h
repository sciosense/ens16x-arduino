#ifndef SCIOSENSE_ENS161_H
#define SCIOSENSE_ENS161_H

#include "ens16x.h"

namespace ScioSense
{
    class ENS161 : public ENS16x
    {
    public:

        // ENS161 specific System Timing Characteristics in addition to the base class (ENS16x::SystemTiming)
        struct SystemTiming
        {
            static constexpr uint16_t LowPowerMeasure       = 2000;
            static constexpr uint16_t UltraLowPowerMeasure  = 60000;
        };

        // ENS161 specific Operating Modes (OPMODE) in addition to the base class (ENS16x::OperatingMode)
        enum class OperatingMode : uint8_t
        {
            LowPower       = 0x03,                                      // Low power Gas Sensing Mode
            UltraLowPower  = 0x04,                                      // Ultra low power Gas Sensing Mode

            Unknown        = 0xFF,
        };

        // ENS161 specific internally used buffer and data indices and sizes (in bytes) in addition to the base class (ENS16x::BufferInfo)
        struct BufferInfo
        {
            static constexpr uint8_t AqisIndex = (uint8_t)RegisterAddress::DATA_AQIS - (uint8_t)RegisterAddress::DATA_AQI;
        };

    public:
        ENS161();
        bool isConnected() override;                                    // checks if the read PART_ID matches the expected value; returns true, if so.

    public:
        Result startLowPowerMeasure();                                  // Starts the LOW POWER operating mode.
        Result startUltraLowPowerMeasure();                             // Starts the ULTRA LOW POWER operating mode.

    public:
        void waitPolled() override;                                     // Wait the operation mode specific response time
        inline uint16_t  getAirQualityIndex_ScioSense();                // Calculate and return the Relative Air Quality Index according to ScioSense

    protected:
        Result setOperatingMode(const OperatingMode& mode);

    protected:
        OperatingMode operatingMode;
    };
}

#include "ens161.inl"

#endif //SCIOSENSE_ENS161_H