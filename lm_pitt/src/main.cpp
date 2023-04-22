#include <Arduino.h>
#include "tiny_lcd.h"
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include "radio.h"
#include <RadioLib.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
SFE_UBLOX_GNSS myGNSS;
SX1276 radio = new Module(pin_cs, pin_dio0, pin_nrst, pin_dio1);

// initalize SD card if it has not alread been setup. write header to file and close
void SDheader(fs::FS &fs )
{
  File file = fs.open("/log.txt", FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    file.print("speed(mph),latitude*10^-7,longitude*10^-7,gforcex(mg),gforcey(mg),gforcez(mg),temp(C),seconds\n");
    file.close();
}

void logToSD(fs::FS &fs, String data){
  
  File file = fs.open("/log.txt", FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    file.println(data);
    Serial.println("data sent to sd card");
    file.close();
}

void lilText(float data)
{

  display.clearDisplay();
  display.display();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.print(data);
  display.println(F(" mph"));
  display.display(); // Show initial text
  delay(100);
  // display.startscrollright(0x00, 0x0F);
}

void lilText(String data)
{

  display.clearDisplay();
  display.display();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.print(data);
  display.println(F(" mph"));
  display.display(); // Show initial text
  delay(100);
  display.startscrollright(0x00, 0x0F);
}

long lastTime = 0; // Simple local timer. Limits amount if I2C traffic to u-blox module.
float gpsStuff()
{
  // Query module only every second. Doing it more often will just cause I2C traffic.
  // The module only responds when a new position is available
  if (millis() - lastTime > 1000)
  {
    lastTime = millis(); // Update the timer

    long latitude = myGNSS.getLatitude();
    
    Serial.print(F("Lat: "));
    Serial.print(latitude);

    long longitude = myGNSS.getLongitude();
    Serial.print(F(" Long: "));
    Serial.print(longitude);

    long speed = myGNSS.getGroundSpeed();
    Serial.print(F(" Speed: "));
    Serial.print(speed);
    Serial.print(F(" (mm/s)"));

    long heading = myGNSS.getHeading();
    Serial.print(F(" Heading: "));
    Serial.print(heading);
    Serial.print(F(" (degrees * 10^-5)"));

    int pDOP = myGNSS.getPDOP();
    Serial.print(F(" pDOP: "));
    Serial.print(pDOP / 100.0, 2); // Convert pDOP scaling from 0.01 to 1

    Serial.println();
    return speed;
  }
} // generates warning not retunringn anything in float return function

// void transmitStuff(float speed)
// {
//   int radioStatus = -1;
//   float speedtemp = gpsStuff() / 447.04; // convert  to mph
//   lilText(speedtemp);

//   radioStatus = radio.transmit("poop");
//    if (radioStatus == RADIOLIB_ERR_NONE) {
//     // the packet was successfully transmitted
//     Serial.println(F(" success!"));

//     // print measured data rate
//     Serial.print(F("[SX1278] Datarate:\t"));
//     Serial.print(radio.getDataRate());
//     Serial.println(F(" bps"));

//   }
// }

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  SPI.begin();
   Serial.println("trying sd card");
  if(!SD.begin(16)){
        Serial.println("Card Mount Failed");
  }
  else
  {
    SDheader(SD);
  }

  Serial.println("trying display");
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
  }

  Serial.println("trying gps ");
  if (myGNSS.begin() == false) // Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
  }

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
  int state = radio.begin(915.0, 500.0, 12, 5, 0x14, 20, 20, 0);
  radio.setRfSwitchPins(pin_rx_enable, pin_tx_enable); //  15, 13
  radio.setCurrentLimit(240);
  radio.setOutputPower(30);
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("success!"));
  }
  else
  {
    Serial.print(F("failed, code "));
    Serial.println(state);
  }

 

}

float speedtemp = -1;
//what we give to radio to send out
char output[256];
//converting float to string to send as char array in output
char speed[5]; // 150.5\0

struct carData //holds all data about the car
{
  float lati =-1; //used to be long datatype
  float longi =-1;
  float speed =-1;
  //tireFR = []
  //tireFL  = []
  float gx = -1;
  float gy = -1;
  float gz = -1;
  float temper = -1;


};

void loop()
{
  struct carData bmw;
 Serial.print(F("[SX1278] Waiting for incoming transmission ... "));

  // you can receive data as an Arduino String
  // NOTE: receive() is a blocking method!
  //       See example ReceiveInterrupt for details
  //       on non-blocking reception method.
  String str;
  int state = radio.receive(str);

  // you can also receive data as byte array
  /*
    byte byteArr[8];
    int state = radio.receive(byteArr, 8);
  */

  if (state == RADIOLIB_ERR_NONE) {
    // packet was successfully received
    Serial.println(F("success!"));

    // print the data of the packet
    Serial.print(F("[SX1278] Data:\t\t\t"));
    Serial.println(str);
    Serial.println("end of data package");

    lilText(str);
    
    // print the RSSI (Received Signal Strength Indicator)
    // of the last received packet
    Serial.print(F("[SX1278] RSSI:\t\t\t"));
    Serial.print(radio.getRSSI());
    Serial.println(F(" dBm"));

    // print the SNR (Signal-to-Noise Ratio)
    // of the last received packet
    Serial.print(F("[SX1278] SNR:\t\t\t"));
    Serial.print(radio.getSNR());
    Serial.println(F(" dB"));

    // print frequency error
    // of the last received packet
    Serial.print(F("[SX1278] Frequency error:\t"));
    Serial.print(radio.getFrequencyError());
    Serial.println(F(" Hz"));

    //log to SD card
    logToSD(SD,str);

  } else if (state == RADIOLIB_ERR_RX_TIMEOUT) {
    // timeout occurred while waiting for a packet
    Serial.println(F("timeout!"));

  } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
    // packet was received, but is malformed
    Serial.println(F("CRC error!"));

  } else {
    // some other error occurred
    Serial.print(F("failed, code "));
    Serial.println(state);

  }
}