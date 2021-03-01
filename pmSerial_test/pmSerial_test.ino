#include "info.h"
#include "aq.h"
#include "led.h"
#include "batt.h"
#include "lcdgfx.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define PM_THRES_LOW 50
#define PM_THRES_MED 100
#define PM_THRES_HIGH 150

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
bool sendAqMsg = 0;
bool sendLowBattMsg = 0;

// oled
#define OLED_TYPE DisplaySSD1306_128x64_I2C
#define OLED_ADDR 0x3C
//#define I2C_SDA 27
//#define I2C_SCL 26
#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_FREQ 100000
/*rstPin, SPlatformI2cConfig{ config.busId,
                                     static_cast<uint8_t>(config.addr ?: 0x3C),
                                     config.scl,
                                     config.sda,
                                     config.frequency ?: 400000 } ) {}
                                     */

OLED_TYPE display(-1,{-1,OLED_ADDR,I2C_SCL,I2C_SDA,I2C_FREQ});

void setup() {  
  Serial.begin(250000);

  init_led();

  display.setFixedFont( ssd1306xled_font6x8 );
  display.setColor(RGB_COLOR16(0,0,255));
  display.begin();
  display.clear();

   // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi \nSSID :");
  display.printFixed(0, 8, "Connecting WiFi...", STYLE_NORMAL);
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
    display.printFixed(0, 16, "Connected to: ", STYLE_NORMAL);
    display.printFixed(0, 24, WIFI_SSID, STYLE_NORMAL);
  }
  else{
    Serial.print("\nWiFi Failed");
    display.printFixed(0, 16, "WiFi Failed", STYLE_NORMAL);
  }
  delay(1000);
  display.clear();

  init_batt(onBattLvlChange);
  begin_aq(onStatusChange);
  //init_pm(NULL); //for polling method
//  init_pm(onStatusChange); //call onStatusChange when air quality status change
}

void loop() {
  //for polling method
//  if(pms.pm25 < PM_THRES_LOW)
//    pm_ok();
//  else if(pms.pm25 > PM_THRES_HIGH)
//    pm_danger();
//  else
//    pm_warn();
//  delay(500);


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
//  if(updateDisplay){
//    display.clear();
//    display.printFixed(0, 8, "                     ", STYLE_NORMAL);//clear line
//    display.printFixed(0, 8, "Battery level: ", STYLE_NORMAL);
//    display.printFixed(15*6, 8, String(BattLvl).c_str(), STYLE_NORMAL);
//    display.printFixed(18*6, 8, "%", STYLE_NORMAL);
//    display.printFixed(0, 16, "Air quality: ", STYLE_NORMAL);
//    display.printFixed(0, 24, "                     ", STYLE_NORMAL);//clear line
//    display.printFixed(0, 24, air_quality_str.c_str(), STYLE_NORMAL);
//    display.printFixed(0, 32, "                     ", STYLE_NORMAL);//clear line
//    display.printFixed(0, 32, "PM 2.5: ", STYLE_NORMAL);
//    display.printFixed(8*6, 32, String(pms.pm25).c_str(), STYLE_NORMAL);
//    display.printFixed(0, 40, "                     ", STYLE_NORMAL);//clear line
//    display.printFixed(0, 40, "eCO2: ", STYLE_NORMAL);
//    display.printFixed(6*6, 40, String(ccs.geteCO2()).c_str(), STYLE_NORMAL);
//  }
}

void onBattLvlChange(void){
  if(BattLvl < LOW_BATT_THRES) 
    lowBatt();
  else 
    stopBlink();

//  display.clear();
  display.printFixed(0, 8, "                     ", STYLE_NORMAL);//clear line
  display.printFixed(0, 8, "Battery level: ", STYLE_NORMAL);
  display.printFixed(15*6, 8, String(BattLvl).c_str(), STYLE_NORMAL);
  display.printFixed(18*6, 8, "%", STYLE_NORMAL);
}

void lowBatt(void){
  static unsigned long lastSentMillis = 0;
//  if( millis()-lastSentMillis >= 600000){ //send warning every 10 mins if low battery
  if( millis()-lastSentMillis >= 20000){
    Serial.println("low battery");
//    bot.sendMessage(chat_id, "low battery");
    sendLowBattMsg = 1;
  }
  blinkLed(LED_RED, 1);
}

void onStatusChange(void){
  Serial.printf("status changed to: %s\n",air_quality_str.c_str());

//  display.clear();
  display.printFixed(0, 16, "Air quality: ", STYLE_NORMAL);
  display.printFixed(0, 24, "                     ", STYLE_NORMAL);//clear line
  display.printFixed(0, 24, air_quality_str.c_str(), STYLE_NORMAL);
//  display.printFixed(0, 32, "                     ", STYLE_NORMAL);//clear line
//  display.printFixed(0, 32, "PM 2.5: ", STYLE_NORMAL);
//  display.printFixed(8*6, 32, String(pms.pm25).c_str(), STYLE_NORMAL);
//  display.printFixed(0, 40, "                     ", STYLE_NORMAL);//clear line
//  display.printFixed(0, 40, "eCO2: ", STYLE_NORMAL);
//  display.printFixed(6*6, 40, String(ccs.geteCO2()).c_str(), STYLE_NORMAL);
  

//  bot.sendMessage(chat_id, "Air quality: "+air_quality_str, "");
//  sendAqMsg = 1; //sending message taking too long
  
  if(air_quality_status == GOOD)
    pm_ok();
  else if(air_quality_status == MODERATE)
    pm_warn();
  else
    pm_danger();
}

void pm_ok(void){
  Serial.println("pm ok");
  
  //set green colour
  setLed(LED_GREEN);
//  digitalWrite(LED_R, LED_OFF);
//  digitalWrite(LED_G, LED_ON);
//  digitalWrite(LED_B, LED_OFF);

  //oled stuff here
  //telegram stuff here
}
void pm_warn(void){
  Serial.println("pm warning");
  
  //set yellow colour
  setLed(LED_YELLOW);
//  digitalWrite(LED_R, LED_ON);
//  digitalWrite(LED_G, LED_ON);
//  digitalWrite(LED_B, LED_OFF);

  //oled stuff here
  //telegram stuff here
}

void pm_danger(void){
  Serial.println("pm danger");
  
  //set red colour
  setLed(LED_RED);
//  digitalWrite(LED_R, LED_ON);
//  digitalWrite(LED_G, LED_OFF);
//  digitalWrite(LED_B, LED_OFF);

  //oled stuff here
  //telegram stuff here
  sendAqMsg = 1;
}
