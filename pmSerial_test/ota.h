#ifndef _OTA_H_
#define _OTA_H_

  #include <Arduino.h>
  #include <WiFi.h>
  #include <WiFiClient.h>
  #include <WebServer.h>
  #include <ESPmDNS.h>
  #include <Update.h>

  void start_ota_server(void);
//  void ota_task(void);
  void ota_task(void * parameter);

  extern const char* loginIndex;
  extern const char* serverIndex;
  
#endif
