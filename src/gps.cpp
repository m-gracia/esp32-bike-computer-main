#include "gps.h"
#include "my_def.h"
#include "my_debug.h"

/* getGPS()
 * Feed the GPS and get data
 */
void getGPS(){
  float tLat, tLon, tSpeed, tAltitud; 
  int itAltitud, tSatellites;
  bool fix;
  int tYear, tMonth, tDay, tHour, tMinute, tSecond;
  int tbikeGPS = STATUS_OK;
  
  fix = gsm.getGPSTime(&tYear,&tMonth,&tDay,&tHour,&tMinute,&tSecond);
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
  
  if (!fix) tbikeGPS = STATUS_WARN;
  
  if (tYear > 2020){    // If the GPS return a realistic date
    setTime(tHour,tMinute,tSecond,tDay,tMonth,tYear); // Update the internal clock
    setTime(CE.toLocal(now())); // Change to our timezone
    
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
  }

  fix = gsm.getGPS(&tLat,&tLon,&tSpeed,&tAltitud,&tSatellites,NULL);
  DEBUG_GPS_PRINT(F("Speed "));
  DEBUG_GPS_PRINTLN(String(tSpeed,1));
  DEBUG_GPS_PRINT(F("Altitude "));
  DEBUG_GPS_PRINTLN(tAltitud);
  DEBUG_GPS_PRINT(F("Position "));
  DEBUG_GPS_PRINT(String(tLat,6));
  DEBUG_GPS_PRINT(F(" | "));
  DEBUG_GPS_PRINTLN(String(tLon,6));
  DEBUG_GPS_PRINT(F("Satellites "));
  DEBUG_GPS_PRINTLN(tSatellites);
  
  if(!fix) tbikeGPS = STATUS_WARN;
  
  bikeLatitude = tLat;
  bikeLongitude = tLon;

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

  if (tAltitud < 0 || tAltitud > 3000) tAltitud = 0;  // To solve some parsing problems
  if ( bikeHeigh != tAltitud) {
    bikeHeigh = tAltitud;
    bitSet(bikeDataChanged,5);      
  }

  if (tSatellites < 0 || tSatellites > 99) tSatellites = 0; // To solve some parsing problems
  if ( bikeSatellites != tSatellites) {
    bikeSatellites = tSatellites;
    bitSet(bikeDataChanged,13);
  }

  if (tbikeGPS != bikeGPS){
    bikeGPS = tbikeGPS;
    bitSet(bikeDataChanged,9);
  }
}
