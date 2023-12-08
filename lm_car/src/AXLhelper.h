#ifndef AXLHELPER_H
#define AXLHELPER_H
#include <Arduino.h>
#include <Wire.h>
#include "SparkFun_LIS2DH12.h" 
#include "CarData.h" //we are going to be passing a reference to the object in main so this class can update the vals


//using qwicc accelerometer LISDH12

//https://www.sparkfun.com/products/retired/15760

class CarAXL
{
    private:
    SPARKFUN_LIS2DH12 accel;       //Create instance
    CarData* carData; //this is what collects all the data

    public:
    CarAXL(); // the default constructor??
    bool begin();

    void updateVals(CarData* needUpdate);

    


};


#endif
