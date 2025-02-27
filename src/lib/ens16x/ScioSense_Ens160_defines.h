#ifndef SCIOSENSE_ENS160_DEFINES_C_H
#define SCIOSENSE_ENS160_DEFINES_C_H

#include <inttypes.h>

//// Register Address
typedef uint8_t Ens16x_RegisterAddress;
#define ENS16X_REGISTER_ADDRESS_PART_ID        (0x00)                                   // This 2-byte register contains the part number in little endian of the ENS16x.
#define ENS16X_REGISTER_ADDRESS_OPMODE         (0x10)                                   // This 1-byte register sets the Operating Mode of the ENS16x. The Host System can write a new OPMODE at any time.
#define ENS16X_REGISTER_ADDRESS_CONFIG         (0x11)                                   // This 1-byte register configures the action of the INTn pin which allows the ENS16x to signal to the host system that data is available
#define ENS16X_REGISTER_ADDRESS_COMMAND        (0x12)                                   // This 1-byte register allows some additional commands to be executed on the ENS16x
#define ENS16X_REGISTER_ADDRESS_TEMP_IN        (0x13)                                   // This 2-byte register allows the host system to write ambient temperature data to ENS16x for compensation.
#define ENS16X_REGISTER_ADDRESS_RH_IN          (0x15)                                   // This 2-byte register allows the host system to write relative humidity data to ENS16x for compensation.
#define ENS16X_REGISTER_ADDRESS_DEVICE_STATUS  (0x20)                                   // This 1-byte register indicates the current status of the ENS16x.
#define ENS16X_REGISTER_ADDRESS_DATA_AQI       (0x21)                                   // This 1-byte register reports the calculated Air Quality Index.
#define ENS16X_REGISTER_ADDRESS_DATA_TVOC      (0x22)                                   // This 2-byte register reports the calculated TVOC concentration in ppb.
#define ENS16X_REGISTER_ADDRESS_DATA_ECO2      (0x24)                                   // This 2-byte register reports the calculated equivalent CO2-concentration in ppm, based on the detected VOCs and hydrogen.
#define ENS16X_REGISTER_ADDRESS_DATA_T         (0x30)                                   // This 2-byte register reports the temperature used in its calculations (taken from TEMP_IN, if supplied).
#define ENS16X_REGISTER_ADDRESS_DATA_H         (0x32)                                   // This 2-byte register reports the relative humidity used in its calculations (taken from RH_IN if supplied).
#define ENS16X_REGISTER_ADDRESS_DATA_MISR      (0x38)                                   // This 1-byte register reports the calculated checksum of the previous DATA_* read transaction (of n-bytes).

#define ENS16X_REGISTER_ADDRESS_GPR_WRITE      (0x40)                                   // This 8-byte register is used by several functions for the Host System to pass data to the ENS16x.
#define ENS16X_REGISTER_ADDRESS_GPR_WRITE_0    (0 + ENS16X_REGISTER_ADDRESS_GPR_WRITE)
#define ENS16X_REGISTER_ADDRESS_GPR_WRITE_1    (1 + ENS16X_REGISTER_ADDRESS_GPR_WRITE)
#define ENS16X_REGISTER_ADDRESS_GPR_WRITE_2    (2 + ENS16X_REGISTER_ADDRESS_GPR_WRITE)
#define ENS16X_REGISTER_ADDRESS_GPR_WRITE_3    (3 + ENS16X_REGISTER_ADDRESS_GPR_WRITE)
#define ENS16X_REGISTER_ADDRESS_GPR_WRITE_4    (4 + ENS16X_REGISTER_ADDRESS_GPR_WRITE)
#define ENS16X_REGISTER_ADDRESS_GPR_WRITE_5    (5 + ENS16X_REGISTER_ADDRESS_GPR_WRITE)
#define ENS16X_REGISTER_ADDRESS_GPR_WRITE_6    (6 + ENS16X_REGISTER_ADDRESS_GPR_WRITE)
#define ENS16X_REGISTER_ADDRESS_GPR_WRITE_7    (7 + ENS16X_REGISTER_ADDRESS_GPR_WRITE)

