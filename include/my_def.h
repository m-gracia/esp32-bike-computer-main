#ifndef _MY_DEF_H_
#define _MY_DEF_H_
#include <Arduino.h>
#include "secrets.h" // To store secret information used here. You must create first. Look on https://github.com/m-gracia for the contents.

// Bluetooth
// https://specificationrefs.bluetooth.com/assigned-values/Appearance%20Values.pdf
#define BT_GENERIC_DISPLAY 320
#define BT_VEHICLE_MOTORBIKE 2244
#define BT_DEVICE_NAME "Jarvis"     // BLE device name
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEAdvertisedDevice.h>
#include <BLEAddress.h>
static const BLEUUID ALERT_DISPLAY_SERVICE_UUID = BLEUUID("cb99fdf2-bebe-cafe-b1bf-bf9803b2df00");          
static const BLEUUID DISPLAY_MESSAGE_CHARACTERISTIC_UUID = BLEUUID("cb99fdf2-bebe-cafe-b1bf-bf9803b2df00");     // Notification text
static const BLEUUID DISPLAY_TIME_CHARACTERISTIC_UUID = BLEUUID("b7b0a14b-3e94-488f-b262-5d584a1ef9e1");        // Time and phone battery
static const BLEUUID DISPLAY_DISPLAY_ORIENTATION_CHARACTERISTIC_UUID = BLEUUID("0070b87e-d825-43f5-be0c-7d86f75e4900");  // Not needed
static const BLEUUID MOTORBIKE_SERVICE_UUID = BLEUUID("cb99fdf2-bebe-cafe-b1bf-bf9803b2df01");
static const BLEUUID DISTANCE_CHARACTERISTIC_UUID = BLEUUID("cb99fdf2-bebe-cafe-b1bf-bf9803b2df01");            // Distance
extern int bikePhoneBattery;       // Phone battery

// TPMS
#define BTSCANTIME 5
#include <BLEScan.h>
static BLEAddress *pServerAddress;
static String TPMS_knownAddresses[] = { "81:ea:ca:22:23:03" , "83:ea:ca:42:25:0c"};  // TPMS sensors MAC: Front , Rear
static unsigned char tpmsBatteyLow = 5;  // Battery low level
static unsigned char tpmsLevel4 = 35;    // TPMS Pressure very high level
static unsigned char tpmsLevel3 = 32;    // TPMS Pressure high level
static unsigned char tpmsLevel2 = 23;    // TPMS Pressure low level
static unsigned char tpmsLevel1 = 21;    // TPMS Pressure very low level
extern BLEScan* pBLEScan;
extern int bikeTPMSF;       // TPMS Front Pressure data
extern int bikeTPMSFTemp;   // TPMS Front Temperature data
extern int bikeTPMSFBatt;   // TPMS Front Battery data
extern int bikeTPMSR;       // TPMS Rear Pressure data
extern int bikeTPMSRTemp;   // TPMS Rear Temperature data
extern int bikeTPMSRBatt;   // TPMS Rear Battery data

