#ifndef _GPRS_H_
#define _GPRS_H_

void initGPRS();        // Initialize GPRS and GPS
void getWeather();      // Get data from OpenWeatherMap
void getMaps();         // Get data from Microsoft Maps
void sendLocation();    // Send location to server
void useGPRS();         // Centralized function to use GPRS
void getGPS();          // Get data from GPS

#endif  // _GPRS_H_