#include "CarRadio.h"


CarRadio::CarRadio() {
    Module* mod = new Module(pin_cs, pin_dio0, pin_nrst, pin_dio1);
    radio = new SX1276(mod);
}

CarRadio::~CarRadio() {
    delete radio->getMod();
    delete radio;
}
bool CarRadio::begin()
{
    Serial.println("trying lora radio ");
    // set up lora radio
    //  carrier frequency:           915.0 MHz
    //  bandwidth:                   500.0 kHz
    //  spreading factor:            12 7
    //  coding rate:                 5
    //  sync word:                   0x14
    //  output power:                30 dBm = 1 watt limit , 20dbm = .1 watt, sw limit is enabled need to disable
    //  preamble length:             20 symbols
    //  amplifier gain:              0 = auto gain, 1 (maximum gain)
    int state = radio->begin(915.0, 500.0, 12, 5, 0x14, 20, 20, 0);
    radio->setRfSwitchPins(pin_rx_enable, pin_tx_enable);
    radio->setCurrentLimit(240);
    radio->setOutputPower(30); // Set output power to max (30 dBm)
    if (state == RADIOLIB_ERR_NONE)
    {
        Serial.println(F("radio success!"));
    }
    else
    {
        Serial.print(F("radio failed, code "));
        Serial.println(state);
    }
}