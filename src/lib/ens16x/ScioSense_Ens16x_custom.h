#ifndef SCIOSENSE_ENS16X_CUSTOM_H
#define SCIOSENSE_ENS16X_CUSTOM_H

#include "ScioSense_Ens160.h"

//// Custom Mode specific Operating Modes
#define ENS16X_OPERATING_MODE_CUSTOM           (0xC0)   // Custom Operating Mode

//// measure settings for Custom Mode steps
typedef uint8_t Ens16x_MeasureFlags;
#define ENS16X_CUSTOM_MEASURE_FLAG_RH          (1 << 0)        //
#define ENS16X_CUSTOM_MEASURE_FLAG_T           (1 << 1)        //
#define ENS16X_CUSTOM_MEASURE_FLAG_H3          (1 << 2)        // measure heat plate 3
#define ENS16X_CUSTOM_MEASURE_FLAG_H2          (1 << 3)        // measure heat plate 2
#define ENS16X_CUSTOM_MEASURE_FLAG_H1          (1 << 4)        // measure heat plate 1
#define ENS16X_CUSTOM_MEASURE_FLAG_H0          (1 << 5)        // measure heat plate 0
#define ENS16X_CUSTOM_MEASURE_FLAG_NONE        (0)             // measure none
#define ENS16X_CUSTOM_MEASURE_FLAG_ALL         (0b1111 << 2);  // measure all heat plates

//// Custom Mode specific Commands
#define ENS16X_COMMAND_SET_SEQUENCE            (0xC2)          // begin to write a custom measure sequence

//// Custom Mode specific System Timing Characteristics
#define ENS16X_SYSTEM_TIMING_WRITE_STEP_DELAY  (2)

//// Custom Mode specific internally used buffer and data indices and sizes (in bytes) 
#define ENS16X_BUFFER_INFO_MIN_STEP_DURATION   (1) // 48ms
#define ENS16X_BUFFER_INFO_MAX_SEQUENCE_STEPS  (20)

typedef uint8_t Ens16x_SequenceStep[8];
typedef Ens16x_SequenceStep* Ens16x_Sequence;

Result Ens16x_StartCustomMeasure        (ScioSense_Ens16x* ens16x);                                 // Starts the OPERATING_MODE_CUSTOM operating mode.
Result Ens16x_BeginWriteCustomSequence  (ScioSense_Ens16x* ens16x);                                 // creates a new custom sequence. ::addStep must be called "numberOfSteps" times to finish the process.
Result Ens16x_WriteStep                 (ScioSense_Ens16x* ens16x, const Ens16x_SequenceStep step);
Result Ens16x_WriteSequence             (ScioSense_Ens16x* ens16x, Ens16x_Sequence sequence, const uint8_t len);

void Ens16x_SetH0                       (Ens16x_SequenceStep step, const uint8_t value);                          // set raw heat plate 0 value
void Ens16x_SetH1                       (Ens16x_SequenceStep step, const uint8_t value);                          // set raw heat plate 1 value
void Ens16x_SetH2                       (Ens16x_SequenceStep step, const uint8_t value);                          // set raw heat plate 2 value
void Ens16x_SetH3                       (Ens16x_SequenceStep step, const uint8_t value);                          // set raw heat plate 3 value
void Ens16x_SetH0InCelsius              (Ens16x_SequenceStep step, const uint16_t temparature);                   // set temperatue of heat plate 0 in celsius
void Ens16x_SetH1InCelsius              (Ens16x_SequenceStep step, const uint16_t temparature);                   // set temperatue of heat plate 1 in celsius
void Ens16x_SetH2InCelsius              (Ens16x_SequenceStep step, const uint16_t temparature);                   // set temperatue of heat plate 2 in celsius
void Ens16x_SetH3InCelsius              (Ens16x_SequenceStep step, const uint16_t temparature);                   // set temperatue of heat plate 3 in celsius
void Ens16x_SetDuration                 (Ens16x_SequenceStep step, uint16_t duration);                            // set raw step duration (max 1023)
void Ens16x_SetDurationInMs             (Ens16x_SequenceStep step, uint32_t duration);                            // set step duration in ms
void Ens16x_SetMeasureFlags             (Ens16x_SequenceStep step, const Ens16x_MeasureFlags value);                     // set step measure settings
void Ens16x_SetStepId                   (Ens16x_SequenceStep step, const uint8_t id);                             // set step id (only needed if the step is written manually with Ens16x_WriteStep. otherwise Ens16x_WriteSequence set it automatically)
void Ens16x_SetEndFlag                  (Ens16x_SequenceStep step, const bool isLastStep);                        // set end of sequence flag (only needed if the step is written manually with Ens16x_WriteStep. otherwise Ens16x_WriteSequence set it automatically)
void Ens16x_SetStep                     (Ens16x_SequenceStep step, const uint16_t durationInMs, const Ens16x_MeasureFlags flags, const uint16_t H0InC, const uint16_t H1InC, const uint16_t H2InC, const uint16_t H3InC); // set configuration for a custom sequence step

uint8_t  Ens16x_GetStepId               (ScioSense_Ens16x* ens16x);                                       // get the stepId of the current measurement. only valid when NewGeneralPurposeData or NewData is set (ENS16x::getDeviceStatus())


#include "ScioSense_Ens16x_custom.inl.h"
#endif // SCIOSENSE_ENS16X_CUSTOM_H