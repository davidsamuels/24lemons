#ifndef CAR_DATA_H
#define CAR_DATA_H

class CarData{

    private:
    

    public:
    
    float axlX;
    float axlY;
    float axlZ;

    float lat; // looks like this Lat: 151604970 Long: -693551830 
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