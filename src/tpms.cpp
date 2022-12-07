/* TPMS BLE ESP32
2020 RA6070
v0.2 06/08/20
https://github.com/ra6070/BLE-TPMS

TPMS BLE "manufacturer data" format
"000180eaca108a78e36d0000e60a00005b00"
 0001                                    Manufacturer (0001: TomTom)
     80                                  Sensor Number (80:1, 81:2, 82:3, 83:4, ..)
     80eaca108a78                        Sensor Address
                 e36d0000                Pressure
                         e60a0000        Temperature
                                 5b      Battery percentage
                                   00    Alarm Flag (00: OK, 01: No Pressure Alarm)

How calculate Sensor Address:            (Sensor number):EA:CA:(Code binding reported in the leaflet) - i.e. 80:EA:CA:10:8A:78
*/

#include <Arduino.h>
#include "tpms.h"
#include "my_def.h"
#include "my_debug.h"

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    DEBUG_TPMS_PRINT("Notify callback for characteristic ");
    DEBUG_TPMS_PRINT(pBLERemoteCharacteristic->getUUID().toString().c_str());
    DEBUG_TPMS_PRINT(" of data length ");
    DEBUG_TPMS_PRINTLN(length);
}
  
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice Device){
    DEBUG_TPMS_PRINT("BLE Advertised: ");
    DEBUG_TPMS_PRINTLN(Device.toString().c_str());
    pServerAddress = new BLEAddress(Device.getAddress());    

    //Front TPMS
    if (strcmp(pServerAddress->toString().c_str(), TPMS_knownAddresses[0].c_str()) == 0){
      String instring = retmanData(Device.toString().c_str(), 0);
      int tbikeTPMSFTemp = (int)returnData(instring,12)/100;
      int tbikeTPMSF = (int)returnData(instring,8)/10000;
      int tbikeTPMSFBatt = returnBatt(instring);
      if(tbikeTPMSFTemp != bikeTPMSFTemp || tbikeTPMSF != bikeTPMSF || tbikeTPMSFBatt != bikeTPMSFBatt )
        bitSet(bikeDataChanged,3);
        
      bikeTPMSFTemp = tbikeTPMSFTemp; 
      bikeTPMSF = tbikeTPMSF;
      bikeTPMSFBatt = tbikeTPMSFBatt;

      DEBUG_TPMS_PRINT("TPMSF found: ");
      DEBUG_TPMS_PRINTLN(Device.getRSSI());
      DEBUG_TPMS_PRINT("    Pressure: ");
      DEBUG_TPMS_PRINTLN(tbikeTPMSF);
      DEBUG_TPMS_PRINT("    Temperature: ");
      DEBUG_TPMS_PRINTLN(tbikeTPMSFTemp);
      DEBUG_TPMS_PRINT("    Battery: ");
      DEBUG_TPMS_PRINTLN(tbikeTPMSFBatt);
    }

    //Rear TPMS
    if (strcmp(pServerAddress->toString().c_str(), TPMS_knownAddresses[1].c_str()) == 0){
      String instring = retmanData(Device.toString().c_str(), 0);
      int tbikeTPMSRTemp = (int)returnData(instring,12)/100;
      int tbikeTPMSR = (int)returnData(instring,8)/10000;
      int tbikeTPMSRBatt = returnBatt(instring);
      if(tbikeTPMSRTemp != bikeTPMSRTemp || tbikeTPMSR != bikeTPMSR || tbikeTPMSRBatt != bikeTPMSRBatt )
        bitSet(bikeDataChanged,4);
        
      bikeTPMSRTemp = tbikeTPMSRTemp; 
      bikeTPMSR = tbikeTPMSR;
      bikeTPMSRBatt = tbikeTPMSRBatt;

      DEBUG_TPMS_PRINT("TPMSR found: ");
      DEBUG_TPMS_PRINTLN(Device.getRSSI());
      DEBUG_TPMS_PRINT("    Pressure: ");
      DEBUG_TPMS_PRINTLN(tbikeTPMSR);
      DEBUG_TPMS_PRINT("    Temperature: ");
      DEBUG_TPMS_PRINTLN(tbikeTPMSRTemp);
      DEBUG_TPMS_PRINT("   Battery: ");
      DEBUG_TPMS_PRINTLN(tbikeTPMSRBatt);
    }

    free(pServerAddress);
    //Device.getScan()->stop();
    /*      
    // Tire Temperature in C°
    Serial.print("Temperature: ");
    Serial.print(returnData(instring,12)/100.0);
    Serial.println("C°");
    // Tire pressure in Kpa           
    Serial.print("Pressure:    ");
    Serial.print(returnData(instring,8)/1000.0);
    Serial.println("Kpa");
    // Tire pressure in Bar           
    Serial.print("Pressure:    ");
    Serial.print(returnData(instring,8)/100000.0);
    Serial.println("bar");
    // Battery percentage             
    Serial.print("Battery:     ");
    Serial.print(returnBatt(instring));
    Serial.println("%");
    if (returnAlarm(instring)) {
      Serial.println("ALARM!");
    }
    */      
  }
};

static void scanCompleteCB(BLEScanResults scanResults) {
  DEBUG_TPMS_PRINTLN("TPMS Scan complete");
  pBLEScan->clearResults();
}

void initTPMS() {
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  /* Original
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);  // less or equal setInterval value
  */
  pBLEScan->setInterval(300);
  pBLEScan->setWindow(300);  // less or equal setInterval value
  DEBUG_BT_PRINTLN("TPMS Initiated");
}

void getTPMS(){
  DEBUG_TPMS_PRINTLN("TPMS Scan init");
  pBLEScan->start(BTSCANTIME, scanCompleteCB);
}


// FUNCTIONS
String retmanData(String txt, int shift) {
  // Return only manufacturer data string
  int start=txt.indexOf("data: ")+6+shift;
  return txt.substring(start,start+(36-shift));  
}

byte retByte(String Data,int start) {
  // Return a single byte from string
  int sp=(start)*2;
  char *ptr;
  return strtoul(Data.substring(sp,sp+2).c_str(),&ptr, 16);
}

long returnData(String Data,int start) {
  // Return a long value with little endian conversion
  return retByte(Data,start)|retByte(Data,start+1)<<8|retByte(Data,start+2)<<16|retByte(Data,start+3)<<24;
}

int returnBatt(String Data) {
  // Return battery percentage
  return retByte(Data,16);
}

int returnAlarm(String Data) {
  // Return battery percentage
  return retByte(Data,17);
}
