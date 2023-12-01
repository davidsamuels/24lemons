#include "GPSHandler.h"
// this file talks to the sparkfun  gnss ublox neo m9n from the micromod ecosystem
//  https://www.sparkfun.com/products/15712

// constructor
GPSHandler::GPSHandler()
{
}

bool GPSHandler::begin()
{

    if (!myGNSS.begin())
    {
        Serial.println(F("u-blox GNSS not detected. Check wiring."));
        return false;
    }
    return true;
}

// looks like this Lat: 151604970 Long: -693551830
float GPSHandler::getAltitude()
{
    return myGNSS.getAltitude();
}

float GPSHandler::getLatitude()
{
    return myGNSS.getLatitude();
}

float GPSHandler::getLongitude()
{
    return myGNSS.getLongitude();
}

float GPSHandler::getGroundSpeed()
{
    return myGNSS.getGroundSpeed();
}

void GPSHandler::updateGPSData()
{
    currentLatitude = myGNSS.getLatitude();
    currentLongitude = myGNSS.getLongitude();
    currentGroundSpeed = myGNSS.getGroundSpeed();
    currentAltitude = myGNSS.getAltitude();
}

void GPSHandler::dumpGPSData()
{
    char tempGPSDumpBuffer[100];
    int tempSprintfBuff = 0;
    tempSprintfBuff = sprintf(tempGPSDumpBuffer, "latitude:[%f] longitude:[%f] speed[%f]mph altitude[%f]", currentLatitude, currentLongitude, currentGroundSpeed, currentAltitude);
    if (tempSprintfBuff < 0)
    {
        Serial.println("error in GPSHandler dumpGPSData function");
    }
    else
    {
        Serial.println(tempGPSDumpBuffer);
    }
}