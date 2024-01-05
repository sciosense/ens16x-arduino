#include <Arduino.h>
#include <Wire.h>
#include <ScioSense_ENS16x.h>

#include "ens16x_i2c_interface.h"

using namespace ScioSense;

#define I2C_ADDRESS 0x52
I2cInterface i2c;

#define USE_INTERRUPT
#define INTN 2

ENS160 ens160;

void setup()
{
    Serial.begin(9600);
    ens160.enableDebugging(Serial);

    Wire.begin();
    i2c.begin(Wire, I2C_ADDRESS);

    Serial.println("begin..");
    while (ens160.begin(&i2c) != true)
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("success");

#ifdef USE_INTERRUPT
    ens160.setInterruptPin(INTN);
    ens160.writeConfiguration
    (
          ENS160::Configuration::InterruptEnable
        | ENS160::Configuration::NewGeneralPurposeData
        | ENS160::Configuration::NewData
    );
#endif

    ens160.startStandardMeasure();
}

void loop()
{
     ens160.wait();

     // Enable Tools->Serial Plotter to see the sensor output as a graph

     if (ens160.update() == ENS16x::Result::Ok)
     {
        if (hasFlag(ens160.getDeviceStatus(), ENS16x::DeviceStatus::NewData))
        {
            Serial.print("AQI UBA:"); Serial.print((uint8_t)ens160.getAirQualityIndex_UBA());

            Serial.print("\tTVOC:"); Serial.print(ens160.getTvoc());
            Serial.print("\tECO2:"); Serial.println(ens160.getEco2());
        }

        if (hasFlag(ens160.getDeviceStatus(), ENS16x::DeviceStatus::NewGeneralPurposeData))
        {
            Serial.print("RS0:"); Serial.print(ens160.getRs0());
            Serial.print("\tRS1:"); Serial.print(ens160.getRs1());
            Serial.print("\tRS2:"); Serial.print(ens160.getRs2());
            Serial.print("\tRS3:"); Serial.println(ens160.getRs3());
        }

        
     }
}