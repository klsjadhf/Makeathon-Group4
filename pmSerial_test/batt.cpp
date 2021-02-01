#include "batt.h"

int getBatt(void);
void battTask(void * parameter);
double mapf(double x, double in_min, double in_max, double out_min, double out_max);

void init_batt(void (*onLowBatt)(void)){
  pinMode(BATT_PIN, INPUT);
  
  xTaskCreate(
    battTask,          /* Task function. */
    "battTask",        /* String with name of task. */
    10000,                 /* Stack size in bytes. */
    (void*)onLowBatt,       /* Parameter passed as input of the task */
    1,                     /* Priority of the task. */
    NULL                  /* Task handle. */
  );  
}

void battTask(void * parameter){
  void (*onLowBatt)(void) = (void (*)(void))parameter;
  while(1){
    if(getBatt() < LOW_BATT_THRES){
      if(onLowBatt != NULL) onLowBatt();
    }
    delay(POLL_INT);
  }
  vTaskDelete(NULL);
}

int getBatt(void){
  int battLevel;
    // voltage curve from https://blog.ampow.com/lipo-voltage-chart/
  const double voltage[] = {3.27, 3.61, 3.69, 3.71, 3.73, 3.75, 3.77, 3.79, 
                            3.8, 3.82, 3.84, 3.85, 3.87, 3.91, 3.95, 3.98, 
                            4.02, 4.08, 4.11, 4.15, 4.2};
  const int percent[] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 
                          55, 60, 65, 70, 75, 80, 85, 90, 95, 100};
  double battVolt = (analogRead(BATT_PIN)/4095.0)*4.2;

  // //test function enter voltage
  // char inStr[10];
  // while(!Serial.available());
  // Serial.readBytesUntil('\n', inStr, 10);
  // double battVolt = atof(inStr);

  int range = ( sizeof(voltage) / sizeof(voltage[0]) ) - 1;
  while (range > 0){
    if (battVolt >= voltage[range]){
      break;
    }
    range--;
  }
  if (battVolt >= 4.2) battLevel = 100;
  else if (battVolt < voltage[0]) battLevel = 0;
  else battLevel = (int)round(mapf(battVolt, voltage[range], voltage[range+1], percent[range], percent[range+1]));
  battLevel = constrain(battLevel, 0 , 100);

  #if BATT_DEBUG
   Serial.printf("battVolt: %f, range: %d, battLevel: %d\n", battVolt, range, battLevel);
  #endif
  return battLevel;
}

double mapf(double x, double in_min, double in_max, double out_min, double out_max){
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
