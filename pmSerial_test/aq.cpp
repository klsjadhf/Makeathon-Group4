#include "aq.h"

String air_quality_str = "";
AIR_QUALITY air_quality_status = AQ_ERROR;
AIR_QUALITY last_air_quality_status = AQ_ERROR;

void check_status_task(void * parameter);
AIR_QUALITY cal_PM_quality(uint16_t pm25_reading, uint16_t pm10_reading);
AIR_QUALITY cal_voc_quality(uint16_t eco2, uint16_t tvoc);
String AQtoS(AIR_QUALITY aq);

void begin_aq(void (*onChange)(void)){
  Serial.println("begin aq");

  //initalize air quality sensors
  init_pm();
  init_ccs();
  
  xTaskCreatePinnedToCore(
    check_status_task,          /* Task function. */
    "check_status_task",        /* String with name of task. */
    10000,                 /* Stack size in bytes. */
    (void*)onChange,       /* Parameter passed as input of the task */
    1,                     /* Priority of the task. */
    NULL,                  /* Task handle. */
    1                      // pin to core 1 (i2c begin was called from core 1, interrupt has to be processed on core 1)
  );  
}

void check_status_task(void * parameter){
  while(1){
    void (*onStatusChange)(void) = (void (*)(void))parameter;

//    char inStr[10]={0};
//    Serial.print("Enter val: ");
//    while(!Serial.available())delay(10);
//    Serial.readBytesUntil('\n', inStr, 10);
//    int inVal = atoi(inStr);
//    Serial.printf("Entered: %d\n", inVal);
//
//    air_quality_status = cal_voc_quality(inVal, 0);
  
//    cal_AQ(pms.pm25, pms.pm10);
    air_quality_status = max(cal_PM_quality(pms.pm25, pms.pm10), cal_voc_quality(ccs.geteCO2(), ccs.getTVOC()));
    air_quality_str = AQtoS(air_quality_status);

    #if AQ_DEBUG 
      Serial.printf("last status: %d, current status: %d, %s\n", last_air_quality_status, air_quality_status, air_quality_str.c_str());
    #endif //AQ_DEBUG
    
    if(last_air_quality_status != air_quality_status){
      last_air_quality_status = air_quality_status;
      if(onStatusChange != NULL) onStatusChange();
    }
    delay(300);
  }
  vTaskDelete(NULL);
}

// calculates the air quality based on following link (need to do 24 hr avg?)
// https://www.haze.gov.sg/docs/default-source/faq/computation-of-the-pollutant-standards-index-%28psi%29.pdf
AIR_QUALITY cal_PM_quality(uint16_t pm25_reading, uint16_t pm10_reading){
  AIR_QUALITY aq;
  const int pm25[] = {0, 12, 55, 150, 250, 350, 500};
  const int pm10[] = {0, 50, 150, 350, 420, 500, 600};
  const int psi[] = {0, 50, 100, 200, 300, 400 ,500};
  int i,j;
  for(i=0; i<7; i++){
    if(pm25_reading < pm25[i])
      break;
  }
  int pm25_sub = round(((float)(psi[i]-psi[i-1])/(float)(pm25[i]-pm25[i-1]))*(float)(pm25_reading-pm25[i-1]))+psi[i-1];

  for(i=0; i<7; i++){
    if(pm10_reading < pm10[i])
      break;
  }
  int pm10_sub = round(((float)(psi[i]-psi[i-1])/(float)(pm10[i]-pm10[i-1]))*(float)(pm10_reading-pm10[i-1]))+psi[i-1];

  int PSI = max(pm25_sub, pm10_sub);

  for(i=1; i<=5; i++){
    if(PSI<psi[i]){
      #if AQ_DEBUG 
      Serial.println(i);
      #endif
      break;
    }
  }
  aq = (AIR_QUALITY)(i-1);
  
//  if(PSI<psi[1]){
//    air_quality_str = "Good";
//    air_quality_status = GOOD;
//  }
//  else if(PSI<psi[2]){
//    air_quality_str = "Moderate";
//    air_quality_status = MODERATE;
//  }
//  else if(PSI<psi[3]){
//    air_quality_str = "Unhealthy";
//    air_quality_status = UNHEALTHY;
//  }
//  else if(PSI<psi[4]){
//    air_quality_str = "Very Unhealthy";
//    air_quality_status = V_UNHEALTHY;
//  }
//  else{
//    air_quality_str = "Hazardous";
//    air_quality_status = HAZARDOUS;
//  }

  #if AQ_DEBUG 
    Serial.printf("pm25_sub: %d, pm10_sub: %d, PSI: %d\n", pm25_sub, pm10_sub, PSI);
    Serial.printf("Air quality is %d\n", aq);
  #endif //AQ_DEBUG

  return aq;
}

AIR_QUALITY cal_voc_quality(uint16_t eco2, uint16_t tvoc){
  AIR_QUALITY aq;
  int i,j;
  const int thres_num = 3;
  uint16_t eco2_thres[thres_num] = {1000, 2000, 4000};
  uint16_t tvoc_thres[thres_num] = {250, 2000, 4000};

  #if AQ_DEBUG 
    Serial.printf("eco2: %d, tvoc: %d\n", eco2, tvoc);
  #endif //AQ_DEBUG

  for(i = 0; i<thres_num; i++){
    #if AQ_DEBUG 
      Serial.printf("eco2_thres: %d, i: %d\n", eco2_thres[i], i);
    #endif //AQ_DEBUG
    if(eco2<eco2_thres[i]){
      break;
    }
  }
  for(j = 0; j<thres_num; j++){
    #if AQ_DEBUG 
      Serial.printf("tvoc_thres: %d, i: %d\n", tvoc_thres[i], i);
    #endif //AQ_DEBUG
    if(tvoc<tvoc_thres[i]){
      break;
    }
  }
  aq = (AIR_QUALITY)max(i, j);
  
  #if AQ_DEBUG 
    Serial.printf("eco2: %d, tvoc: %d, quality: %d\n", eco2, tvoc, aq);
  #endif //AQ_DEBUG

  return aq;
}

String AQtoS(AIR_QUALITY aq){
  switch(aq){
    case GOOD:
      return "Good";
    case MODERATE:
      return "Moderate";
    case UNHEALTHY:
      return "Unhealthy";
    case V_UNHEALTHY:
      return "Very Unhealthy";
    case HAZARDOUS:
      return "Hazardous";
    default:
      return "Error";
  };
}
