#include "AXLhelper.h"


CarAXL::CarAXL() {
    // Constructor implementation (if any)
}

bool CarAXL::begin()
{
    if (accel.begin() == false)
        return false;
    else
        return true;
}

void CarAXL::updateVals(CarData* needUpdate)
{
    if (needUpdate == nullptr)
    {
        Serial.println("nullptr got to CarAXL updateVAls function gg");
        return;
    }
    
    needUpdate->axlX = accel.getX(); // returns milli gs
    needUpdate->axlY = accel.getY();
    needUpdate->axlZ = accel.getZ();
    needUpdate->boardTemp = accel.getTemperature(); //return in C


    return;


}