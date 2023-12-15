// cleaner version of main which will get renamed back to main when done
//
#include <Arduino.h>

#include "GPSHandler.h"
GPSHandler gps;
// gets pos and speed

#include "CarRadio.h"
CarRadio radio;
// lora radio for long range transmission

#include "CarData.h"
CarData bmw; // object that collects all the data in one place
// current idea is to pass this to all the other sensors and they stick in the new values

#include "AXLhelper.h"
CarAXL axl;
// gets acceleration and temp values

#include "OTAhandler.h"
OTAHandler ota;

void setup()
{
    Serial.begin(115200);
    Serial.println("booting");
    Wire.begin();
    ota.setupWiFi();
    ota.setupOTA();
    gps.begin();   // not ready to test
   // radio.begin(); // not ready to test fails state 0
    axl.begin();   // ready to test
    Serial.println("\n\n we have made a change from wifi \n\n");
}

void loop()
{
    ota.handleOTA();

    axl.updateVals(&bmw);

    // Print the accelerometer data to the Serial Monitor
    Serial.print("X: ");
    Serial.print(bmw.axlX);
    Serial.print(" Y: ");
    Serial.print(bmw.axlY);
    Serial.print(" Z: ");
    Serial.println(bmw.axlZ);

    Serial.println("AHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH");

    gps.updateGPSData(&bmw);
    gps.dumpGPSData();
    delay(500);
}