#ifndef CAR_DATA_H
#define CAR_DATA_H

class CarData{

    public:
    
    float axlX;
    float axlY;
    float axlZ;

    float lat;
    float lon;
    float alt;
    float groundSpeed;

    float boardTemp;

    CarData();

    bool updateTemp();
    bool updateAxl();
    bool updateGPS();
    


};

#endif