// LCD
//#define TFT_GFX_SCK 18   // SCL PIN ESP32
//#define TFT_GFX_MOSI 23  // SDA PIN ESP32
#define TFT_GFX_SCK 12   // SCL PIN ESP32-S3
#define TFT_GFX_MOSI 11  // SDA PIN ESP32-S3
#define TFT_GFX_MISO GFX_NOT_DEFINED // Not used on this LCD's
#define TFT_GFX_RESET  GFX_NOT_DEFINED //2   // Just testing
#include <SPI.h>
#include <U8g2lib.h> //v2.32.15 https://github.com/olikraus/u8g2
#include <FS.h>
#include <SPIFFS.h>
#include <Arduino_GFX_Library.h>    //v1.2.3 https://github.com/moononournation/Arduino_GFX
#include "BmpClass.h"
//static Arduino_DataBus *bus1 = new Arduino_ESP32SPI(21 /* DC */, 22 /* CS */, TFT_GFX_SCK, TFT_GFX_MOSI, TFT_GFX_MISO, HSPI /* spi_num */); //ESP32
static Arduino_DataBus *bus1 = new Arduino_ESP32SPI(35 /* DC */, 36 /* CS */, TFT_GFX_SCK, TFT_GFX_MOSI, TFT_GFX_MISO, HSPI /* spi_num */); //ESP32-S3
//static Arduino_DataBus *bus1 = new Arduino_SWSPI(35 /* DC */, 36 /* CS */, TFT_GFX_SCK, TFT_GFX_MOSI, TFT_GFX_MISO); //ESP32-S3
static Arduino_GFX *tftR = new Arduino_GC9A01(bus1, TFT_GFX_RESET, 0 /* rotation */, true /*IPS*/);
//static Arduino_DataBus *bus2 = new Arduino_ESP32SPI(21 /* DC */, 17 /* CS 26 default */, TFT_GFX_SCK, TFT_GFX_MOSI, TFT_GFX_MISO, HSPI /* spi_num */); // ESP32
static Arduino_DataBus *bus2 = new Arduino_ESP32SPI(35 /* DC */, 18 /* CS 26 default */, TFT_GFX_SCK, TFT_GFX_MOSI, TFT_GFX_MISO, HSPI /* spi_num */); // ESP32-S3
static Arduino_GFX *tftS = new Arduino_ST7789(bus2, TFT_GFX_RESET, 2 /* rotation */, true /*IPS*/, 240 /* width */, 280 /* height */,0,0,0,20 );

#define FONT_SIGNAL u8g2_font_fub30_tn 
#define FONT_BIG u8g2_font_logisoso92_tn
#define FONT_MEDIUM2 u8g2_font_inb21_mr
//#define FONT_MEDIUM u8g2_font_lubB18_tf
//#define FONT_MEDIUM2 u8g2_font_logisoso24_tr
#define FONT_MEDIUM u8g2_font_luBS18_tr 
#define FONT_SMALL u8g2_font_helvB14_tf
#define TFT_TIMEMESSAGE 7000    // Phone alert message show time (7 sec)
extern String tftMsgTitle;      // Phone msg Title received
extern String tftMsgBody;       // Phone msg Body received
extern String tftMsgApp;        // Phone msg Application received

// Time
#include <Time.h>
#include <TimeLib.h>
#include <Timezone.h>   // https://github.com/JChristensen/Timezone
static TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     // Central European Summer Time
static TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       // Central European Standard Time
static Timezone CE(CEST, CET);

// GPS
//#define GPS_PIN_TX 33        // <- Connected to RX pin on SIM808 ESP32
//#define GPS_PIN_RX 26        // <- Connected to TX pin on SIM808 ESP32
#define GPS_PIN_TX 4           // <- Connected to RX pin on SIM808 ESP32-S3
#define GPS_PIN_RX 2           // <- Connected to TX pin on SIM808 ESP32-S3
#define gps_dev Serial2        // Hardware UART
#define GPS_UART_SPEED 9600    // UART Speed
#include <TinyGPSPlus.h>       // https://github.com/mikalhart/TinyGPSPlus
static TinyGPSPlus gps;

// Wifi
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArduinoHttpClient.h>  // https://github.com/arduino-libraries/ArduinoHttpClient
static WiFiMulti wifi_dev;
static const char wifi_ssid01[] = WIFI_SSID01;      // Defined in secrets.h
static const char wifi_passwd01[] = WIFI_PASS01;    // Defined in secrets.h
static const char wifi_ssid02[] = WIFI_SSID02;      // Defined in secrets.h
static const char wifi_passwd02[] = WIFI_PASS02;    // Defined in secrets.h
static const char http_server[] = HTTP_SERVER;      // Defined in secrets.h
static const char http_path[] = HTTP_PATH;          // Defined in secrets.h
static const int http_port = 80;
static const char web_passw[] = WEB_PASS;           // Defined in secrets.h
extern int bikeHeigh;                // GPS Heigh
extern int bikeSpeed;                // GPS Speed
extern unsigned char bikeHour;       // GPS hour
extern unsigned char bikeMinute;     // GPS minute
extern unsigned char bikeDay;        // GPS day
extern unsigned char bikeMonth;      // GPS month
extern unsigned char bikeSatellites; // GPS Satellites
extern bool firstGPRS;               // First GPRS send
extern float bikeLatitude;           // GPS Latitude
extern float bikeLongitude;          // GPS Longitude

