/*
 * Debug functions
 */
#ifndef _MY_DEBUG_H_
#define _MY_DEBUG_H_

#define DEBUG       // General debug switch. No data sent if commented

// Uncomment to enable
//#define DEBUG_GPRS
//#define DEBUG_GPS
//#define DEBUG_GPSRAW
//#define DEBUG_BT
#define DEBUG_TFT
//#define DEBUG_MSG
//#define DEBUG_TPMS



// Don't change below this line
#ifdef DEBUG
    #define DEBUG_PRINT(x)  Serial.print(x)
    #define DEBUG_PRINTLN(x)  Serial.println(x)

    #ifdef DEBUG_GPRS
    #define DEBUG_GPRS_PRINT(x)  Serial.print(x)
    #define DEBUG_GPRS_PRINTLN(x)  Serial.println(x)
    #else
    #define DEBUG_GPRS_PRINT(x)
    #define DEBUG_GPRS_PRINTLN(x)
    #endif

    #ifdef DEBUG_GPS
    #define DEBUG_GPS_PRINT(x)  Serial.print(x)
    #define DEBUG_GPS_PRINTLN(x)  Serial.println(x)
    #else
    #define DEBUG_GPS_PRINT(x)
    #define DEBUG_GPS_PRINTLN(x)
    #endif

    #ifdef DEBUG_GPSRAW
    #define DEBUG_GPSRAW_PRINT(x)  Serial.print(x)
    #define DEBUG_GPSRAW_PRINTLN(x)  Serial.println(x)
    #else
    #define DEBUG_GPSRAW_PRINT(x)
    #define DEBUG_GPSRAW_PRINTLN(x)
    #endif

    #ifdef DEBUG_BT
    #define DEBUG_BT_PRINT(x)  Serial.print(x)
    #define DEBUG_BT_PRINTLN(x)  Serial.println(x)
    #else
    #define DEBUG_BT_PRINT(x)
    #define DEBUG_BT_PRINTLN(x)
    #endif
    #ifdef DEBUG_TFT
    #define DEBUG_TFT_PRINT(x)  Serial.print(x)
    #define DEBUG_TFT_PRINTLN(x)  Serial.println(x)
    #else
    #define DEBUG_TFT_PRINT(x)
    #define DEBUG_TFT_PRINTLN(x)
    #endif

    #ifdef DEBUG_MSG
    #define DEBUG_MSG_PRINT(x)  Serial.print(x)
    #define DEBUG_MSG_PRINTLN(x)  Serial.println(x)
    #else
    #define DEBUG_MSG_PRINT(x)
    #define DEBUG_MSG_PRINTLN(x)
    #endif

    #ifdef DEBUG_TPMS
    #define DEBUG_TPMS_PRINT(x)  Serial.print(x)
    #define DEBUG_TPMS_PRINTLN(x)  Serial.println(x)
    #else
    #define DEBUG_TPMS_PRINT(x)
    #define DEBUG_TPMS_PRINTLN(x)
    #endif

#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_GPRS_PRINT(x)
    #define DEBUG_GPRS_PRINTLN(x)
    #define DEBUG_GPS_PRINT(x)
    #define DEBUG_GPS_PRINTLN(x)
    #define DEBUG_GPSRAW_PRINT(x)
    #define DEBUG_GPSRAW_PRINTLN(x)
    #define DEBUG_BT_PRINT(x)
    #define DEBUG_BT_PRINTLN(x)
    #define DEBUG_TFT_PRINT(x)
    #define DEBUG_TFT_PRINTLN(x)
    #define DEBUG_MSG_PRINT(x)
    #define DEBUG_MSG_PRINTLN(x)
    #define DEBUG_TPMS_PRINT(x)
    #define DEBUG_TPMS_PRINTLN(x)
#endif

#endif  // _MY_DEBUG_H_