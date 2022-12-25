#include <Arduino.h>
#include "tiny_lcd.h"
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
SFE_UBLOX_GNSS myGNSS;

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

  Serial.println("trying gps ");
  if (myGNSS.begin() == false) // Connect to the u-blox module using Wire port
  {
    Serial.println(F("u-blox GNSS not detected at default I2C address. Please check wiring. Freezing."));
  }

  
  
}
float speedtemp = -1;
void loop()
{
  speedtemp = gpsStuff() / 447.04;
  
  lilText(speedtemp);
  // put your main code here, to run repeatedly:
}