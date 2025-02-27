#ifndef SCIOSENSE_ENS16x_H
#define SCIOSENSE_ENS16x_H

#include <stdint.h>

#include <Arduino.h>
#include <Stream.h>
#include <Wire.h>
#include <SPI.h>

#include "lib/ens16x/ScioSense_Ens16x.h"
#include "lib/io/ScioSense_IOInterface_Arduino_I2C.h"
#include "lib/io/ScioSense_IOInterface_Arduino_Ens16x_SPI.h"

#ifndef NOT_A_PIN
#define NOT_A_PIN 0
#endif

class ENS16x : public ScioSense_Ens16x
{
public:
    ENS16x();
    ~ENS16x();

public:
    inline void begin(TwoWire* wire, const uint8_t address = 0x52);                                // Connnects to ENS16x using the given TwoWire object and address
    inline void begin(SPIClass* spi, const uint8_t chipSelect, const SPISettings settings= SPISettings(2000000, MSBFIRST, SPI_MODE0));  // Connnects to ENS16x using arduinos SPI class
    inline bool init();                                                                             // Resets the device to IDLE and reads PartID and FirmwareVersion
    virtual bool isConnected() = 0;                                                                 // checks if the read PART_ID matches the expected value; returns true, if so.

public:
    inline Result update();                                                                         // Checks the device Status for NEWDAT and NEWGPR; Reads Data if flags are set.
    inline Result reset();                                                                          // Device reset to the power-on configuration.
    inline Result startStandardMeasure();                                                           // Starts the STANDARD operating mode.
    inline Result writeCompensation(const uint16_t& temperature, const uint16_t& relativeHumidity); // Writes ambient temperature and relative humidity data for compensation.
    inline Result writeConfiguration(const Ens16x_Configuration& configuration);                    // Writes configuration data.

public:
    void                        wait();                                                             // Wait for new data; uses the waitInterrupt or waitPolled function depending on the configuration
    virtual void                waitInterrupt();                                                    // Wait until an interrupt occurs; Configuration::InterruptEnable and setInterruptPin must be set, for this to work
    virtual void                waitPolled();                                                       // Wait the operation mode specific response time
    inline AirQualityIndex_UBA  getAirQualityIndex_UBA();                                           // Calculate and return the air quality index (UBA)
    inline uint16_t             getEco2();                                                          // Calculate and return the Equivalent CO2 Concentration (ppm)
    inline uint16_t             getTvoc();                                                          // Calculate and return the TVOC Concentration (ppb)
    inline uint32_t             getRs0();
    inline uint32_t             getRs1();
    inline uint32_t             getRs2();
    inline uint32_t             getRs3();
    inline Ens16x_DeviceStatus  getDeviceStatus();                                                  // Return last DeviceStatus read by update()
    inline bool                 hasNewData();                                                       // Tests if the NewData flag is set in DeviceStatus
    inline bool                 hasNewGeneralPurposeData();                                         // Tests if the NewGeneralPurposeData flag is set in DeviceStatus
    inline uint8_t*             getFirmwareVersion();                                               // return the firmware version number as uint8_t[3] {major, minor, release}
    inline uint8_t*             getDataRaw();
    inline void                 setInterruptPin(int p);                                             // Set INTn interrupt pin

public:
    inline void enableAutoDataIntegrityValidation(bool enable = true);                              // If enabled, data read from the sensor is validated using the MISR register checksum
    inline void enableDebugging(Stream& debugStream);                                               // Enables the debug log. The output is written to the given debugStream
    inline void disableDebugging();                                                                 // Stops the debug log if enabled. Does nothing otherwise.

public:
    inline Result read(const Ens16x_RegisterAddress address, uint8_t* data, size_t size);
    inline Result write(const Ens16x_RegisterAddress address, uint8_t* data, size_t size);

public:
    inline Result setOperatingMode(const Ens16x_OperatingMode& mode);
    inline Result invoke(const Ens16x_Command& command, unsigned long ms = 0);

protected:
    const char* debugPrefix = "ENS16x debug -- ";
    inline const char* toString(const Result& result);
    inline void debug(const char* msg);
    inline void debug(const char* msg, Result& result);
    inline void debug(const char* msg, uint8_t* data, size_t size, Result& result);
    template<class T> inline void debug(const char* msg, T data);

protected:
    ScioSense_Arduino_I2c_Config        i2cConfig;
    ScioSense_Arduino_Ens16x_Spi_Config spiConfig;
    uint8_t interruptPin;
    bool autoMisr;

private:
    Stream* debugStream;
};

#include "ens16x.inl.h"

#endif //SCIOSENSE_ENS16x_H