// esp32 based project using platform io inside of vscode.
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
    radio.begin(); // if else statement is acting very weird
    axl.begin();   // ready to test
  
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
    gps.updateGPSData(&bmw);
    gps.dumpGPSData();
    delay(500);
}