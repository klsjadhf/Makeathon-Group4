#include "led.h"

TaskHandle_t BlinkTaskHandle = NULL;
COLOUR blinkColour = LED_BLACK;
float blinkFreq = -1;
int blinking = 0;

void blinkLedTask(void * parameter);

void init_led(void){
  Serial.print("init led");
  
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  
  digitalWrite(LED_R, LED_OFF);
  digitalWrite(LED_G, LED_OFF);
  digitalWrite(LED_B, LED_OFF);
}

void blinkLed(COLOUR colour, float freq){
  #if LED_DEBUG
    Serial.printf("colour: %d, freq: %f\n", colour, freq);
  #endif
  
  blinkColour = colour;
  blinkFreq = freq;

  if(!blinking){
    blinking = 1; //prevent starting extra tasks
    xTaskCreate(
      blinkLedTask,          /* Task function. */
      "blinkLedTask",        /* String with name of task. */
      10000,                 /* Stack size in bytes. */
      NULL,       /* Parameter passed as input of the task */
      1,                     /* Priority of the task. */
      &BlinkTaskHandle                  /* Task handle. */
    );
  }
}
void stopBlink(void){
  #if LED_DEBUG
      Serial.println("stop blinking");
  #endif

  if (blinking){
    if (BlinkTaskHandle != NULL) vTaskDelete(BlinkTaskHandle);
    setLed(LED_BLACK);
    blinking = 0;
  }
}
void blinkLedTask(void * parameter){
  while(1){
    #if LED_DEBUG
      Serial.println("blink led");
    #endif
    
    setLed(blinkColour);
    delay((long)(((1.0/blinkFreq) * 1000.0)/ 2.0));
    setLed(LED_BLACK);
    delay((long)(((1.0/blinkFreq) * 1000.0)/ 2.0));
  }
  vTaskDelete(NULL);
}

void setLed(COLOUR colour){
  #if LED_DEBUG
    Serial.printf("colour: %d\n", colour);
  #endif
  
  switch(colour){
    case LED_BLACK:
      digitalWrite(LED_R, LED_OFF);
      digitalWrite(LED_G, LED_OFF);
      digitalWrite(LED_B, LED_OFF);
      break;
    case LED_RED:
      digitalWrite(LED_R, LED_ON);
      digitalWrite(LED_G, LED_OFF);
      digitalWrite(LED_B, LED_OFF);
      break;

    case LED_GREEN:
      digitalWrite(LED_R, LED_OFF);
      digitalWrite(LED_G, LED_ON);
      digitalWrite(LED_B, LED_OFF);
      break;

    case LED_BLUE:
      digitalWrite(LED_R, LED_OFF);
      digitalWrite(LED_G, LED_OFF);
      digitalWrite(LED_B, LED_ON);
      break;

    case LED_YELLOW:
      digitalWrite(LED_R, LED_ON);
      digitalWrite(LED_G, LED_ON);
      digitalWrite(LED_B, LED_OFF);
      break;
      
    default:
      break;
  }  
}
