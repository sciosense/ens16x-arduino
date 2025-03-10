#ifndef SCIOSENSE_ENS160_C_INL
#define SCIOSENSE_ENS160_C_INL

#include "ScioSense_Ens160.h"

#include <math.h>

#define write(address, data, len)   ens16x->io.write(ens16x->io.config, (address), (uint8_t*)(data), (len))
#define wait(ms)                    ens16x->io.wait(ms)

#define hasAnyFlag(a, b)    (((a) & (b)) != 0)
#define hasFlag(a, b)       (((a) & (b)) == (b))

static inline Result Ens16x_Invoke(ScioSense_Ens16x* ens16x, const Ens16x_Command command, uint32_t ms);

static inline Result Ens16x_Read(ScioSense_Ens16x* ens16x, const uint16_t address, uint8_t* data, const size_t size)
{
    Result result;

    result= ens16x->io.read(ens16x->io.config, address, data, size);
    if (result == RESULT_OK)
    {
        //MISR data integrity check
        //if (    (address >= ENS16X_REGISTER_ADDRESS_DEVICE_STATUS)
        //    &&  (address <  ENS16X_REGISTER_ADDRESS_DATA_MISR)
        //)
        if(address != ENS16X_REGISTER_ADDRESS_DATA_MISR)
        {
            for(size_t i=0; i<size; i++)
            {
                if ((ens16x->misr & 0x80) == 0)
                {
                    ens16x->misr= ((uint8_t)(ens16x->misr<<1) ^ data[i]);
                }
                else
                {
                    ens16x->misr= ((uint8_t)(ens16x->misr<<1) ^ data[i]) ^ (uint8_t)0x1d;
                }
            }
        }
    }

    return result;
}

static inline Result Ens16x_Init(ScioSense_Ens16x* ens16x)
{
    Result result;

    result = Ens16x_Reset(ens16x);
    if (result != RESULT_OK)
    {
        return result;
    }

    result = Ens16x_SetOperatingMode(ens16x, ENS16X_OPERATING_MODE_IDLE);
    if (result != RESULT_OK)
    {
        return result;
    }

    result = Ens16x_ReadPartId(ens16x);
    if (result != RESULT_OK)
    {
        return result;
    }

    result = Ens16x_ReadFirmwareVersion(ens16x);
    if (result != RESULT_OK)
    {
        return result;
    }

    return result;
}

static inline Result Ens16x_Reset(ScioSense_Ens16x* ens16x)
{
    Result result;

    result = Ens16x_SetOperatingMode(ens16x, ENS16X_OPERATING_MODE_RESET);
    if (result != RESULT_OK)
    {
        return result;
    }

    wait(ENS16X_SYSTEM_TIMING_RESET);

    result = Ens16x_SetOperatingMode(ens16x, ENS16X_OPERATING_MODE_IDLE);
    if (result != RESULT_OK)
    {
        return result;
    }

    Ens16x_ValidateDataIntegrity(ens16x);

    return result;
}

static inline Result Ens16x_Update(ScioSense_Ens16x* ens16x)
{
    Result result;

    result = Ens16x_Read(ens16x, ENS16X_REGISTER_ADDRESS_DEVICE_STATUS, &ens16x->deviceStatus, sizeof(ens16x->deviceStatus));
    if (result == RESULT_OK)
    {
        result = RESULT_INVALID;

        if (hasFlag(ens16x->deviceStatus, ENS16X_DEVICE_STATUS_NEW_DATA))
        {
            result = Ens16x_Read(ens16x, ENS16X_REGISTER_ADDRESS_DATA_AQI, ens16x->dataBuffer, sizeof(ens16x->dataBuffer));
            if (result != RESULT_OK)
            {
                return result;
            }
        }

        if (hasFlag(ens16x->deviceStatus, ENS16X_DEVICE_STATUS_NEW_GENERAL_PURPOSE_DATA))
        {
            result = Ens16x_Read(ens16x, ENS16X_REGISTER_ADDRESS_GPR_READ, ens16x->resistanceBuffer, sizeof(ens16x->resistanceBuffer));
            if (result != RESULT_OK)
            {
                return result;
            }
        }
    }

    return result;
}

static inline Result Ens16x_StartStandardMeasure(ScioSense_Ens16x* ens16x)
{
    return Ens16x_SetOperatingMode(ens16x, ENS16X_OPERATING_MODE_STANDARD);
}

static inline Result Ens16x_WriteCompensation(ScioSense_Ens16x* ens16x, const uint16_t temperature, const uint16_t relativeHumidity)
{
    uint8_t compensation[4];
    compensation[0] = temperature & 0xff;
    compensation[1] = (temperature >> 8) & 0xff;

    compensation[2] = relativeHumidity & 0xff;
    compensation[3] = (relativeHumidity >> 8) & 0xff;

    return write(ENS16X_REGISTER_ADDRESS_TEMP_IN, compensation, sizeof(compensation));
}

