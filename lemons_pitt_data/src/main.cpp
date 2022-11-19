#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <SPI.h>
#include "secret.h"
#include <RadioLib.h>

// Redefine CS Pin Name
// SPI_CS0:     ESP32
// SS:          ESP32, nRF, RP2040
// SPI_CS:      Artemis
// PIN_SPI_SS:  STM32, SAMD51, nRF, RP2040

#ifndef PIN_SPI_SS
// For Artemis
#ifdef SS
#define PIN_SPI_SS SPI_CS
#endif
// For ESP32
#ifdef SPI_CS0
#define PIN_SPI_SS SS
#endif
#endif

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

#if defined(ARDUINO_RASPBERRY_PI_PICO)
// MM RP2040 Processor Board (Using RP2040 Pico board definition)
int pin_cs = 21;
int pin_dio0 = 6;
int pin_tx_enable = 13;
int pin_rx_enable = 16;
int pin_nrst = 17;
int pin_dio1 = 18;

// Redefine SPI pins
int miso = 20;
int mosi = 23;
int sck = 22;

// Custom SPI object
MbedSPI SPI_mm(miso, mosi, sck);

SX1276 radio = new Module(pin_cs, pin_dio0, pin_nrst, pin_dio1, SPI_mm);

#else
#if defined(ARDUINO_TEENSY_MICROMOD)
// MM Teensy Processor Board
int pin_cs = 10;
int pin_dio0 = 4;
int pin_tx_enable = 3;
int pin_rx_enable = 40;
int pin_nrst = 41;
int pin_dio1 = 42;

#else
// All other MicroMod Processor Boards
int pin_cs = PIN_SPI_SS;
int pin_dio0 = D0;
int pin_tx_enable = PWM0;
int pin_rx_enable = G0;
int pin_nrst = G1;
int pin_dio1 = G2;
#endif

SX1276 radio = new Module(pin_cs, pin_dio0, pin_nrst, pin_dio1);
#endif

void carTx() //transmits data out of car
{
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  /*
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  */

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());

  //set up lora radio
  // carrier frequency:           915.0 MHz
  // bandwidth:                   500.0 kHz
  // spreading factor:            12 7
  // coding rate:                 5
  // sync word:                   0x14
  // output power:                30 dBm = 1 watt limit , 20dbm = .1 watt, sw limit is enabled need to disable
  // preamble length:             20 symbols
  // amplifier gain:              0 = auto gain, 1 (maximum gain)
  int state = radio.begin(915.0, 500.0, 12, 5, 0x14, 20, 20, 0);
  radio.setRfSwitchPins(pin_rx_enable, pin_tx_enable);
  radio.setCurrentLimit(240);
  if (state == ERR_NONE)
  {
    Serial.println("radio is up");
  }
  else
  {
    Serial.println("radio is down. check setup");
    Serial.print("error");
    Serial.print(state);
    digitalWrite(LED_BUILTIN, HIGH);
    while (1)
      ;
  }

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname("car_collector");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
      .onStart([]()
               {
                 String type;
                 if (ArduinoOTA.getCommand() == U_FLASH)
                   type = "sketch";
                 else // U_SPIFFS
                   type = "filesystem";

                 // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
                 Serial.println("Start updating " + type);
               })
      .onEnd([]()
             { Serial.println("\nEnd"); })
      .onProgress([](unsigned int progress, unsigned int total)
                  { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
      .onError([](ota_error_t error)
               {
                 Serial.printf("Error[%u]: ", error);
                 if (error == OTA_AUTH_ERROR)
                   Serial.println("Auth Failed");
                 else if (error == OTA_BEGIN_ERROR)
                   Serial.println("Begin Failed");
                 else if (error == OTA_CONNECT_ERROR)
                   Serial.println("Connect Failed");
                 else if (error == OTA_RECEIVE_ERROR)
                   Serial.println("Receive Failed");
                 else if (error == OTA_END_ERROR)
                   Serial.println("End Failed");
               });

  ArduinoOTA.begin();
}
byte payload[5];
void loop()
{
  ArduinoOTA.handle();
  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(2, LOW);
  delay(100);
  Serial.println("waiting for incoming transmision");
  int state = radio.receive(payload, 5);

  if (state == ERR_NONE)
  {
    // packet was successfully received
    Serial.println(F("success!"));

    // print the data of the packet
    Serial.print(F("[SX1278] Data:\t\t\t"));
    for (int i = 0; i < 5; i++)
      Serial.println(payload[i]);

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
  }
  else if (state == ERR_RX_TIMEOUT)
  {
    // timeout occurred while waiting for a packet
    Serial.println(F("timeout!"));
  }
  else if (state == ERR_CRC_MISMATCH)
  {
    // packet was received, but is malformed
    Serial.println(F("CRC error!"));
  }
  else
  {
    // some other error occurred
    Serial.print(F("failed, code "));
    Serial.println(state);
  }
}