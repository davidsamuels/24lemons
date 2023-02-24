#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <SPI.h>
#include "secret.h"
#include <RadioLib.h>
#include "radioSetUp.h"





void carTx() //transmits data out of car
{

}



void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN,OUTPUT);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  

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
  int state = radio.begin(915.0,500.0,12,5,0x14,20,20,0); 
  radio.setRfSwitchPins(pin_rx_enable, pin_tx_enable);
  radio.setCurrentLimit(240);
  

  if(state == ERR_NONE)
  {
    Serial.println("radio is up");
  }
  else
  {
    Serial.println("radio is down. check setup");
    Serial.print("error");
    Serial.print(state);
    digitalWrite(LED_BUILTIN,HIGH);
    while(1);
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
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  
}

void loop() {
  ArduinoOTA.handle();
  digitalWrite(2,HIGH);
  delay(100);
  digitalWrite(2,LOW);
  delay(100);
  Serial.println("transmiting");
  byte payload[] = {0x1,0x2,0x3,0x4,0x5}; //5 bytes
  int state = radio.transmit(payload,5);
  Serial.println("done transmitting");
  if (state == ERR_NONE) {
    // the packet was successfully transmitted
    Serial.println(F(" success!"));

    // print measured data rate
    Serial.print(F("[SX1278] Datarate:\t"));
    Serial.print(radio.getDataRate());
    Serial.println(F(" bps"));

  } else if (state == ERR_PACKET_TOO_LONG) {
    // the supplied packet was longer than 256 bytes
    Serial.println(F("too long!"));

  } else if (state == ERR_TX_TIMEOUT) {
    // timeout occurred while transmitting packet
    Serial.println(F("timeout!"));

  } else {
    // some other error occurred
    Serial.print(F("failed, code "));
    Serial.println(state);
  }


}