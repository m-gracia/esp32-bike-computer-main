#ifndef _GPRS_H_
#define _GPRS_H_

void initWifi();        // Initialize Wifi
void getWeather();      // Get data from OpenWeatherMap
void getMapsStreet();   // Get street data from Microsoft Maps
void getMapsSpeed();    // Get speed limit data from Microsoft Maps
void sendLocation();    // Send location to server
void useGPRS();         // Centralized function to use GPRS

#endif  // _GPRS_H_