#define ENS16X_REGISTER_ADDRESS_GPR_READ       (0x48)                                   // This 8-byte register is used by several functions for the ENS16x to pass data to the Host System.
#define ENS16X_REGISTER_ADDRESS_GPR_READ_0     (0 + ENS16X_REGISTER_ADDRESS_GPR_READ)
#define ENS16X_REGISTER_ADDRESS_GPR_READ_1     (1 + ENS16X_REGISTER_ADDRESS_GPR_READ)
#define ENS16X_REGISTER_ADDRESS_GPR_READ_2     (2 + ENS16X_REGISTER_ADDRESS_GPR_READ)
#define ENS16X_REGISTER_ADDRESS_GPR_READ_3     (3 + ENS16X_REGISTER_ADDRESS_GPR_READ)
#define ENS16X_REGISTER_ADDRESS_GPR_READ_4     (4 + ENS16X_REGISTER_ADDRESS_GPR_READ)
#define ENS16X_REGISTER_ADDRESS_GPR_READ_5     (5 + ENS16X_REGISTER_ADDRESS_GPR_READ)
#define ENS16X_REGISTER_ADDRESS_GPR_READ_6     (6 + ENS16X_REGISTER_ADDRESS_GPR_READ)
#define ENS16X_REGISTER_ADDRESS_GPR_READ_7     (7 + ENS16X_REGISTER_ADDRESS_GPR_READ)

//// Operating Mode
typedef uint8_t Ens16x_OperatingMode;
#define ENS16X_OPERATING_MODE_DEEP_SLEEP   (0x00)                                  // low-power standby (DEEP_SLEEP)
#define ENS16X_OPERATING_MODE_IDLE         (0x01)                                  // low-power
#define ENS16X_OPERATING_MODE_STANDARD     (0x02)                                  // Gas Sensing Mode
#define ENS16X_OPERATING_MODE_RESET        (0xF0)                                  // Device reset to the power-on configuration.
#define ENS16X_OPERATING_MODE_UNKNOWN      (0xFF)                                  // Internal

//// Configuration
typedef uint8_t Ens16x_Configuration;
#define ENS16X_CONFIGURATION_INTERRUPT_POLARITY        (1 << 6)                    // pin polarity: 0: Active low (Default) 1: Active high
#define ENS16X_CONFIGURATION_INTERRUPT_CONFIGURATION   (1 << 5)                    // pin drive: 0: Open drain 1: Push / Pull
#define ENS16X_CONFIGURATION_NEW_GENERAL_PURPOSE_DATA  (1 << 3)                    // pin asserted when new data is presented in the General Purpose Read Registers
#define ENS16X_CONFIGURATION_NEW_DATA                  (1 << 1)                    // pin asserted when new data is presented in the DATA_XXX Registers
#define ENS16X_CONFIGURATION_INTERRUPT_ENABLE          (1 << 0)                    // pin is enabled for the functions above
#define ENS16X_CONFIGURATION_DEFAULT                   (0)                         // Default setting for the configuration register

//// Command
typedef uint8_t Ens16x_Command;
#define ENS16X_COMMAND_NOP                             (0x00)                      // Clears the Command Register (ENS160_COMMAND_NOP)
#define ENS16X_COMMAND_GET_FIRMWARE_VERSION            (0x0E)                      // Get FW Version (ENS160_COMMAND_GET_APPVER)
#define ENS16X_COMMAND_CLEAR_GENERAL_PURPOSE_REGISTERS (0xCC)                      // Clears GPR Read Registers (ENS160_COMMAND_CLRGPR)

