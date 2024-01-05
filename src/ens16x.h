#ifndef SCIOSENSE_ENS16x_H
#define SCIOSENSE_ENS16x_H

#include <stdint.h>

#include <Arduino.h>
#include <Stream.h>
#include <Wire.h>

#include "utils.h"

#ifndef NOT_A_PIN
#define NOT_A_PIN 0
#endif

namespace ScioSense
{
    class ENS16x
    {
    public:

        // Addresses of the ENS16x registers (named to match the datasheet)
        enum class RegisterAddress : uint8_t
        {
            PART_ID         = 0x00,                     // This 2-byte register contains the part number in little endian of the ENS16x.
            OPMODE          = 0x10,                     // This 1-byte register sets the Operating Mode of the ENS16x. The Host System can write a new OPMODE at any time.
            CONFIG          = 0x11,                     // This 1-byte register configures the action of the INTn pin which allows the ENS16x to signal to the host system that data is available
            COMMAND         = 0x12,                     // This 1-byte register allows some additional commands to be executed on the ENS16x
            TEMP_IN         = 0x13,                     // This 2-byte register allows the host system to write ambient temperature data to ENS16x for compensation.
            RH_IN           = 0x15,                     // This 2-byte register allows the host system to write relative humidity data to ENS16x for compensation.
            DEVICE_STATUS   = 0x20,                     // This 1-byte register indicates the current status of the ENS16x.
            DATA_AQI        = 0x21,                     // This 1-byte register reports the calculated Air Quality Index.
            DATA_TVOC       = 0x22,                     // This 2-byte register reports the calculated TVOC concentration in ppb.
            DATA_ECO2       = 0x24,                     // This 2-byte register reports the calculated equivalent CO2-concentration in ppm, based on the detected VOCs and hydrogen.
            DATA_AQIS       = 0x26,                     // This 2-byte register reports the calculated relative Air Quality Index, proprietary to ScioSense, in arbitrary units between 0 and 500
            DATA_T          = 0x30,                     // This 2-byte register reports the temperature used in its calculations (taken from TEMP_IN, if supplied).
            DATA_H          = 0x32,                     // This 2-byte register reports the relative humidity used in its calculations (taken from RH_IN if supplied).
            DATA_MISR       = 0x38,                     // This 1-byte register reports the calculated checksum of the previous DATA_* read transaction (of n-bytes).

            GPR_WRITE       = 0x40,                     // This 8-byte register is used by several functions for the Host System to pass data to the ENS16x.
            GPR_WRITE_0     = 0 + GPR_WRITE,
            GPR_WRITE_1     = 1 + GPR_WRITE,
            GPR_WRITE_2     = 2 + GPR_WRITE,
            GPR_WRITE_3     = 3 + GPR_WRITE,
            GPR_WRITE_4     = 4 + GPR_WRITE,
            GPR_WRITE_5     = 5 + GPR_WRITE,
            GPR_WRITE_6     = 6 + GPR_WRITE,
            GPR_WRITE_7     = 7 + GPR_WRITE,

            GPR_READ        = 0x48,                     // This 8-byte register is used by several functions for the ENS16x to pass data to the Host System.
            GPR_READ_0      = 0 + GPR_READ,
            GPR_READ_1      = 1 + GPR_READ,
            GPR_READ_2      = 2 + GPR_READ,
            GPR_READ_3      = 3 + GPR_READ,
            GPR_READ_4      = 4 + GPR_READ,
            GPR_READ_5      = 5 + GPR_READ,
            GPR_READ_6      = 6 + GPR_READ,
            GPR_READ_7      = 7 + GPR_READ,
        };

        // Device Operating Modes (OPMODE)
        enum class OperatingMode : uint8_t
        {
            DeepSleep   = 0x00,                         // low-power standby (DEEP_SLEEP)
            Idle        = 0x01,                         // low-power
            Standard    = 0x02,                         // Gas Sensing Mode
            Reset       = 0xF0,                         // Device reset to the power-on configuration.

            Unknown     = 0xFF,                         // Internal
        };

