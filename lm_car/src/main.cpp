#include <Arduino.h>
#include "tiny_lcd.h"
#include "GPSHandler.h"  //<- currently working on
#include "radio.h"
#include <RadioLib.h>
#include "FS.h" 
#include "SD.h"
#include "SPI.h"
#include "SparkFun_LIS2DH12.h" //Click here to get the library: http://librarymanager/All#SparkFun_LIS2DH12
SPARKFUN_LIS2DH12 accel;       //Create instance
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

SX1276 radio = new Module(pin_cs, pin_dio0, pin_nrst, pin_dio1);
//what we give to radio to send out
char output[256];
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
void lilText(float data) //128x32 resolution
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


long lastTime = 0; // Simple local timer. Limits amount if I2C traffic to u-blox module.
float gpsStuff(struct carData* _bmw)
{
  // Query module only every second. Doing it more often will just cause I2C traffic.
  // The module only responds when a new position is available
  if (millis() - lastTime > 250)
  {
    lastTime = millis(); // Update the timer

    long latitude = myGNSS.getLatitude(); // looks like this Lat: 151604970 Long: -693551830 
    Serial.print(F("Lat: "));
    Serial.print(latitude);

    long longitude = myGNSS.getLongitude();
    Serial.print(F(" Long: "));
    Serial.print(longitude);

    long speed = myGNSS.getGroundSpeed()/447.04 ;
    Serial.print(F(" Speed: "));
    Serial.print(speed);
    Serial.print(F(" (mph)"));

    long heading = myGNSS.getHeading();
    Serial.print(F(" Heading: "));
    Serial.print(heading);
    Serial.print(F(" (degrees * 10^-5)"));

    int pDOP = myGNSS.getPDOP();
    Serial.print(F(" pDOP: "));
    Serial.print(pDOP / 100.0, 2); // Convert pDOP scaling from 0.01 to 1

    Serial.println("done");
    _bmw->lati = latitude ;
    _bmw->longi = longitude;
    _bmw->speed = speed;

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
void logToSD(float speed,struct carData bmw,fs::FS &fs){
  
  File file = fs.open("/log.txt", FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    file.print(bmw.speed);file.print(",");file.print(bmw.lati);file.print(",");file.print(bmw.longi);file.print(",");file.print(bmw.gx);file.print(",");file.print(bmw.gy);file.print(",");file.print(bmw.gz);file.print(",");file.print(bmw.temper);file.print(",");file.print(millis()/1000);file.print("\n");
    Serial.println("data sent to sd card");
    file.close();
}

void accelTest(struct carData* _bmw)
{
   if (accel.available())
  {
    float accelX = accel.getX();
    float accelY = accel.getY();
    float accelZ = accel.getZ();
    float tempC = accel.getTemperature();

    Serial.print("Acc [mg]: ");
    Serial.print(accelX, 1);
    Serial.print(" x, ");
    Serial.print(accelY, 1);
    Serial.print(" y, ");
    Serial.print(accelZ, 1);
    Serial.print(" z, ");
    Serial.print(tempC, 1);
    Serial.print("C");
    Serial.println();

    _bmw->gx = accelX; // log the acceleration into the car struct
    _bmw->gy = accelY;
    _bmw->gz = accelZ;
    _bmw->temper = tempC;

  }
}

void packageData(struct carData bmw) //send the car struct to copy the data to one big string payload for the lora transmit funtion
{

  char strSpeed[7];
  char strLat[11];
  char strLon[11]; // need to figure out size based off sparkfun library -xxx.xxxxx
  char strGx[10]; 
  char strGy[10];
  char strGz[10];
  char strTemp[7];
  char strTime[8];

  dtostrf(bmw.speed, 6, 2, strSpeed);
  dtostrf(bmw.lati, 10, 0, strLat);
  dtostrf(bmw.longi, 10, 0, strLon);
  dtostrf(bmw.gx, 7, 2, strGx);
  dtostrf(bmw.gy, 7, 2, strGy);
  dtostrf(bmw.gz, 7, 2, strGz);
  dtostrf(bmw.temper, 6, 2, strTemp);
  dtostrf(millis() / 1000.0, 7, 2, strTime);

  int ret = sprintf(output, "speed:%s,lat:%s,lon:%s,gx:%s,gy:%s,gz:%s,temp:%s,sec:%s\n", strSpeed, strLat, strLon, strGx, strGy, strGz, strTemp, strTime);

if (ret < 0) {
    Serial.println("Error: sprintf failed");
} else {
    Serial.print("packaged data looks like this[");
    Serial.print(output);
    Serial.println("]done packing data");
}


}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("trying display");
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
  }

   if(!SD.begin(16)){
        Serial.println("Card Mount Failed");
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
  radio.setOutputPower(30); // Set output power to max (30 dBm)
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("radio success!"));
  }
  else
  {
    Serial.print(F("radio failed, code "));
    Serial.println(state);
  }
  SDheader(SD);
 
  if(accel.begin() == false) //activate accelerometer
  {
    Serial.println("accelerometer failed ");
  }
}

float speedtemp = -1;

//converting float to string to send as char array in output
char speed[5]; // 150.5\0


void loop()
{
  struct carData bmw; 
  
  int radioStatus = -1;
  speedtemp = gpsStuff(&bmw) ; // convert  to mph also store gps data in struct
  lilText(speedtemp);
  //dtostrf(speedtemp,-4,1,speed); // not needed anymore
  sprintf(output,"speed %s",speed); // a way to combine a bunch of data into a char array
  //check out sprintf-arduino https://www.programmingelectronics.com/sprintf-arduino/
  
  accelTest(&bmw);

  packageData(bmw);

  
  
  logToSD(speedtemp,bmw,SD); 
  radioStatus = radio.transmit(output);
   if (radioStatus == RADIOLIB_ERR_NONE) {
    // the packet was successfully transmitted
    Serial.println(F(" success!"));

    // print measured data rate
    Serial.print(F("[SX1278] Datarate:\t"));
    Serial.print(radio.getDataRate());
    Serial.println(F(" bps"));

  }
  // put your main code here, to run repeatedly:
}