//cleaner version of main which will get renamed back to main when done
//
#include <Arduino.h>

#include "GPSHandler.h"
GPSHandler gps;

void setup()
{
    Serial.begin(115200);
    gps.begin();

}