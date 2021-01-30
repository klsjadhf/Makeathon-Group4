#include "aq.h"

String air_quality_str = "";
AIR_QUALITY air_quality_status = AQ_ERROR;
AIR_QUALITY last_air_quality_status = AQ_ERROR;

void check_status_task(void * parameter);
String cal_AQ(uint16_t pm25_reading, uint16_t pm10_reading);

void begin_aq(void (*onChange)(void)){
  Serial.print("begin aq");

  //initalize air quality sensors
  init_pm();
  init_ccs();
  
  xTaskCreate(
    check_status_task,          /* Task function. */
    "check_status_task",        /* String with name of task. */
    10000,                 /* Stack size in bytes. */
    (void*)onChange,       /* Parameter passed as input of the task */
    1,                     /* Priority of the task. */
    NULL                  /* Task handle. */
  );  
}

void check_status_task(void * parameter){
  while(1){
    void (*onStatusChange)(void) = (void (*)(void))parameter;
  
    cal_AQ(pms.pm25, pms.pm10);
//    cal_AQ(pms.pm25, pms.pm10, ccs.geteCO2(), ccs.getTVOC());
    
    if(last_air_quality_status != air_quality_status){
      last_air_quality_status = air_quality_status;
      if(onStatusChange != NULL) onStatusChange();
    }
  }
  vTaskDelete(NULL);
}

// calculates the air quality based on following link (need to do 24 hr avg?)
// https://www.haze.gov.sg/docs/default-source/faq/computation-of-the-pollutant-standards-index-%28psi%29.pdf
String cal_AQ(uint16_t pm25_reading, uint16_t pm10_reading){
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
  
  if(PSI<psi[1]){
    air_quality_str = "Good";
    air_quality_status = GOOD;
  }
  else if(PSI<psi[2]){
    air_quality_str = "Moderate";
    air_quality_status = MODERATE;
  }
  else if(PSI<psi[3]){
    air_quality_str = "Unhealthy";
    air_quality_status = UNHEALTHY;
  }
  else if(PSI<psi[4]){
    air_quality_str = "Very Unhealthy";
    air_quality_status = V_UNHEALTHY;
  }
  else{
    air_quality_str = "Hazardous";
    air_quality_status = HAZARDOUS;
  }

  #if AQ_DEBUG 
    Serial.printf("pm25_sub: %d, pm10_sub: %d, PSI: %d\n", pm25_sub, pm10_sub, PSI);
    Serial.printf("Air quality is %s\n", air_quality_str.c_str());
  #endif //AQ_DEBUG

  return air_quality_str;
}

AIR_QUALITY cal_voc(uint16_t eco2, uint16_t tvoc){
  AIR_QUALITY quality;
  int i;
  const int thres_levels = 3;
  uint16_t eco2_thres[thres_levels] = {500, 1000, 1500};
  uint16_t tvoc_thres[thres_levels] = {10, 15, 20};

  #if AQ_DEBUG 
    Serial.printf("eco2: %d, tvoc: %d\n", eco2, tvoc);
  #endif //AQ_DEBUG

  for(i = 0; i<thres_levels; i++){
    #if AQ_DEBUG 
      Serial.printf("eco2_thres: %d, tvoc_thres: %d, i: %d\n", eco2_thres[i], tvoc_thres[i], i);
    #endif //AQ_DEBUG
    if(eco2<eco2_thres[i] || tvoc<tvoc_thres[i]){
      break;
    }
  }
  quality = (AIR_QUALITY)i;
  
  #if AQ_DEBUG 
    Serial.printf("eco2: %d, tvoc: %d, quality: %d\n", eco2, tvoc, quality);
  #endif //AQ_DEBUG

  return quality;
}