// Weather OpenWeatherMap
#define WEATHER_TXT_SIZE 13
#include <ArduinoJson.h>    // https://github.com/bblanchon/ArduinoJson
static const char weather_server[] = "api.openweathermap.org";
static const int weather_port = 80;
static String weather_apikey = WEATHER_APIKEY; // Defined in secrets.h
static String weather_cityId = WEATHER_CITYID; // Default city ID. Defined in secrets.h
extern String weatherLocation;          // City
//extern String weatherTemperature;     // Temperature
//extern String weatherWeather;         // Weather text
//extern String weatherDescription;     // Weather description
//extern float weatherWind;             // Wind speed
extern int weatherIcon;                 // Weather icon

// Microsoft Maps
#define MAPS_TXT_SIZE 15
static const char maps_server[] = "dev.virtualearth.net";
static const int maps_port = 80;
static String maps_apikey = MAPS_APIKEY;    // Defined in secrets.h
extern String mapsStreet;                   // Street
extern int mapsSpeed;                       // Speed limit
extern bool mapsSpeedAlert;                 // Speed limit exceded

// Webserver
#include <WebServer.h>
//#include <HTTPUpdateServer.h>
//#include <AsyncTCP.h>
//#include <ESPAsyncWebServer.h>
// static const char wifi_ssid[] = "JARVIS";
// static const char wifi_passwd[] = WIFI_PASS;  // Defined in secrets.h
// static const char ota_passwd[] = OTA_PASS;    // Defined in secrets.h
 static WebServer webSrv(80);
//static HTTPUpdateServer updateWebServer;

// NeoPixel
#include <Adafruit_NeoPixel.h>
#define PIXEL_PIN 47
static Adafruit_NeoPixel pixel(1 /*NUM PIXELS*/, PIXEL_PIN, NEO_RGB + NEO_KHZ800);

//Status IDs
#define STATUS_UNK 8        // Unknown
#define STATUS_CRIT 2       // Critical
#define STATUS_WARN 1       // Warning
#define STATUS_OK 0         // OK
#define STATUS_POWERON 1    // Powering ON

//Status variables
extern unsigned char bikeBT;         // Bluetooth Status
extern unsigned char bikeGPS;        // GPS Status
extern unsigned char bikeGPRS;       // GPRS Status
extern unsigned char bikeDistance;   // Distance Status
extern unsigned char bikeNotif;      // Notify Status
extern unsigned char bikeCPU;        // Back CPU Status

/* To show changes on LCD's
 *
 * The bits are set when a new information needs to be updated on the LCD's
 * 1 - GPS Speed (Not used here)
 * 2 - GPRS Status (Not used here)
 * 3 - TPMS Front
 * 4 - TPMS Rear
 * 5 - GPS Heigh
 * 6 - Hour,minute
 * 7 - Date
 * 8 - CPU status (Not used here)
 * 9 - GPS Status
 * 10 - Phone battery
 * 11 - Street name
 * 12 - Speed limit
 * 13 - GPS Satellites
 * 14 - City and weather
 * 15 - USonic distance
 */
extern unsigned long bikeDataChanged;


// Run timers
extern unsigned long timerGPRS;
extern unsigned long timerBT;
extern unsigned long timerWeather;
extern unsigned long timerMaps;
extern unsigned long timerGPS;
extern unsigned long timerDisplay;
extern unsigned long timerMsg;
extern unsigned long timerTPMS;

// Tasks
extern TaskHandle_t T_loopOthers;

#endif  // _MY_DEF_H_