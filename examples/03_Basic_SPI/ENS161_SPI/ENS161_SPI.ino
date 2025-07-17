/*
    Basic example for ScioSense ENS161 using SPI Interface on ESP32 DevMod
*/

#include <Arduino.h>
#include <ScioSense_ENS16x.h>

//#define USE_INTERRUPT
#define PIN_INTN  2  // define pin for interrupt
#define PIN_CS    5  // define pin for chip select

ENS161 ens161;

void setup() {
    Serial.begin(9600);
    ens161.enableDebugging(Serial);

    SPI.begin();
    ens161.begin(&SPI, PIN_CS);

    Serial.println("begin..");
    while (ens161.init() != true) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("success");

#ifdef USE_INTERRUPT
    ens161.setInterruptPin(PIN_INTN);
    ens161.writeConfiguration(
      ENS16X_CONFIGURATION_INTERRUPT_ENABLE
      | ENS16X_CONFIGURATION_NEW_GENERAL_PURPOSE_DATA
      | ENS16X_CONFIGURATION_NEW_DATA);
#endif

    ens161.startStandardMeasure();
}

void loop() {
    ens161.wait();

    // Enable Tools->Serial Plotter to see the sensor output as a graph

    if (ens161.update() == RESULT_OK) {
        if (ens161.hasNewData()) {
            Serial.print("AQI UBA:");
            Serial.print((uint8_t)ens161.getAirQualityIndex_UBA());

            Serial.print("\tTVOC:");
            Serial.print(ens161.getTvoc());
            Serial.print("\tECO2:");
            Serial.println(ens161.getEco2());
        }

        if (ens161.hasNewGeneralPurposeData()) {
            Serial.print("RS0:");
            Serial.print(ens161.getRs0());
            Serial.print("\tRS1:");
            Serial.print(ens161.getRs1());
            Serial.print("\tRS2:");
            Serial.print(ens161.getRs2());
            Serial.print("\tRS3:");
            Serial.println(ens161.getRs3());
        }
    }
}
