/*
    Basic example for ScioSense ENS160 using SPI Interface on ESP32 DevMod
*/

#include <Arduino.h>
#include <ScioSense_ENS16x.h>


//#define USE_INTERRUPT
#define PIN_INTN  2  // define pin for interrupt
#define PIN_CS    5  // define pin for chip select

ENS160 ens160;

void setup()
{
    Serial.begin(9600);
    ens160.enableDebugging(Serial);

    SPI.begin();
    ens160.begin(&SPI, PIN_CS);

    Serial.println("begin..");
    while (ens160.init() != true)
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("success");

#ifdef USE_INTERRUPT
    ens160.setInterruptPin(INTN);
    ens160.writeConfiguration
    (
          ENS16X_CONFIGURATION_INTERRUPT_ENABLE
        | ENS16X_CONFIGURATION_NEW_GENERAL_PURPOSE_DATA
        | ENS16X_CONFIGURATION_NEW_DATA
    );
#endif

    ens160.startStandardMeasure();
}

void loop()
{
     ens160.wait();

     // Enable Tools->Serial Plotter to see the sensor output as a graph

     if (ens160.update() == RESULT_OK)
     {
        if (ens160.hasNewData())
        {
            Serial.print("AQI UBA:"); Serial.print((uint8_t)ens160.getAirQualityIndex_UBA());

            Serial.print("\tTVOC:"); Serial.print(ens160.getTvoc());
            Serial.print("\tECO2:"); Serial.println(ens160.getEco2());
        }

        if (ens160.hasNewGeneralPurposeData())
        {
            Serial.print("RS0:"); Serial.print(ens160.getRs0());
            Serial.print("\tRS1:"); Serial.print(ens160.getRs1());
            Serial.print("\tRS2:"); Serial.print(ens160.getRs2());
            Serial.print("\tRS3:"); Serial.println(ens160.getRs3());
        }
     }
}
