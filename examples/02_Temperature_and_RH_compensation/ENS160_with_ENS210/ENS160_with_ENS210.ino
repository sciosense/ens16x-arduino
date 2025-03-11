#include <Arduino.h>
#include <Wire.h>
#include <ScioSense_ENS16x.h>
#include <ScioSense_ENS21x.h>

#define ENS160_I2C_ADDRESS 0x52

ENS160 ens160;
ENS210 ens210;

void setup()
{
    Serial.begin(9600);
    ens160.enableDebugging(Serial);

    Wire.begin();
    ens160.begin(&Wire, ENS160_I2C_ADDRESS);
	ens210.begin();

    Serial.println("begin ens160..");
    while (ens160.init() != true)
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

    ens160.startStandardMeasure();
}

void loop()
{

    // Enable Tools->Serial Plotter to see the sensor output as a graph

    if (ens210.singleShotMeasure() == RESULT_OK)
    {
        Serial.print("Temp[°C]: "); Serial.print(ens210.getTempCelsius());
        Serial.print("\tHumidity[%RH]: "); Serial.print(ens210.getHumidityPercent());

        ens160.writeCompensation(ens210.getDataT(), ens210.getDataH());
    }

     ens160.wait();

     if (ens160.update() == RESULT_OK)
     {
        if (ens160.hasNewData())
        {
            Serial.print("\tAQI UBA:"); Serial.print((uint8_t)ens160.getAirQualityIndex_UBA());

            Serial.print("\tTVOC:"); Serial.print(ens160.getTvoc());
            Serial.print("\tECO2: "); Serial.print(ens160.getEco2());
        }

        if (ens160.hasNewGeneralPurposeData())
        {
            Serial.print("\tRS0:"); Serial.print(ens160.getRs0());
            Serial.print("\tRS1:"); Serial.print(ens160.getRs1());
            Serial.print("\tRS2:"); Serial.print(ens160.getRs2());
            Serial.print("\tRS3:"); Serial.print(ens160.getRs3());
        }

        Serial.println("");
     }
}