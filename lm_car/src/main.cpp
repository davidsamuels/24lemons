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
    gps.begin();   //
    radio.begin(); //
    axl.begin();   //
}
int count = 0;
void loop()
{
    ota.handleOTA();         // needed for the microcontroller to not miss a wifi update request
    axl.updateVals(&bmw);    // grab data from the accelerometer and put it in the bmw object which stores all car relavant data
    gps.updateGPSData(&bmw); // get gps coords and speed
    gps.dumpGPSData();

    String str = "Hello World! #" + String(count++);
    radio.tx(str);

    delay(500);

    
}