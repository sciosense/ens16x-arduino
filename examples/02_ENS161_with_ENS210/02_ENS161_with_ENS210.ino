#include <Arduino.h>
#include <Wire.h>
#include <ScioSense_ENS16x.h>
#include <ScioSense_ENS21x.h>

#include "ens16x_i2c_interface.h"

using namespace ScioSense;

#define I2C_ADDRESS 0x52
I2cInterface i2c;

ENS161 ens161;
ENS210 ens210;

void setup()
{
    Serial.begin(9600);
    ens161.enableDebugging(Serial);

    Wire.begin();
    i2c.begin(Wire, I2C_ADDRESS);

    Serial.println("begin ens161..");
    while (ens161.begin(&i2c) != true)
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("success");

    Serial.println("begin ens210..");
    while (ens210.begin() != true)
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("success");

    ens161.startStandardMeasure();
}

void loop()
{

    // Enable Tools->Serial Plotter to see the sensor output as a graph

    if (ens210.singleShotMeasure() == ENS210::Result::STATUS_OK)
    {
        Serial.print("Temp[°C]: "); Serial.print(ens210.getTempCelsius());
        Serial.print("\tHumidity[%RH]: "); Serial.print(ens210.getHumidityPercent());

        ens161.writeCompensation(ens210.getDataT(), ens210.getDataH());
    }

     ens161.wait();

     if (ens161.update() == ENS16x::Result::Ok)
     {
        if (hasFlag(ens161.getDeviceStatus(), ENS16x::DeviceStatus::NewData))
        {
            Serial.print("\tAQI UBA:"); Serial.print((uint8_t)ens161.getAirQualityIndex_UBA());
            Serial.print("\tAQI ScioSense:"); Serial.print(ens161.getAirQualityIndex_ScioSense());

            Serial.print("\tTVOC:"); Serial.print(ens161.getTvoc());
            Serial.print("\tECO2: "); Serial.print(ens161.getEco2());
        }

        if (hasFlag(ens161.getDeviceStatus(), ENS16x::DeviceStatus::NewGeneralPurposeData))
        {
            Serial.print("\tRS0:"); Serial.print(ens161.getRs0());
            Serial.print("\tRS1:"); Serial.print(ens161.getRs1());
            Serial.print("\tRS2:"); Serial.print(ens161.getRs2());
            Serial.print("\tRS3:"); Serial.print(ens161.getRs3());
        }

        Serial.println("");
     }
}