//cleaner version of main which will get renamed back to main when done
//
#include <Arduino.h>

#include "GPSHandler.h"
GPSHandler gps;

#include "CarRadio.h"
CarRadio radio;

#include "CarData.h"
CarData bmw;

void setup()
{
    Serial.begin(115200);
    gps.begin();
    radio.begin();

}

void loop()
{

}