static inline Result Ens16x_WriteConfiguration(ScioSense_Ens16x* ens16x, const Ens16x_Configuration configuration)
{
    Result result;

    result = write(ENS16X_REGISTER_ADDRESS_CONFIG, &configuration, sizeof(configuration));
    if (result == RESULT_OK)
    {
        ens16x->configuration = configuration;
    }

    return result;
}

static inline Result Ens16x_ReadFirmwareVersion(ScioSense_Ens16x* ens16x)
{
    Result result;

    //wait for long running op-modes
    {
        for (int i = 0; i < 30; i++)
        {
            Ens16x_Invoke(ens16x, ENS16X_COMMAND_CLEAR_GENERAL_PURPOSE_REGISTERS, ENS16X_SYSTEM_TIMING_CLEAR_GENERAL_PURPOSE_REGISTERS);
            Ens16x_Read(ens16x, ENS16X_REGISTER_ADDRESS_GPR_READ_4, ens16x->firmwareVersion, 3);
            if
            (
                ens16x->firmwareVersion[0] == 0
             && ens16x->firmwareVersion[1] == 0
             && ens16x->firmwareVersion[2] == 0
            )
            {
                break;
            }

            wait(ENS16X_SYSTEM_TIMING_STANDARD_MEASURE);
        }
    }

    result = Ens16x_Invoke(ens16x, ENS16X_COMMAND_GET_FIRMWARE_VERSION, ENS16X_SYSTEM_TIMING_RESET);
    if (result == RESULT_OK)
    {
        result = Ens16x_Read(ens16x, ENS16X_REGISTER_ADDRESS_GPR_READ_4, ens16x->firmwareVersion, sizeof(ens16x->firmwareVersion));
    }

    return result;
}

static inline Result Ens16x_ReadPartId(ScioSense_Ens16x* ens16x)
{
    Result result;
    uint8_t partIdData[2];

    result= Ens16x_Read(ens16x, ENS16X_REGISTER_ADDRESS_PART_ID, partIdData, 2);
    if (result == RESULT_OK)
    {
        ens16x->partId= ((uint16_t)partIdData[1] << 8) + partIdData[0];
    }

    return result;
}

static inline Result Ens16x_ReadOpMode(ScioSense_Ens16x* ens16x)
{
    return Ens16x_Read(ens16x, ENS16X_REGISTER_ADDRESS_OPMODE, &ens16x->operatingMode, sizeof(ens16x->operatingMode));
}

static inline Result Ens16x_SetOperatingMode(ScioSense_Ens16x* ens16x, const Ens16x_OperatingMode mode)
{
    Result result;

    // Before switching to a measurement mode, the idle mode must be activated.
    if (    (mode != ENS16X_OPERATING_MODE_IDLE)
        &&  (mode != ENS16X_OPERATING_MODE_RESET)
        &&  (ens16x->operatingMode != ENS16X_OPERATING_MODE_IDLE)
    )
    {
        result = Ens16x_SetOperatingMode(ens16x, ENS16X_OPERATING_MODE_IDLE);
        if (result != RESULT_OK)
        {
            return result;
        }
    }

    result = write(ENS16X_REGISTER_ADDRESS_OPMODE, &mode, sizeof(mode));
    if (result == RESULT_OK)
    {
        ens16x->operatingMode = mode;
    }

    return result;
}

static inline Result Ens16x_Wait(ScioSense_Ens16x* ens16x)
{
    switch (ens16x->operatingMode)
    {
        case ENS16X_OPERATING_MODE_STANDARD : wait(ENS16X_SYSTEM_TIMING_STANDARD_MEASURE); break;
        default                             : return RESULT_NOT_ALLOWED;
    }

    return RESULT_OK;
}

static inline Result Ens16x_ValidateDataIntegrity (ScioSense_Ens16x* ens16x)
{
    Result result;
    uint8_t misr;

    result= Ens16x_Read(ens16x, ENS16X_REGISTER_ADDRESS_DATA_MISR, &misr, 1);
    if(result == RESULT_OK)
    {
        if(misr != ens16x->misr)
        {
            result          = RESULT_CHECKSUM_ERROR;
            ens16x->misr    = misr;
        }
    }

    return result;
}

static inline Result Ens16x_Invoke(ScioSense_Ens16x* ens16x, const Ens16x_Command command, uint32_t ms)
{
    Result result = RESULT_OK;
    uint8_t retry = 3;

    if (ens16x->operatingMode != ENS16X_OPERATING_MODE_IDLE)
    {
        result = Ens16x_SetOperatingMode(ens16x, ENS16X_OPERATING_MODE_IDLE);
    }

    if (result == RESULT_OK)
    {
        Ens16x_Command cmd;

        cmd = ENS16X_COMMAND_NOP;
        write(ENS16X_REGISTER_ADDRESS_COMMAND, &cmd, sizeof(cmd));

        cmd = ENS16X_COMMAND_CLEAR_GENERAL_PURPOSE_REGISTERS;
        write(ENS16X_REGISTER_ADDRESS_COMMAND, &cmd, sizeof(cmd));

        wait(ENS16X_SYSTEM_TIMING_CLEAR_GENERAL_PURPOSE_REGISTERS);

        result = write(ENS16X_REGISTER_ADDRESS_COMMAND, &command, sizeof(command));
        if(result == RESULT_OK)
        {
            wait(ms);

            // confirm successful execution
            for(uint8_t i=0; i<retry; i++)
            {
                uint8_t checksum;

                wait(8);
                result= Ens16x_Read(ens16x, ENS16X_REGISTER_ADDRESS_GPR_READ_7, &checksum, 1);
                if(checksum==(command+1))
                {
                    break;
                }
                else
                {
                    result= RESULT_INVALID;
                }
            }
        }
    }

    return result;
}