        // Device Interrupt Configuration
        enum class Configuration : uint8_t
        {
            InterruptPolarity       = 1 << 6,           // pin polarity: 0: Active low (Default) 1: Active high
            InterruptConfiguration  = 1 << 5,           // pin drive: 0: Open drain 1: Push / Pull
            NewGeneralPurposeData   = 1 << 3,           // pin asserted when new data is presented in the General Purpose Read Registers
            NewData                 = 1 << 1,           // pin asserted when new data is presented in the DATA_XXX Registers
            InterruptEnable         = 1 << 0,           // pin is enabled for the functions above

            Default                 = 0                 // Default setting for the configuration register
        };

        // Device System Commands
        enum class Command : uint8_t
        {
            Nop                                 = 0x00, // Clears the Command Register (ENS160_COMMAND_NOP)
            GetFirmwareVersion                  = 0x0E, // Get FW Version (ENS160_COMMAND_GET_APPVER)
            ClearGeneralPourposeRegisters       = 0xCC, // Clears GPR Read Registers (ENS160_COMMAND_CLRGPR)
        };

        // Device status register (DEVICE_STATUS).
        enum class DeviceStatus : uint8_t
        {
            Active                  = 1 << 7,           // High indicates that an OPMODE is running (STATAS)
            Error                   = 1 << 6,           // High indicates that an error is detected. E.g. Invalid Operating Mode has been selected. (STATER)
            ValidHigh               = 1 << 3,           // 0: Normal operation 1: Warm-Up phase 2: Initial Start-Up phase 3: Invalid output (VALIDITY FLAG)
            ValidLow                = 1 << 2,           // 0: Normal operation 1: Warm-Up phase 2: Initial Start-Up phase 3: Invalid output
            NewData                 = 1 << 1,           // High indicates that a new data is available in the DATA_x registers. (NEWDAT)
            NewGeneralPurposeData   = 1 << 0,           // High indicates that a new data is available in the GPR_READx registers. (NEWGPR)
        };

        // UBA Air Quality Index
        enum class AirQualityIndex_UBA : byte
        {
            Unknown     = 0,

            Excellent   = 1,
            Good        = 2,
            Moderate    = 3,
            Poor        = 4,
            Unhealthy   = 5,
        };

        // System Timing Characteristics
        struct SystemTiming
        {
            static constexpr uint16_t Reset                         = 10;
            static constexpr uint16_t StandardMeasure               = 1000;
            static constexpr uint16_t ClearGeneralPurposeRegisters  = 2;

        };

        enum class Result : uint8_t
        {
            NotAllowed              = 5,                    // (unused) The requested command is not allowed.
            IOError                 = 4,                    // There was an IO communication error, read/write the stream failed.
            ChecksumError           = 3,                    // (unused) The value was read, but the checksum over the payload (valid and data) does not match.
            Invalid                 = 2,                    // The value was read, but the data is invalid.
            Ok                      = 1                     // All OK; The value was read, the checksum matches, and data is valid.
        };

        // BufferInfo defines internally used buffer and data indices and sizes (in bytes)
        struct BufferInfo
        {
            static constexpr uint8_t DataSize            = 7;
            static constexpr uint8_t ResistanceDataSize  = 8;
            static constexpr uint8_t FirmwareVersionSize = 3;

            static constexpr uint8_t AqiIndex            = (uint8_t)RegisterAddress::DATA_AQI    - (uint8_t)RegisterAddress::DATA_AQI;
            static constexpr uint8_t TvocIndex           = (uint8_t)RegisterAddress::DATA_TVOC   - (uint8_t)RegisterAddress::DATA_AQI;
            static constexpr uint8_t Eco2Index           = (uint8_t)RegisterAddress::DATA_ECO2   - (uint8_t)RegisterAddress::DATA_AQI;
            static constexpr uint8_t Rs0Index            = (uint8_t)RegisterAddress::GPR_READ    - (uint8_t)RegisterAddress::GPR_READ;
            static constexpr uint8_t Rs1Index            = (uint8_t)RegisterAddress::GPR_READ_2  - (uint8_t)RegisterAddress::GPR_READ;
            static constexpr uint8_t Rs2Index            = (uint8_t)RegisterAddress::GPR_READ_4  - (uint8_t)RegisterAddress::GPR_READ;
            static constexpr uint8_t Rs3Index            = (uint8_t)RegisterAddress::GPR_READ_6  - (uint8_t)RegisterAddress::GPR_READ;
        };

