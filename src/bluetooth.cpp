#include "bluetooth.h"
#include "my_def.h"
#include "my_debug.h"


class MyServerCallback: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    DEBUG_BT_PRINTLN("BT Device connected");
    BLEDevice::startAdvertising();
  }

  void onDisconnect(BLEServer* pServer) {
    DEBUG_BT_PRINTLN("BT Device disconnected");
    bikeCPU = STATUS_CRIT;
    BLEDevice::startAdvertising();
  }
};

class DisplayCharacteristicCallback: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {    
    String tftMessage = pCharacteristic->getValue().c_str();
    DEBUG_BT_PRINT("BT MSG received: ");
    DEBUG_BT_PRINTLN(tftMessage);

    if (tftMessage[0]=='#')
      tftMsgTitle = tftMessage.substring(1,tftMessage.length());
    else if (tftMessage[0]=='$')
      tftMsgBody = tftMessage.substring(1,tftMessage.length());
    else if (tftMessage[0]=='@'){
      tftMsgApp = tftMessage.substring(1,tftMessage.length());
      timerMsg = millis()+ TFT_TIMEMESSAGE; // Set the time to show
      bikeNotif = STATUS_CRIT; // Set to show
    }    
  }
};

class TimeCharacteristicCallback: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pTimeCharacteristic) {
    std::string str = pTimeCharacteristic->getValue(); //read values
    int tbikePhoneBattery = (int)str[0]; //first char indicates battery-level
    if (bikePhoneBattery != tbikePhoneBattery) bitSet(bikeDataChanged,10);
    bikePhoneBattery = tbikePhoneBattery;

    DEBUG_BT_PRINT("BT Battery msg received: ");
    DEBUG_BT_PRINTLN(str.c_str());
    DEBUG_BT_PRINT("BT Battery received: ");
    DEBUG_BT_PRINTLN(bikePhoneBattery);
  }
};

// Not used but required by Android application
class DisplayOrientationCharacteristicCallback: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    /*std::string str = pCharacteristic->getValue(); //read values
    display_orientation = (int)str[0];
    Serial.println(" DisplayOrientationCharacteristicCallback->onWrite()");*/
  }
};

// UltraSonic Distance
class DistanceCharacteristicCallback: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristicDistance) {
    std::string str = pCharacteristicDistance->getValue(); //read values
    unsigned char tbikeDistance = (unsigned char)str[0];
    
    if (bikeSpeed > 20) tbikeDistance = STATUS_OK;   // Do not show if the speed is > 20km/h
    else {
      if(tbikeDistance == STATUS_OK){ // Show others
        DEBUG_BT_PRINTLN("BT Reset display");
        bitSet(bikeDataChanged,7);
        bitSet(bikeDataChanged,14);
      } else bitSet(bikeDataChanged,15);

      bikeDistance = tbikeDistance;
    }

    DEBUG_BT_PRINT("BT Distance msg received: ");
    DEBUG_BT_PRINTLN(str.c_str());
    DEBUG_BT_PRINT("BT Distance received: ");
    DEBUG_BT_PRINTLN(tbikeDistance);
  }
};

void initBT(){
  BLEDevice::init(BT_DEVICE_NAME);
  BLEDevice::setMTU(256); //MTU SIZE

  /***** Service0 for Phone *****/
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallback());
  BLEService *pService = pServer->createService(ALERT_DISPLAY_SERVICE_UUID);
  DEBUG_BT_PRINTLN("BT Phone Server created");
  
  // Message Characteristic
  BLECharacteristic* pCharacteristicText = new BLECharacteristic(DISPLAY_MESSAGE_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE_NR);
  pCharacteristicText->setCallbacks(new DisplayCharacteristicCallback());
  pService->addCharacteristic(pCharacteristicText);

  // Battery and Time Characteristic
  BLECharacteristic* pCharacteristicTime = new BLECharacteristic(DISPLAY_TIME_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE_NR);
  pCharacteristicTime->setCallbacks(new TimeCharacteristicCallback());
  pService->addCharacteristic(pCharacteristicTime); 
    
  // Not used but required by Android application
  BLECharacteristic* pCharacteristicDisplayOrientation = new BLECharacteristic(DISPLAY_DISPLAY_ORIENTATION_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE_NR);
  pCharacteristicDisplayOrientation->setCallbacks(new DisplayOrientationCharacteristicCallback());
  pService->addCharacteristic(pCharacteristicDisplayOrientation); 


  /***** Service1 for other ESP *****/
  BLEService *pService1 = pServer->createService(MOTORBIKE_SERVICE_UUID);
  DEBUG_BT_PRINTLN("BT ESP Server created");
  //Distance
  BLECharacteristic* pCharacteristicDistance = new BLECharacteristic(DISTANCE_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE_NR);
  pCharacteristicDistance->setCallbacks(new DistanceCharacteristicCallback());
  pService1->addCharacteristic(pCharacteristicDistance);


  pService->start();
  pService1->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(MOTORBIKE_SERVICE_UUID);
  pAdvertising->addServiceUUID(ALERT_DISPLAY_SERVICE_UUID); 
  BLEDevice::startAdvertising();
}
