#include <Arduino.h>
#include <Wire.h>
#include <ScioSense_ENS16x.h>
#include <ScioSense_ENS21x.h>

#define ENS161_I2C_ADDRESS 0x52

ENS161 ens161;
ENS210 ens210;

void setup()
{
    Serial.begin(9600);
    ens161.enableDebugging(Serial);

    Wire.begin();
    ens161.begin(&Wire, ENS161_I2C_ADDRESS);
	ens210.begin();

    Serial.println("begin ens161..");
    while (ens161.init() != true)
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("success");

    Serial.println("begin ens210..");
    while (ens210.init() != true)
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

    if (ens210.singleShotMeasure() == RESULT_OK)
    {
        Serial.print("Temp[°C]: "); Serial.print(ens210.getTempCelsius());
        Serial.print("\tHumidity[%RH]: "); Serial.print(ens210.getHumidityPercent());

        ens161.writeCompensation(ens210.getDataT(), ens210.getDataH());
    }

     ens161.wait();

     if (ens161.update() == RESULT_OK)
     {
        if (ens161.hasNewData())
        {
            Serial.print("\tAQI ScioSense:"); Serial.print(ens161.getAirQualityIndex_ScioSense());

            Serial.print("\tTVOC:"); Serial.print(ens161.getTvoc());
            Serial.print("\tECO2: "); Serial.print(ens161.getEco2());
        }

        if (ens161.hasNewGeneralPurposeData())
        {
            Serial.print("\tRS0:"); Serial.print(ens161.getRs0());
            Serial.print("\tRS1:"); Serial.print(ens161.getRs1());
            Serial.print("\tRS2:"); Serial.print(ens161.getRs2());
            Serial.print("\tRS3:"); Serial.print(ens161.getRs3());
        }

        Serial.println("");
     }
}