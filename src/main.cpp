#include <Arduino.h>
#include "my_def.h"
#include "my_debug.h"
#include "bluetooth.h"
#include "lcd.h"
#include "gprs.h"
#include "tpms.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
//#include "soc/rtc_wdt.h"


// TASK ON CORE 0 //
void loopOthers (void * parameter){
  for(;;) {
    delay(20); // For WDog feeding

    if(timerDisplay < millis()){
      sendDisplay();
      timerDisplay = millis() + 180; // Every 200ms min + time on GPS
    }

    getWeb();
  }
}
// -------------- //


void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Disable browout detector
  //rtc_wdt_protect_off(); // Disable watchdog
  //rtc_wdt_disable();
  //disableCore1WDT();
  //disableCore0WDT();

  #ifdef DEBUG
    //Serial.begin(921600); 
    Serial.begin(115200);
  #endif
  DEBUG_PRINTLN("Manuel Gracia.Abr-2023");
  DEBUG_PRINTLN("https://github.com/m-gracia");
  DEBUG_PRINTLN("esp32-bike-computer_20230430");

  pinMode(LED_PIN,OUTPUT);
  for(int i=0;i<10; i++){  // Wait until the current is stabilized
    if (digitalRead(LED_PIN)) digitalWrite(LED_PIN,LOW);
    else digitalWrite(LED_PIN,HIGH);
    delay(200);
  }
  digitalWrite(LED_PIN,HIGH);

  // Init Setup
  initDisplay();
  initWeb();
  initGPRS();
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
  digitalWrite(LED_PIN,LOW);
}

void loop() {
  if ((timerGPRS < millis() || timerWeather < millis() || timerMaps < millis()) 
    && bikeGPRS != STATUS_UNK){
      useGPRS();
  }

  if (timerTPMS < millis()){
    getTPMS();
    timerTPMS = millis() + (BTSCANTIME * 1000) + 200; // 5 sec
  }

  if (timerGPS < millis() && bikeGPS != STATUS_UNK){
    getGPS(); // Every time
    timerGPS = millis() + 1000;  // 1s
    
    if (digitalRead(LED_PIN)) digitalWrite(LED_PIN,LOW);
    else digitalWrite(LED_PIN,HIGH);
  }
}