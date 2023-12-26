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

void GPSHandler::updateGPSData(CarData* carData)
{
    //store the data in the car object
    carData->lat = myGNSS.getLatitude();
    carData->lon = myGNSS.getLongitude();
    carData->alt = myGNSS.getAltitude();
    carData->groundSpeed = myGNSS.getGroundSpeed()/447.04; //convert to mph

    
}

void GPSHandler::dumpGPSData(CarData* carData)
{
    char tempGPSDumpBuffer[500];
    int tempSprintfBuff = 0;
    //strongly consider changing to snprintf
    tempSprintfBuff = sprintf(tempGPSDumpBuffer, "latitude:[%f] longitude:[%f] speed[%f]mph altitude[%f]", carData->lat, carData->lon, carData->groundSpeed, carData->alt);
    if (tempSprintfBuff < 0)
    {
        Serial.println("error in GPSHandler dumpGPSData function");
    }
    else
    {
        Serial.println(tempGPSDumpBuffer);
    }
}