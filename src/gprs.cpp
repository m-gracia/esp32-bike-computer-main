#include "gprs.h"
#include "my_def.h"
#include "my_debug.h"


void initGPRS(){
  DEBUG_GPRS_PRINTLN("GPRS Init start");

  gsmDev.begin(9600,SERIAL_8N1,GPRS_PIN_RX,GPRS_PIN_TX,false); // Try at default poweron baudrate
  if (!gsm.init()){
    DEBUG_GPRS_PRINTLN("GPRS Init fail at 9600");
    gsmDev.begin(115200,SERIAL_8N1,GPRS_PIN_RX,GPRS_PIN_TX,false);  //Try faster
    if (!gsm.init()) DEBUG_GPRS_PRINTLN("GPRS Init fail also at 115200");
  }
  else{
    gsm.setBaud(115200);
    gsmDev.end();
    gsmDev.begin(115200,SERIAL_8N1,GPRS_PIN_RX,GPRS_PIN_TX,false);
  }
  
  if (!gsm.enableGPS()){ 
    DEBUG_GPS_PRINTLN("GPS Init fail");
    DEBUG_GPS_PRINT("GPS ERROR: ");
    DEBUG_GPS_PRINTLN(gsm.getGPSraw());
    delay(200);
    if (!gsm.enableGPS()){
      DEBUG_GPS_PRINTLN("GPS Init fail2");
      DEBUG_GPS_PRINT("GPS ERROR: ");
      DEBUG_GPS_PRINTLN(gsm.getGPSraw());
    }
  }
  
  timerGPRS = 0;
  bikeGPRS = STATUS_CRIT; // Initialized but without data
  bikeGPS = STATUS_CRIT; // Initialized but without data
  DEBUG_GPRS_PRINTLN("GPRS Init end");
  DEBUG_GPS_PRINTLN("GPS Init end");
}

void sendGPRS(){
  if (!gsm.waitForNetwork()) {
    DEBUG_GPRS_PRINTLN("GPRS Network NOT ready");
    if (bikeGPRS != STATUS_CRIT) bitSet(bikeDataChanged,2);
    bikeGPRS = STATUS_CRIT;
  } else if (!gsm.gprsConnect(gprs_apn, gprs_user, gprs_pass)){
    DEBUG_GPRS_PRINTLN("GPRS Network NOT connected");
    if (bikeGPRS != STATUS_CRIT) bitSet(bikeDataChanged,2);
    bikeGPRS = STATUS_CRIT;
  } else {
    unsigned char gprsEnvio;
    // Send poweron flag the first time, otherwise send if the phone is connected.
    if(firstGPRS) gprsEnvio = STATUS_POWERON;
    else gprsEnvio = bikeBT;
    
    String http_url;
      http_url = String(http_path) + "/p.php?u=0&p=" + String(web_passw) + "&s=" + String(gprsEnvio);
      http_url = http_url + "&a=" + String(bikeHeigh);
      http_url = http_url + "&la=" + String(bikeLatitude,8);
      http_url = http_url + "&l=" + String(bikeLongitude,8);
      DEBUG_GPRS_PRINT("Web URL: ");
      DEBUG_GPRS_PRINTLN(http_url);

    if(http_client.get(http_url) != 0){
      DEBUG_GPRS_PRINTLN("GPRS fail get http");
      if (bikeGPRS != STATUS_WARN) bitSet(bikeDataChanged,2);
      bikeGPRS = STATUS_WARN;
    } else {
      DEBUG_GPRS_PRINTLN("GPRS http connected OK");
      // Change the status on success
      if(firstGPRS) firstGPRS = false;
      if(bikeGPRS != STATUS_OK) bitSet(bikeDataChanged,2);
      bikeGPRS = STATUS_OK;
    }

    http_client.stop();
    DEBUG_GPRS_PRINTLN("GPRS http disconnected");
  }
}
