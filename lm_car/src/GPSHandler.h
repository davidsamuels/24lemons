#ifndef GPS_HANDLER_H
#define GPS_HANDLER_H
//this file talks to the sparkfun  gnss ublox neo m9n from the micromod ecosystem
// https://www.sparkfun.com/products/15712

#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

class GPSHandler {

    private:
    SFE_UBLOX_GNSS myGNSS;
    float currentLatitude =1 ;
    float currentLongitude =1 ;
    float currentAltitude =1 ;
    float currentGroundSpeed = -1;

    public:
    GPSHandler();
    bool begin();

    //need to check if use float or long
    float getLatitude();
    float getLongitude();
    float getAltitude();
    float getGroundSpeed();
    void updateGPSData();
    void dumpGPSData();

    //track lap time
    //fastest speed
    //number of laps
};


#endif