//// Device Status
typedef uint8_t Ens16x_DeviceStatus;
#define ENS16X_DEVICE_STATUS_ACTIVE                    (1 << 7)                    // High indicates that an OPMODE is running (STATAS)
#define ENS16X_DEVICE_STATUS_ERROR                     (1 << 6)                    // High indicates that an error is detected. E.g. Invalid Operating Mode has been selected. (STATER)
#define ENS16X_DEVICE_STATUS_VALID_HIGH                (1 << 3)                    // 0: Normal operation 1: Warm-Up phase 2: Initial Start-Up phase 3: Invalid output (VALIDITY FLAG)
#define ENS16X_DEVICE_STATUS_VALID_LOW                 (1 << 2)                    // 0: Normal operation 1: Warm-Up phase 2: Initial Start-Up phase 3: Invalid output
#define ENS16X_DEVICE_STATUS_NEW_DATA                  (1 << 1)                    // High indicates that a new data is available in the DATA_x registers. (NEWDAT)
#define ENS16X_DEVICE_STATUS_NEW_GENERAL_PURPOSE_DATA  (1 << 0)                    // High indicates that a new data is available in the GPR_READx registers. (NEWGPR)

//// SystemTiming in ms
#define ENS16X_SYSTEM_TIMING_RESET                             (10)
#define ENS16X_SYSTEM_TIMING_STANDARD_MEASURE                  (1000)
#define ENS16X_SYSTEM_TIMING_CLEAR_GENERAL_PURPOSE_REGISTERS   (2)

//// BufferInfo defines internally used buffer and data indices and sizes (in bytes)
#define ENS16X_BUFFER_INFO_DATA_SIZE               (7)
#define ENS16X_BUFFER_INFO_RESISTANCE_DATA_SIZE    (8)
#define ENS16X_BUFFER_INFO_FIRMWARE_VERSION_SIZE   (3)
#define ENS16X_BUFFER_INFO_AQI_INDEX               (ENS16X_REGISTER_ADDRESS_DATA_AQI    - ENS16X_REGISTER_ADDRESS_DATA_AQI)
#define ENS16X_BUFFER_INFO_TVOC_INDEX              (ENS16X_REGISTER_ADDRESS_DATA_TVOC   - ENS16X_REGISTER_ADDRESS_DATA_AQI)
#define ENS16X_BUFFER_INFO_ECO2_INDEX              (ENS16X_REGISTER_ADDRESS_DATA_ECO2   - ENS16X_REGISTER_ADDRESS_DATA_AQI)
#define ENS16X_BUFFER_INFO_RS0_INDEX               (ENS16X_REGISTER_ADDRESS_GPR_READ    - ENS16X_REGISTER_ADDRESS_GPR_READ)
#define ENS16X_BUFFER_INFO_RS1_INDEX               (ENS16X_REGISTER_ADDRESS_GPR_READ_2  - ENS16X_REGISTER_ADDRESS_GPR_READ)
#define ENS16X_BUFFER_INFO_RS2_INDEX               (ENS16X_REGISTER_ADDRESS_GPR_READ_4  - ENS16X_REGISTER_ADDRESS_GPR_READ)
#define ENS16X_BUFFER_INFO_RS3_INDEX               (ENS16X_REGISTER_ADDRESS_GPR_READ_6  - ENS16X_REGISTER_ADDRESS_GPR_READ)

// UBA Air Quality Index
#ifndef SCIOSENSE_AQI_UBA_CODES
#define SCIOSENSE_AQI_UBA_CODES
typedef uint8_t AirQualityIndex_UBA;
#define AQI_UBA_UNKNOWN         (0)
#define AQI_UBA_EXCELLENT       (1)
#define AQI_UBA_GOOD            (2)
#define AQI_UBA_MODERATE        (3)
#define AQI_UBA_POOR            (4)
#define AQI_UBA_UNHEALTHY       (5)
#endif

//// Result and Errors
#ifndef SCIOSENSE_RESULT_CODES
#define SCIOSENSE_RESULT_CODES
typedef int8_t Result;
#define RESULT_NOT_ALLOWED      (4)     // The requested command is not allowed.
#define RESULT_CHECKSUM_ERROR   (3)     // The value was read, but the checksum over the payload (valid and data) does not match.
#define RESULT_INVALID          (2)     // The value was read, but the data is invalid.
#define RESULT_IO_ERROR         (1)     // There was an IO communication error, read/write the stream failed.
#define RESULT_OK               (0)     // All OK; The value was read, the checksum matches, and data is valid.
#endif

#endif // SCIOSENSE_ENS160_DEFINES_C_H