#include <Arduino.h>
#include <Wire.h>
#include <ScioSense_ENS16x.h>

#include "ens16x_i2c_interface.h"

using namespace ScioSense;

#define I2C_ADDRESS 0x52
I2cInterface i2c;

#define USE_INTERRUPT
#define INTN 2

ENS161 ens161;

void setup()
{
    Serial.begin(9600);
    ens161.enableDebugging(Serial);

    Wire.begin();
    i2c.begin(Wire, I2C_ADDRESS);

    Serial.println("begin..");
    while (ens161.begin(&i2c) != true)
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("success");

#ifdef USE_INTERRUPT
    ens161.setInterruptPin(INTN);
    ens161.writeConfiguration
    (
          ENS161::Configuration::InterruptEnable
        | ENS161::Configuration::NewGeneralPurposeData
        | ENS161::Configuration::NewData
    );
#endif

    ens161.startStandardMeasure();
}

void loop()
{
     ens161.wait();

     // Enable Tools->Serial Plotter to see the sensor output as a graph

     if (ens161.update() == ENS16x::Result::Ok)
     {
        if (hasFlag(ens161.getDeviceStatus(), ENS16x::DeviceStatus::NewData))
        {
            Serial.print("AQI UBA:"); Serial.print((uint8_t)ens161.getAirQualityIndex_UBA());

            Serial.print("\tTVOC:"); Serial.print(ens161.getTvoc());
            Serial.print("\tECO2:"); Serial.println(ens161.getEco2());
        }

        if (hasFlag(ens161.getDeviceStatus(), ENS16x::DeviceStatus::NewGeneralPurposeData))
        {
            Serial.print("RS0:"); Serial.print(ens161.getRs0());
            Serial.print("\tRS1:"); Serial.print(ens161.getRs1());
            Serial.print("\tRS2:"); Serial.print(ens161.getRs2());
            Serial.print("\tRS3:"); Serial.println(ens161.getRs3());
        }

        
     }
}