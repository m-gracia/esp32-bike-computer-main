#ifndef _GPRS_H_
#define _GPRS_H_

void initGPRS();    // Initialize GPRS and GPS
void sendGPRS();    // Send data to server
void getWeather();  // Get data from OpenWeatherMap
void getMaps();     // Get data from Microsoft Maps
void getGPS();     // Get data from GPS

#endif  // _GPRS_H_