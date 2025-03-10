#ifndef SCIOSENSE_ENS160_C_H
#define SCIOSENSE_ENS160_C_H

#include "ScioSense_Ens160_defines.h"

#include <stdbool.h>
#include <inttypes.h>

typedef struct ScioSense_Ens16x_IO
{
    Result  (*read)     (void* config, const uint16_t address, uint8_t* data, const size_t size);
    Result  (*write)    (void* config, const uint16_t address, uint8_t* data, const size_t size);
    void    (*wait)     (const uint32_t ms);
    void* config;
} ScioSense_Ens16x_IO;

typedef struct ScioSense_Ens16x
{
    ScioSense_Ens16x_IO io;
    uint8_t                 dataBuffer      [ENS16X_BUFFER_INFO_DATA_SIZE];
    uint8_t                 resistanceBuffer[ENS16X_BUFFER_INFO_RESISTANCE_DATA_SIZE];
    uint8_t                 firmwareVersion [ENS16X_BUFFER_INFO_FIRMWARE_VERSION_SIZE];
    uint16_t                partId;
    uint8_t                 misr;
    Ens16x_OperatingMode    operatingMode;
    Ens16x_DeviceStatus     deviceStatus;
    Ens16x_Configuration    configuration;
} ScioSense_Ens16x;

static inline Result Ens16x_Init                                  (ScioSense_Ens16x* ens16x);     // Resets the device to IDLE and reads PartID and FirmwareVersion
static inline Result Ens16x_Reset                                 (ScioSense_Ens16x* ens16x);     // Device reset to the power-on configuration.
static inline Result Ens16x_Update                                (ScioSense_Ens16x* ens16x);     // Checks the device Status for NEWDAT and NEWGPR; Reads Data if flags are set.
static inline Result Ens16x_StartStandardMeasure                  (ScioSense_Ens16x* ens16x);     // Starts the STANDARD operating mode.
static inline Result Ens16x_WriteCompensation                     (ScioSense_Ens16x* ens16x, const uint16_t temperature, const uint16_t relativeHumidity);    // Writes ambient temperature and relative humidity data for compensation.
static inline Result Ens16x_WriteConfiguration                    (ScioSense_Ens16x* ens16x, const Ens16x_Configuration configuration);                       // Writes configuration data.
static inline Result Ens16x_ReadPartId                            (ScioSense_Ens16x* ens16x);     // Reads the PartID of the device
static inline Result Ens16x_ReadOpMode                            (ScioSense_Ens16x* ens16x);     // Reads the currently running OpMode
static inline Result Ens16x_ReadFirmwareVersion                   (ScioSense_Ens16x* ens16x);     // Reads the FirmwareVersion; only availeble in IDLE OpMode
static inline Result Ens16x_SetOperatingMode                      (ScioSense_Ens16x* ens16x, const Ens16x_OperatingMode mode);
static inline Result Ens16x_Wait                                  (ScioSense_Ens16x* ens16x);     // Waits for a measurement to finish; Only allowed in MeasurementMode
static inline Result Ens16x_ValidateDataIntegrity                 (ScioSense_Ens16x* ens16x);     // Reads DATA_MISR. returns RESULT_CHECKSUM_ERROR on data checksum failure

static inline bool                Ens16x_IsConnected              (ScioSense_Ens16x* ens16x);     // Tests whether a device is connected; returns true if ReadPartId() returned a ENS16x family ID
static inline bool                Ens160_IsConnected              (ScioSense_Ens16x* ens16x);     // Tests whether a device is connected; returns true if ReadPartId() returned 0x160
static inline AirQualityIndex_UBA Ens16x_GetAirQualityIndex_UBA   (ScioSense_Ens16x* ens16x);     // Calculate and return the air quality index (UBA)
static inline uint16_t            Ens16x_GetEco2                  (ScioSense_Ens16x* ens16x);     // Calculate and return the Equivalent CO2 Concentration (ppm)
static inline uint16_t            Ens16x_GetTvoc                  (ScioSense_Ens16x* ens16x);     // Calculate and return the TVOC Concentration (ppb)
static inline uint32_t            Ens16x_GetRs0                   (ScioSense_Ens16x* ens16x);     // Returns resistor 0 value;
static inline uint32_t            Ens16x_GetRs1                   (ScioSense_Ens16x* ens16x);     // Returns resistor 1 value;
static inline uint32_t            Ens16x_GetRs2                   (ScioSense_Ens16x* ens16x);     // Returns resistor 2 value;
static inline uint32_t            Ens16x_GetRs3                   (ScioSense_Ens16x* ens16x);     // Returns resistor 3 value;
static inline Ens16x_DeviceStatus Ens16x_GetDeviceStatus          (ScioSense_Ens16x* ens16x);     // Returns last DeviceStatus read by update()
static inline uint8_t*            Ens16x_GetFirmwareVersion       (ScioSense_Ens16x* ens16x);     // Returns the firmware version number as uint8_t[3] {major, minor, release}
static inline uint8_t*            Ens16x_GetDataRaw               (ScioSense_Ens16x* ens16x);     // Returns the the internal uint8_t[7] read buffer

static inline uint16_t            Ens16x_CalcTempInFromKelvin     (const float temperature);      // Calculate raw Temp_In from Kelvin; Result is to be used as temperature value for Ens16x_WriteCompensation
static inline uint16_t            Ens16x_CalcTempInFromCelsius    (const float temperature);      // Calculate raw Temp_In from Celsius; Result is to be used as temperature value for Ens16x_WriteCompensation
static inline uint16_t            Ens16x_CalcTempInFromFahrenheit (const float temperature);      // Calculate raw Temp_In from Fahrenheit; Result is to be used as temperature value for Ens16x_WriteCompensation
static inline uint16_t            Ens16x_CalcRhIn                 (const float relativeHumidity); // Calculate raw RH_In from relative humidity %. Result is to be used as relativeHumidity value for Ens16x_WriteCompensation

#include "ScioSense_Ens160.inl.h"
#endif // SCIOSENSE_ENS160_C_H