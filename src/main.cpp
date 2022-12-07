#include <Arduino.h>
#include "my_def.h"
#include "my_debug.h"
#include "bluetooth.h"
#include "lcd.h"
#include "gprs.h"
#include "gps.h"
#include "maps.h"
#include "tpms.h"
#include "weather.h"


// TASKS ON CORE 0 //
void initOthers (void * parameter){ 
  initDisplay();
  initGPRS();
  vTaskDelete(T_initOthers);
}

void loopOthers (void * parameter){
  for(;;) {
    if(timerDisplay < millis()){
      sendDisplay();
      timerDisplay = millis() + 180; // Every 200ms min + time on GPS
    }

    if (bikeGPS != STATUS_UNK){
      getGPS(); // Every time
    }

    delay(20); // For WDog feeding
  }
}
// -------------- //


void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
  #endif
  DEBUG_PRINTLN("Manuel Gracia.Nov-2022");
  DEBUG_PRINTLN("https://github.com/m-gracia");
  DEBUG_PRINTLN("esp32-bike-computer_20221104");
    
  xTaskCreatePinnedToCore(
    initOthers, /* Function to implement the task */
    "initOthers", /* Name of the task */
    10000,  /* Stack size in words */
    NULL,  /* Task input parameter */
    4,  /* Priority of the task */
    &T_initOthers,  /* Task handle. */
    0); /* Core where the task should run */
  initBT();
  initTPMS();

  xTaskCreatePinnedToCore(
      loopOthers, // Function to implement the task
      "loopOthers", // Name of the task
      20000,  // Stack size in words
      NULL,  // Task input parameter
      1,  // Priority of the task
      &T_loopOthers,  // Task handle.
      0); // Core where the task should run
      
  DEBUG_PRINTLN("Setup end");
}

void loop() {
  if (timerGPRS < millis() && bikeGPRS != STATUS_UNK){
    sendGPRS();
    if (bikeGPRS != STATUS_OK) timerGPRS = millis() + 10000;    // On error recheck in 10 sec
    else timerGPRS = millis() + 300000; // 5 min
  }

  if (timerWeather < millis() && bikeGPRS != STATUS_UNK){
    getWeather();
    if (bikeGPRS != STATUS_OK) timerWeather = millis() + 10000;   // On error recheck in 10 sec
    else timerWeather = millis() + 600000;  // 10 min
  }

  if (timerMaps < millis() && bikeGPRS != STATUS_UNK){
    getMaps();
    timerMaps = millis() + 10000; // 10 sec
  }

  if (timerTPMS < millis()){
    getTPMS();
    timerTPMS = millis() + (BTSCANTIME * 1000) + 200; // 5 sec
  }
}