#include "pm.h"
#include "lcdgfx.h"

#define PM_THRES_LOW 50
#define PM_THRES_MED 100
#define PM_THRES_HIGH 150

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
#define I2C_SDL 26
#define I2C_FREQ 400000
/*rstPin, SPlatformI2cConfig{ config.busId,
                                     static_cast<uint8_t>(config.addr ?: 0x3C),
                                     config.scl,
                                     config.sda,
                                     config.frequency ?: 400000 } ) {}
                                     */

OLED_TYPE display(-1,{-1,OLED_ADDR,I2C_SDA,I2C_SDL,I2C_FREQ});

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

  //init_pm(NULL); //for polling method
  init_pm(onStatusChange); //call onStatusChange when air quality status change
}

void loop() {
//  if(pms.pm25 < PM_THRES_LOW)
//    pm_ok();
//  else if(pms.pm25 > PM_THRES_HIGH)
//    pm_danger();
//  else
//    pm_warn();
  delay(500);
}

void onStatusChange(void){
  Serial.printf("status changed to: %s\n",air_quality_str.c_str());

//  display.setFixedFont( ssd1306xled_font6x8 );
//  display.begin();
  display.clear();
  display.printFixed(0, 8, "Air quality: ", STYLE_NORMAL);
  display.printFixed(0, 16, air_quality_str.c_str(), STYLE_NORMAL);
  
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
}
