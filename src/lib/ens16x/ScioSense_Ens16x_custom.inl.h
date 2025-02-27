#ifndef SCIOSENSE_ENS16X_CUSTOM_INL_H
#define SCIOSENSE_ENS16X_CUSTOM_INL_H

#include "ScioSense_Ens16x_custom.h"

#define write(address, data, len)   ens16x->io.write(ens16x->io.config, (address), (uint8_t*)(data), (len))
#define wait(ms)                    ens16x->io.wait(ms)

inline Result Ens16x_StartCustomMeasure(ScioSense_Ens16x* ens16x)
{
    return Ens16x_SetOperatingMode(ens16x, ENS16X_OPERATING_MODE_CUSTOM);
}

inline Result Ens16x_BeginWriteCustomSequence(ScioSense_Ens16x* ens16x)
{
    Ens16x_Reset(ens16x);
    return Ens16x_Invoke(ens16x, ENS16X_COMMAND_SET_SEQUENCE, 0);
}

inline Result Ens16x_WriteStep(ScioSense_Ens16x* ens16x, const Ens16x_SequenceStep step)
{
    uint8_t readResult  = 0;
    bool isLastStep     = (step[7] == 0x80);

    if (step[6] >= ENS16X_BUFFER_INFO_MAX_SEQUENCE_STEPS)
    {
        return RESULT_NOT_ALLOWED;
    }

    write(ENS16X_REGISTER_ADDRESS_GPR_WRITE, step, sizeof(Ens16x_SequenceStep));
    wait(ENS16X_SYSTEM_TIMING_WRITE_STEP_DELAY);
    Ens16x_Read(ens16x, ENS16X_REGISTER_ADDRESS_GPR_READ_7, &readResult, 1);

    uint8_t expectedResult = (isLastStep ? 0xC0 : 0x80) | step[6];
    if (readResult == expectedResult)
    {
        return RESULT_OK;
    }
    else
    {
        return RESULT_CHECKSUM_ERROR;
    }
}

inline Result Ens16x_WriteSequence(ScioSense_Ens16x* ens16x, const Ens16x_Sequence sequence, const uint8_t len)
{
    Result result;

    // set IDs and end flags
    for (uint8_t i=0; i<len; i++)
    {
        Ens16x_SetStepId(sequence[i], i);
        Ens16x_SetEndFlag(sequence[i], false);
    }
    Ens16x_SetEndFlag(sequence[len-1], true);

    result = Ens16x_BeginWriteCustomSequence(ens16x);
    if (result != RESULT_OK)
    {
        return result;
    }

    for (uint8_t i = 0; i < len; i++)
    {
       result=  Ens16x_WriteStep(ens16x, sequence[i]);
       if (result != RESULT_OK)
       {
           return result;
       }
    }

    return result;
}

inline void Ens16x_SetH0(Ens16x_SequenceStep step, const uint8_t value)
{
    step[2] = value;
}

inline void Ens16x_SetH1(Ens16x_SequenceStep step, const uint8_t value)
{
    step[3] = value;
}

inline void Ens16x_SetH2(Ens16x_SequenceStep step, const uint8_t value)
{
    step[4] = value;
}

inline void Ens16x_SetH3(Ens16x_SequenceStep step, const uint8_t value)
{
    step[5] = value;
}

inline void Ens16x_SetH0InCelsius(Ens16x_SequenceStep step, const uint16_t temparature)
{
    Ens16x_SetH0(step, (uint8_t)(temparature >> 1));
}

inline void Ens16x_SetH1InCelsius(Ens16x_SequenceStep step, const uint16_t temparature)
{
    Ens16x_SetH1(step, (uint8_t)(temparature >> 1));
}

inline void Ens16x_SetH2InCelsius(Ens16x_SequenceStep step, const uint16_t temparature)
{
    Ens16x_SetH2(step, (uint8_t)(temparature >> 1));
}

inline void Ens16x_SetH3InCelsius(Ens16x_SequenceStep step, const uint16_t temparature)
{
    Ens16x_SetH3(step, (uint8_t)(temparature >> 1));
}

inline void Ens16x_SetDuration(Ens16x_SequenceStep step, uint16_t duration)
{
    duration    = (ENS16X_BUFFER_INFO_MIN_STEP_DURATION > duration) ? ENS16X_BUFFER_INFO_MIN_STEP_DURATION : duration;
    step[0]     = (uint8_t)(step[0] & 0b00111111) | (uint8_t)((duration & 0x0003) << 6);
    step[1]     = (uint8_t)((duration >> 2) & 0x00ff);
}

inline void Ens16x_SetDurationInMs(Ens16x_SequenceStep step, uint32_t duration)
{
    Ens16x_SetDuration(step, (duration / 24) - 1);
}

inline void Ens16x_SetMeasureFlags(Ens16x_SequenceStep step, const Ens16x_MeasureFlags value)
{
    step[0] = (uint8_t)(step[0] & 0b11000000) | (uint8_t)value;
}

inline void Ens16x_SetStepId(Ens16x_SequenceStep step, const uint8_t id)
{
    step[6] = id;
}

inline void Ens16x_SetEndFlag(Ens16x_SequenceStep step, const bool isLastStep)
{
    step[7] = isLastStep ? 0x80 : 0;
}

inline void Ens16x_SetStep(Ens16x_SequenceStep step, const uint16_t durationInMs, const Ens16x_MeasureFlags flags, const uint16_t H0InC, const uint16_t H1InC, const uint16_t H2InC, const uint16_t H3InC)
{
    Ens16x_SetDurationInMs(step, durationInMs);
    Ens16x_SetMeasureFlags(step, flags);
    Ens16x_SetH0InCelsius(step, H0InC);
    Ens16x_SetH1InCelsius(step, H1InC);
    Ens16x_SetH2InCelsius(step, H2InC);
    Ens16x_SetH3InCelsius(step, H3InC);
}

inline uint8_t  Ens16x_GetStepId(ScioSense_Ens16x* ens16x)
{
    return ens16x->dataBuffer[0];
}

#undef write
#undef wait

#endif // SCIOSENSE_ENS16X_CUSTOM_INL_H