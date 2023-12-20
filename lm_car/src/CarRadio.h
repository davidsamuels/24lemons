#ifndef RADIOHELPER_H
#define RADIOHELPER_H

#include <RadioLib.h>

//holds the functionality of the lora radio on function zero of the micromod board
class CarRadio {
private:
    static const int pin_cs = 5;  
    static const int pin_dio0 = 14;         
    static const int pin_tx_enable = 13;
    static const int pin_rx_enable = 15;
    static const int pin_nrst = 25;
    static const int pin_dio1 = 26; 

    static volatile bool transmittedFlag ;
    static void setFlag();

    

    SX1276* radio;

public:
    CarRadio();
    ~CarRadio();
    bool begin();
    bool tx(String payload); // implement as not blocking
    bool rx();
    
};

#endif // RADIOHELPER_H

// SX1276 pin connections:
//       | SLOT 0 | SLOT 1 |
//==========================
// cs    |   CS0  |   CS1  |
// dio0  |   D0   |   D1   |
// dio1  |   G2   |   G7   |
// dio2  |   G3   |   G8   |
// rst   |   G1   |   G6   |
// tx_en |  PWM0  |  PWM1  |
// rx_en |   G0   |   G5   |