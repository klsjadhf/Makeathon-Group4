#include "pm.h"
#include "lcdgfx.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#define PM_THRES_LOW 50
#define PM_THRES_MED 100
#define PM_THRES_HIGH 150

// Wifi network station credentials
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASSWORD"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
String chat_id = "123456789"; //replace with your chat id

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
bool sendMsg = 0;

//pins for rgb led
#define LED_R 13
#define LED_G 12
#define LED_B 14
//using active low led
#define LED_ON LOW
#define LED_OFF HIGH

// oled
#define OLED_TYPE DisplaySSD1306_128x64_I2C
#define OLED_ADDR 0x3C
#define I2C_SDA 27
#define I2C_SCL 26
#define I2C_FREQ 400000
/*rstPin, SPlatformI2cConfig{ config.busId,
                                     static_cast<uint8_t>(config.addr ?: 0x3C),
                                     config.scl,
                                     config.sda,
                                     config.frequency ?: 400000 } ) {}
                                     */

OLED_TYPE display(-1,{-1,OLED_ADDR,I2C_SCL,I2C_SDA,I2C_FREQ});

void setup() {
  Serial.begin(9600);
  
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  digitalWrite(LED_R, LED_OFF);
  digitalWrite(LED_G, LED_OFF);
  digitalWrite(LED_B, LED_OFF);

  display.setFixedFont( ssd1306xled_font6x8 );
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

  //init_pm(NULL); //for polling method
  init_pm(onStatusChange); //call onStatusChange when air quality status change
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


  if(sendMsg){
    sendMsg = 0;
    Serial.println("sending message... ");
    bot.sendMessage(chat_id, "Air quality: "+air_quality_str, "");
    Serial.println("message sent");
  }
}

void onStatusChange(void){
  Serial.printf("status changed to: %s\n",air_quality_str.c_str());

  display.clear();
  display.printFixed(0, 8, "Air quality: ", STYLE_NORMAL);
  display.printFixed(0, 16, air_quality_str.c_str(), STYLE_NORMAL);

//  bot.sendMessage(chat_id, "Air quality: "+air_quality_str, "");
//  sendMsg = 1; //sending message taking too long
  
  if(air_quality_status == GOOD)
    pm_ok();
  else if(air_quality_status == MODERATE)
    pm_warn();
  else{
    pm_danger();
  }
}

void pm_ok(void){
  Serial.println("pm ok");
  
  //set green colour
  digitalWrite(LED_R, LED_OFF);
  digitalWrite(LED_G, LED_ON);
  digitalWrite(LED_B, LED_OFF);

  //oled stuff here
  //telegram stuff here
}
void pm_warn(void){
  Serial.println("pm warning");
  
  //set yellow colour
  digitalWrite(LED_R, LED_ON);
  digitalWrite(LED_G, LED_ON);
  digitalWrite(LED_B, LED_OFF);

  //oled stuff here
  //telegram stuff here
}

void pm_danger(void){
  Serial.println("pm danger");
  
  //set red colour
  digitalWrite(LED_R, LED_ON);
  digitalWrite(LED_G, LED_OFF);
  digitalWrite(LED_B, LED_OFF);

  //oled stuff here
  //telegram stuff here
  sendMsg = 1;
}