    public:
        ENS16x();
        ~ENS16x();

    public:
        bool begin(Utils::IoInterface<RegisterAddress, Result>* ioInterface);                           // Connnects to ENS16x using the given IoInterface object and reads PART_ID; returns the result of isConnected()
        virtual bool isConnected() = 0;                                                                 // checks if the read PART_ID matches the expected value; returns true, if so.


    public:
        Result update();                                                                                // Checks the device Status for NEWDAT and NEWGPR; Reads Data if flags are set.
        Result reset();                                                                                 // Device reset to the power-on configuration.
        Result startStandardMeasure();                                                                  // Starts the STANDARD operating mode.
        Result writeCompensation(const uint16_t& temperature, const uint16_t& relativeHumidity);        // Writes ambient temperature and relative humidity data for compensation.
        Result writeConfiguration(const Configuration& configuration);                                  // Writes configuration data.

    public:
        inline void                 wait();                                                             // Wait for new data; uses the waitInterrupt or waitPolled function depending on the configuration
        virtual void                waitInterrupt();                                                    // Wait until an interrupt occurs; Configuration::InterruptEnable and setInterruptPin must be set, for this to work
        virtual void                waitPolled();                                                       // Wait the operation mode specific response time
        inline AirQualityIndex_UBA  getAirQualityIndex_UBA();                                           // Calculate and return the air quality index (UBA)
        inline uint16_t             getEco2();                                                          // Calculate and return the Equivalent CO2 Concentration (ppm)
        inline uint16_t             getTvoc();                                                          // Calculate and return the TVOC Concentration (ppb)
        inline uint32_t             getRs0();
        inline uint32_t             getRs1();
        inline uint32_t             getRs2();
        inline uint32_t             getRs3();
        inline DeviceStatus         getDeviceStatus();                                                  // Return last DeviceStatus read by update()
        inline uint8_t*             getFirmwareVersion();                                               // return the firmware version number as uint8_t[3] {major, minor, release}
        inline uint8_t*             getDataRaw();
        inline void                 setInterruptPin(int p);                                             // Set INTn interrupt pin

    public:
        void enableDebugging(Stream& debugStream);                                                      // Enables the debug log. The output is written to the given debugStream
        void disableDebugging();                                                                        // Stops the debug log if enabled. Does nothing otherwise.

    public:
        inline Result read(RegisterAddress address, uint8_t* data, size_t size);
        inline Result write(RegisterAddress address, uint8_t* data, size_t size);

    public:
        Result setOperatingMode(const OperatingMode& mode);
        Result invoke(const Command& command, unsigned long ms = 0);
        Result invoke(uint8_t command, unsigned long ms = 0);

    protected:
        template<class T> Result read(RegisterAddress address, T& data);
        template<class T> Result write(RegisterAddress address, const T& data);

    protected:
        Result readFirmwareVersion();

    protected:
        const char* debugPrefix = "ENS16x debug -- ";
        inline const char* toString(const Result& result);
        inline void debug(const char* msg);
        inline void debug(const char* msg, Result& result);
        inline void debug(const char* msg, uint8_t* data, size_t size, Result& result);
        template<class T> inline void debug(const char* msg, T data);

    protected:
        uint8_t dataBuffer[BufferInfo::DataSize];
        uint8_t resistanceBuffer[BufferInfo::ResistanceDataSize];
        uint8_t firmwareVersion[BufferInfo::FirmwareVersionSize] = {};
        uint8_t interruptPin;
        uint16_t partId;
        OperatingMode operatingMode;
        DeviceStatus deviceStatus;
        Configuration configuration;

    private:
        Utils::IoInterface<RegisterAddress, Result>* ioInterface;
        Stream* debugStream;
    };

    DEFINE_ENUM_FLAG_OPERATORS(ENS16x::DeviceStatus, uint8_t);
    DEFINE_ENUM_FLAG_OPERATORS(ENS16x::Configuration, uint8_t);
}

#include "ens16x.inl"

#endif //SCIOSENSE_ENS16x_H