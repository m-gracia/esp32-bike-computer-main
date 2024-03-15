#include "gps.h"
#include "my_def.h"
#include "my_debug.h"

/* initGPS()
 * Init Serial device where GPS is connected
 */
void initGPS(){
    gps_dev.begin(GPS_UART_SPEED,SERIAL_8N1,GPS_PIN_TX,GPS_PIN_RX);
    bikeGPS = STATUS_CRIT; // Initialized but without data
    DEBUG_GPS_PRINTLN("GPS Init end");
}


/* feedGPS()
 * Feed the gps library
 */
void feedGPS(){
    char c;

    DEBUG_GPSRAW_PRINT("GPSRAW: ");
    while (gps_dev.available() > 0){
        c = gps_dev.read();
        gps.encode(c);
        DEBUG_GPSRAW_PRINT(c);
    }
    DEBUG_GPSRAW_PRINTLN("");

    if (millis() > 5000 && gps.charsProcessed() < 10){
        DEBUG_GPS_PRINTLN("GPS Failed to communicate with module");
        if (bikeGPS != STATUS_CRIT){
            bikeGPS = STATUS_CRIT;
            bitSet(bikeDataChanged,9);
        }
    }
}


/* getGPS()
 * Get data from GPS
 */
void getGPS(){
  float tLat = 0;
  float tLon = 0;
  float tSpeed = 0; 
  float tAltitud = 0;
  int tSatellites = 0;
  int tYear = 0;
  int tMonth = 0;
  int tDay = 0;
  int tHour = 0;
  int tMinute = 0;
  int tSecond = 0;
  int tbikeGPS = STATUS_OK;
  
  // Date and time
  if (gps.time.isValid() && gps.date.isValid()){
    tHour = gps.time.hour();
    tMinute = gps.time.minute();
    tSecond = gps.time.second();
    tDay = gps.date.day();
    tMonth = gps.date.month();
    tYear = gps.date.year();

    setTime(tHour,tMinute,tSecond,tDay,tMonth,tYear); // Update the internal clock
    setTime(CE.toLocal(now())); // Change to our timezone only if taken from GPS (UTC)

    if (bikeMonth != month()) {
      bikeMonth = month();
      bitSet(bikeDataChanged,7);
    }
    if (bikeDay != day()) {
      bikeDay = day();
      bitSet(bikeDataChanged,7);
    }
    if (bikeHour != hour()) {
      bikeHour = hour();
      bitSet(bikeDataChanged,6);
    }
    if (bikeMinute != minute()) {
      bikeMinute = minute();
      bitSet(bikeDataChanged,6);
    }
    DEBUG_GPS_PRINT(F("DAY "));
    DEBUG_GPS_PRINT(tDay);
    DEBUG_GPS_PRINT(F("/"));
    DEBUG_GPS_PRINT(tMonth);
    DEBUG_GPS_PRINT(F("/"));
    DEBUG_GPS_PRINTLN(tYear);
    DEBUG_GPS_PRINT(F("HOUR "));
    DEBUG_GPS_PRINT(tHour);
    DEBUG_GPS_PRINT(F(":"));
    DEBUG_GPS_PRINTLN(tMinute);
  } else {
    tbikeGPS = STATUS_WARN;
    // Failover to less acurate GPRS location and time

  }

  // Location
  if (gps.location.isValid()){
    bikeLatitude = gps.location.lat();
    bikeLongitude = gps.location.lng();

    DEBUG_GPS_PRINT(F("Position "));
    DEBUG_GPS_PRINT(String(bikeLatitude,6));
    DEBUG_GPS_PRINT(F(" | "));
    DEBUG_GPS_PRINTLN(String(bikeLongitude,6));
  } else {
    tbikeGPS = STATUS_WARN;
  }

  // Speed
  if (gps.speed.isValid()){
    tSpeed = gps.speed.kmph();
    if (tSpeed > 250) tSpeed = 0;   // To solve some parsing problems
      bikeSpeed = (int)tSpeed;

    // Check if we are going 10% faster than we should
    unsigned char tmapsSpeedAlert;
    if ((bikeSpeed * 1,1) > mapsSpeed) tmapsSpeedAlert = true;
    else tmapsSpeedAlert = false;
    if (tmapsSpeedAlert != mapsSpeedAlert){
        mapsSpeedAlert = tmapsSpeedAlert;
        bitSet(bikeDataChanged,12);
    }

    DEBUG_GPS_PRINT(F("Speed "));
    DEBUG_GPS_PRINTLN(String(tSpeed,1));
  } else {
    tbikeGPS = STATUS_WARN;
  }
 
  // Altitude
  if (gps.altitude.isValid()){
    tAltitud = gps.altitude.meters();
    if (tAltitud < 0 || tAltitud > 3000) tAltitud = 0;  // To solve some parsing problems
    if ( bikeHeigh != tAltitud) {
        bikeHeigh = tAltitud;
        bitSet(bikeDataChanged,5);      
    }

    DEBUG_GPS_PRINT(F("Altitude "));
    DEBUG_GPS_PRINTLN(tAltitud);
  } else {
    tbikeGPS = STATUS_WARN;
  }
  
  // Satellites
  if (gps.satellites.isValid()){
    tSatellites = gps.satellites.value();
    if (tSatellites < 0 || tSatellites > 99) tSatellites = 0; // To solve some parsing problems
    if ( bikeSatellites != tSatellites) {
        bikeSatellites = tSatellites;
        bitSet(bikeDataChanged,13);
    }

    DEBUG_GPS_PRINT(F("Satellites "));
    DEBUG_GPS_PRINTLN(tSatellites);
  } else {
    tbikeGPS = STATUS_WARN;
  }  

  if (tbikeGPS != bikeGPS){
    bikeGPS = tbikeGPS;
    bitSet(bikeDataChanged,9);
  }
}
