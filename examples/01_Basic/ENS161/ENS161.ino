#include <Arduino.h>
#include <Wire.h>
#include <ScioSense_ENS16x.h>

#define I2C_ADDRESS 0x52

// #define USE_INTERRUPT		// enable in case you're using a dedicated interrupt pin
#define INTN 2

ENS161 ens16x;

void setup() {
    Serial.begin(9600);
    ens16x.enableDebugging(Serial);

    Wire.begin();
    ens16x.begin(&Wire, I2C_ADDRESS);

    Serial.println("begin..");
    while (ens16x.init() != true) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("success");

#ifdef USE_INTERRUPT
    ens16x.setInterruptPin(INTN);
    ens16x.writeConfiguration(
      ENS16X_CONFIGURATION_INTERRUPT_ENABLE
      | ENS16X_CONFIGURATION_NEW_GENERAL_PURPOSE_DATA
      | ENS16X_CONFIGURATION_NEW_DATA);
#endif


    // select one operating mode
    ens16x.startStandardMeasure();
    //ens16x.startLowPowerMeasure();
    //ens16x.startUltraLowPowerMeasure();
}

void loop() {
    ens16x.wait();

    // Enable Tools->Serial Plotter to see the sensor output as a graph

    if (ens16x.update() == RESULT_OK) {
        if (ens16x.hasNewData()) {
            Serial.print("AQI-U:");
            Serial.print((uint8_t)ens16x.getAirQualityIndex_UBA());
            Serial.print("\tAQI-S:");
            Serial.print((uint8_t)ens16x.getAirQualityIndex_ScioSense());
            Serial.print("\tTVOC:");
            Serial.print(ens16x.getTvoc());
            Serial.print("\tECO2:");
            Serial.println(ens16x.getEco2());
        }

        if (ens16x.hasNewGeneralPurposeData()) {
            Serial.print("RS0:");
            Serial.print(ens16x.getRs0());
            Serial.print("\tRS1:");
            Serial.print(ens16x.getRs1());
            Serial.print("\tRS2:");
            Serial.print(ens16x.getRs2());
            Serial.print("\tRS3:");
            Serial.println(ens16x.getRs3());
        }
    }
}