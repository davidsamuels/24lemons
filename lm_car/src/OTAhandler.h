#ifndef OTA_HANDLER_H
#define OTA_HANDLER_H

#include <ArduinoOTA.h>
#include <WiFi.h>

class OTAHandler
{
public:
    static void setupOTA();  // Function to set up OTA
    static void handleOTA(); // Function to handle OTA
    static void setupWiFi();

};

#endif // OTA_HANDLER_H
