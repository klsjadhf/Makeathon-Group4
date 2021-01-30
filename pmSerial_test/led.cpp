#include "led.h"

void init_led(void){
  Serial.print("init led");
  
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  digitalWrite(LED_R, LED_OFF);
  digitalWrite(LED_G, LED_OFF);
  digitalWrite(LED_B, LED_OFF);
}

void setLed(COLOUR colour){
  switch(colour){
    case RED:
      digitalWrite(LED_R, LED_ON);
      digitalWrite(LED_G, LED_OFF);
      digitalWrite(LED_B, LED_OFF);
      break;

    case GREEN:
      digitalWrite(LED_R, LED_OFF);
      digitalWrite(LED_G, LED_ON);
      digitalWrite(LED_B, LED_OFF);
      break;

    case BLUE:
      digitalWrite(LED_R, LED_OFF);
      digitalWrite(LED_G, LED_OFF);
      digitalWrite(LED_B, LED_ON);
      break;

    case YELLOW:
      digitalWrite(LED_R, LED_ON);
      digitalWrite(LED_G, LED_ON);
      digitalWrite(LED_B, LED_OFF);
      break;
      
    default:
      break;
  }  
}
