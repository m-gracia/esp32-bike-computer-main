#include <Arduino.h>
#include "my_def.h"

int bikePhoneBattery = 0;

BLEScan* pBLEScan = NULL;
int bikeTPMSF = 0;
int bikeTPMSFTemp = 0;
int bikeTPMSFBatt = 0;
int bikeTPMSR = 0;
int bikeTPMSRTemp = 0;
int bikeTPMSRBatt = 0;

String tftMsgTitle = "";
String tftMsgBody = "";
String tftMsgApp = "";

int bikeHeigh = 0;
int bikeSpeed = 0;
unsigned char bikeHour = 00;
unsigned char bikeMinute = 00;
unsigned char bikeDay = 0;
unsigned char bikeMonth = 0;
unsigned char bikeSatellites = 0;
bool firstGPRS = true;
float bikeLatitude = 0;
float bikeLongitude = 0;

String weatherLocation = "";
//String weatherTemperature;
//String weatherWeather;
//String weatherDescription;
//float weatherWind;
int weatherIcon = 0; //Shows nothing

String mapsStreet = "";
int mapsSpeed = 0;
bool mapsSpeedAlert = false;

unsigned char bikeBT = STATUS_UNK;
unsigned char bikeGPS = STATUS_UNK;
unsigned char bikeGPRS = STATUS_UNK;
unsigned char bikeDistance = STATUS_OK;
unsigned char bikeNotif = STATUS_OK;
unsigned char bikeCPU = STATUS_UNK;

unsigned long bikeDataChanged = 0xFFFFFFFF;

unsigned long timerGPRS = 0;
unsigned long timerBT = millis() + 10000;
unsigned long timerWeather = 0;
unsigned long timerMaps = millis() + 20000;
unsigned long timerGPS = 0;
unsigned long timerDisplay = 0;
unsigned long timerMsg = 0;
unsigned long timerTPMS = millis() + 5000;

TaskHandle_t T_loopOthers = NULL;