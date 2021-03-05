#include "info.h"
#include "aq.h"
#include "led.h"
#include "batt.h"
#include "oled.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
bool sendAqMsg = 0;
bool sendLowBattMsg = 0;

SemaphoreHandle_t i2cMux;

void setup() {  
  Serial.begin(250000);

  i2cMux = xSemaphoreCreateMutex();

  init_led();
  init_oled();

   // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi \nSSID :");
  oledPrintOnLine(1, "Connecting WiFi...");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  for (int i=0; i<10; i++){
    if(WiFi.status() == WL_CONNECTED){
      break;
    }
    Serial.print(".");
    delay(500);
  }
  if(WiFi.status() == WL_CONNECTED){
    Serial.print("\nWiFi connected. IP address: ");
    Serial.println(WiFi.localIP());
    oledPrintOnLine(2, "Connected to: ");
    oledPrintOnLine(3, WIFI_SSID);
  }
  else{
    Serial.print("\nWiFi Failed");
    oledPrintOnLine(2, "WiFi Failed");
  }
  delay(1000);
  clearOled();

  init_batt(onBattLvlChange);
  begin_aq(onStatusChange);
}

void loop() {
  static long lastDispMillis = millis();

  if(sendAqMsg){
    sendAqMsg = 0;
    Serial.println("sending message... ");
    bot.sendMessage(chat_id, "Air quality: "+air_quality_str, "");
    Serial.println("message sent");
  }
  if(sendLowBattMsg){
    sendLowBattMsg = 0;
    Serial.println("sending message... ");
    bot.sendMessage(chat_id, "low battery");
    Serial.println("message sent");
  }
}

void onBattLvlChange(void){
  if(BattLvl < LOW_BATT_THRES) 
    lowBatt();
  else 
    stopBlink();

  String dispOut = "Battery: " + String(BattLvl) + "%";
  oledPrintOnLine(1, dispOut.c_str());
}

void lowBatt(void){
  static unsigned long lastSentMillis = 0;
  if( millis()-lastSentMillis >= 600000){ //send warning every 10 mins if low battery
//  if( millis()-lastSentMillis >= 20000){
    Serial.println("low battery");
    bot.sendMessage(chat_id, "low battery");
    sendLowBattMsg = 1;
  }
  blinkLed(LED_RED, 1);
}

void onStatusChange(void){
  Serial.printf("status changed to: %s\n",air_quality_str.c_str());

  oledPrintOnLine(2, "Air quality: ");
  oledPrintOnLine(3, air_quality_str.c_str());

  String dispOut = "PSI: " + String(PSI);
  oledPrintOnLine(5, dispOut.c_str());
  
  if(air_quality_status == GOOD)
    pm_ok();
  else if(air_quality_status == MODERATE)
    pm_warn();
  else
    pm_danger();
}

void pm_ok(void){
  Serial.println("pm ok");
  setLed(LED_GREEN);
}
void pm_warn(void){
  Serial.println("pm warning");
  setLed(LED_YELLOW);
}

void pm_danger(void){
  Serial.println("pm danger");
  setLed(LED_RED);
  sendAqMsg = 1;
}
