#ifndef _TPMS_H_
#define _TPMS_H_

#include <Arduino.h>

void initTPMS();   // Initialize BLE for TPMS
void getTPMS();    // Scan for BLE TPMS devices

String retmanData(String txt, int shift);
byte retByte(String Data,int start);
long returnData(String Data,int start);
int returnBatt(String Data);
int returnAlarm(String Data);

#endif  // _TPMS_H_