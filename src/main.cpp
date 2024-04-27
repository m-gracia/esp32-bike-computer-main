#include <Arduino.h>
#include "my_def.h"
#include "my_debug.h"
#include "bluetooth.h"
#include "lcd.h"
#include "gps.h"
#include "gprs.h"
//#include "httpsrv.h"
#include "tpms.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
//#include "soc/rtc_wdt.h"


// TASK ON CORE 0 //
void loopOthers (void * parameter){
  for(;;) {
    delay(20);    // For WDog feeding

    if(timerDisplay < millis()){
      sendDisplay();
      timerDisplay = millis() + 180; // Every 200ms min + time on GPS
    }

    feedGPS();    // Everytime
    //getWeb();
  }
}
// -------------- //


void setup() {
  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Disable browout detector
  //rtc_wdt_protect_off(); // Disable watchdog
  //rtc_wdt_disable();
  //disableCore1WDT();
  //disableCore0WDT();

  pixel.begin();
  pixel.setBrightness(64);
  for(int i=0;i<6; i++){  // Wait until the current is stabilized 0,5s each loop
    pixel.clear(); pixel.show();
    delay(250);
    pixel.fill(0xFF0000); pixel.show(); // Red    
    delay(250);    
  }

  #ifdef DEBUG
    //Serial.begin(921600); 
    Serial.begin(115200);
  #endif
  DEBUG_PRINTLN("Manuel Gracia.Apr-2024");
  DEBUG_PRINTLN("https://github.com/m-gracia");
  DEBUG_PRINTLN("esp32-bike-computer_20240427");

  // Init Setup
  initDisplay();
  initGPS();
  initBT();
  initTPMS();
  initWifi();
  //initWeb();

  xTaskCreatePinnedToCore(
      loopOthers, // Function to implement the task
      "loopOthers", // Name of the task
      20000,  // Stack size in words
      NULL,  // Task input parameter
      1,  // Priority of the task
      &T_loopOthers,  // Task handle.
      0); // Core where the task should run
      
  pixel.clear(); pixel.show();
  DEBUG_PRINTLN("Setup end");
}

void loop() {
  if ((timerGPRS < millis() || timerWeather < millis() || timerMaps < millis()) 
    && bikeGPRS != STATUS_UNK){
      pixel.fill(0x00FF00); pixel.show(); // Green
      useGPRS();
      pixel.clear(); pixel.show();
  }

  if (timerTPMS < millis()){
    pixel.fill(0x0000FF);  pixel.show(); // Blue
    getTPMS();
    timerTPMS = millis() + (BTSCANTIME * 1000) + 200; // 5 sec
    pixel.clear(); pixel.show();
  }

  if (timerGPS < millis() && bikeGPS != STATUS_UNK){
    pixel.fill(0xB200FF);  pixel.show(); // Purple
    getGPS(); // Every time
    timerGPS = millis() + 1000;  // 1s
    pixel.clear(); pixel.show();
  }
}