static inline bool Ens16x_IsConnected(ScioSense_Ens16x* ens16x)
{
    return (ens16x->partId >= 0x160) && (ens16x->partId < 0x170);
}

static inline bool Ens160_IsConnected(ScioSense_Ens16x* ens16x)
{
    return ens16x->partId == 0x160;
}

static inline AirQualityIndex_UBA Ens16x_GetAirQualityIndex_UBA(ScioSense_Ens16x* ens16x)
{
    static const uint8_t mask = 7; // 0b00000111
    return (AirQualityIndex_UBA)(ens16x->dataBuffer[ENS16X_BUFFER_INFO_AQI_INDEX] & mask);
}

static inline uint16_t Ens16x_GetEco2(ScioSense_Ens16x* ens16x)
{
    return (ens16x->dataBuffer[ENS16X_BUFFER_INFO_ECO2_INDEX + 1] << 8) + ens16x->dataBuffer[ENS16X_BUFFER_INFO_ECO2_INDEX];
}

static inline uint16_t Ens16x_GetTvoc(ScioSense_Ens16x* ens16x)
{
    return (ens16x->dataBuffer[ENS16X_BUFFER_INFO_TVOC_INDEX + 1] << 8) + ens16x->dataBuffer[ENS16X_BUFFER_INFO_TVOC_INDEX];
}

static inline uint32_t calculateResistance(const uint16_t r)
{
    return (uint32_t)pow(2, (float)(r) / 2048);
}

static inline uint32_t Ens16x_GetRs0(ScioSense_Ens16x* ens16x)
{
    uint16_t r = (ens16x->resistanceBuffer[ENS16X_BUFFER_INFO_RS0_INDEX + 1] << 8) + ens16x->resistanceBuffer[ENS16X_BUFFER_INFO_RS0_INDEX];
    return calculateResistance(r);
}

static inline uint32_t Ens16x_GetRs1(ScioSense_Ens16x* ens16x)
{
    uint16_t r = (ens16x->resistanceBuffer[ENS16X_BUFFER_INFO_RS1_INDEX + 1] << 8) + ens16x->resistanceBuffer[ENS16X_BUFFER_INFO_RS1_INDEX];
    return calculateResistance(r);
}

static inline uint32_t Ens16x_GetRs2(ScioSense_Ens16x* ens16x)
{
    uint16_t r = (ens16x->resistanceBuffer[ENS16X_BUFFER_INFO_RS2_INDEX + 1] << 8) + ens16x->resistanceBuffer[ENS16X_BUFFER_INFO_RS2_INDEX];
    return calculateResistance(r);
}

static inline uint32_t Ens16x_GetRs3(ScioSense_Ens16x* ens16x)
{
    uint16_t r = (ens16x->resistanceBuffer[ENS16X_BUFFER_INFO_RS3_INDEX + 1] << 8) + ens16x->resistanceBuffer[ENS16X_BUFFER_INFO_RS3_INDEX];
    return calculateResistance(r);
}

static inline Ens16x_DeviceStatus Ens16x_GetDeviceStatus(ScioSense_Ens16x* ens16x)
{
    return ens16x->deviceStatus;
}

static inline uint8_t* Ens16x_GetFirmwareVersion(ScioSense_Ens16x* ens16x)
{
    return ens16x->firmwareVersion;
}

static inline uint8_t* Ens16x_GetDataRaw(ScioSense_Ens16x* ens16x)
{
    return ens16x->dataBuffer;
}

static inline uint16_t Ens16x_CalcTempInFromKelvin(const float temperature)
{
    return (uint16_t) (temperature * 64.f);
}

static inline uint16_t Ens16x_CalcTempInFromCelsius(const float temperature)
{
    return Ens16x_CalcTempInFromKelvin(temperature + 273.15f);
}

static inline uint16_t Ens16x_CalcTempInFromFahrenheit(const float temperature)
{
    return Ens16x_CalcTempInFromKelvin((5.f / 9.f) * (temperature + 459.67f));
}

static inline uint16_t Ens16x_CalcRhIn(float relativeHumidity)
{
    return (uint16_t)(relativeHumidity * 512.f);
}

#undef write
#undef wait
#undef hasAnyFlag
#undef hasFlag

#endif // SCIOSENSE_ENS160_C_INL