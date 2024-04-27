#include "lcd.h"
#include "my_def.h"
#include "my_debug.h"
#include "BmpClass.h"

void initDisplay(){
  DEBUG_TFT_PRINTLN("TFT Init");
  SPIFFS.begin();
  tftS->begin();  // MUST BE INITIATED BEFORE THE ROUND LCD
  tftR->begin();
  
  tftR->fillScreen(BLACK);
  tftS->fillScreen(BLACK);
  bikeDataChanged = 0xFFFFFFFF;
  sendDisplay();
}

void sendDisplay(){
  if(bikeDataChanged != 0 || bikeNotif != STATUS_OK){ // Only if there is something changed
    tftR->setFont(FONT_MEDIUM);
    tftR->setTextColor(WHITE);
    tftR->setTextSize(1);
    tftS->setFont(FONT_MEDIUM);
    tftS->setTextColor(WHITE);
    tftS->setTextSize(1);
    
    if (bitRead(bikeDataChanged,3)){    // TPMSF
      DEBUG_TFT_PRINTLN("TPMSF CHG");
      tftR->fillRect(25,85,45,80,BLACK); //Clear text
      tftR->fillArc(120,120,115,105,140,220,BLACK); //Clear bar
      tftR->drawArc(120,120,117,103,138,222,WHITE); //Border
      if (bikeTPMSF == 0)               tftR->fillArc(120,120,115,105,140,220,LIGHTGREY); // full grey
      else if(bikeTPMSF < tpmsLevel1) tftR->fillArc(120,120,115,105,140,160,RED);       // 1 red
      else if(bikeTPMSF < tpmsLevel2) tftR->fillArc(120,120,115,105,140,180,YELLOW);    // 2 yellow
      else if(bikeTPMSF < tpmsLevel3) tftR->fillArc(120,120,115,105,140,200,GREEN);     // 3 green
      else if(bikeTPMSF < tpmsLevel4) tftR->fillArc(120,120,115,105,140,200,YELLOW);    // 3 yellow
      else                              tftR->fillArc(120,120,115,105,140,220,RED);       // 4 red

      tftR->fillArc(120,120,115,105,159,161,BLACK);
      tftR->fillArc(120,120,115,105,179,181,BLACK);
      tftR->fillArc(120,120,115,105,199,201,BLACK);

      tftR->setCursor(25,130);
      tftR->print(bikeTPMSF/10.0, 1);
      tftR->setFont(FONT_SMALL);
      tftR->setCursor(30,160);
      tftR->print(bikeTPMSFTemp);
      tftR->print(char(176));
      if(bikeTPMSFBatt < tpmsBatteyLow) drawBmpR("/icons/batteryLow.bmp",30,85,16,16);
    }
    
    if (bitRead(bikeDataChanged,4)){    // TPMSR
      DEBUG_TFT_PRINTLN("TPMSR CHG");
      tftR->fillRect(175,85,50,80,BLACK); //Clear text
      tftR->fillArc(120,120,115,105,320,40,BLACK); //Clear bar
      tftR->drawArc(120,120,117,103,318,42,WHITE); //Border
      if (bikeTPMSR == 0)               tftR->fillArc(120,120,115,105,320,40,LIGHTGREY); // full grey
      else if(bikeTPMSR < tpmsLevel1) tftR->fillArc(120,120,115,105,20,40,RED);        // 1 red
      else if(bikeTPMSR < tpmsLevel2) tftR->fillArc(120,120,115,105,0,40,YELLOW);      // 2 yellow
      else if(bikeTPMSR < tpmsLevel3) tftR->fillArc(120,120,115,105,340,40,GREEN);     // 3 green
      else if(bikeTPMSR < tpmsLevel4) tftR->fillArc(120,120,115,105,340,40,YELLOW);    // 3 yellow
      else                              tftR->fillArc(120,120,115,105,320,40,RED);       // 4 red

      tftR->fillArc(120,120,115,105,339,341,BLACK);
      tftR->fillArc(120,120,115,105,359,1,BLACK);
      tftR->fillArc(120,120,115,105,19,21,BLACK);

      tftR->setFont(FONT_MEDIUM);
      tftR->setCursor(175,130);
      tftR->print(bikeTPMSR/10.0, 1);
      tftR->setFont(FONT_SMALL);
      tftR->setCursor(180,160);
      tftR->print(bikeTPMSRTemp);
      tftR->print(char(176));
      if(bikeTPMSRBatt < tpmsBatteyLow) drawBmpR("/icons/batteryLow.bmp",190,85,16,16);
    }
    
    if (bitRead(bikeDataChanged,10)){   // BT PHONE
      DEBUG_TFT_PRINTLN("BT PHONE CHG");
      tftR->fillRect(105,25,45,25,BLACK); //Clear text
      tftR->fillArc(120,120,115,105,230,310,BLACK); // Clear bar
      tftR->drawArc(120,120,117,103,228,312,WHITE); // Border
      if(bikePhoneBattery == 0)      tftR->fillArc(120,120,115,105,230,310,LIGHTGREY);  // full gris
      else if(bikePhoneBattery < 6)  tftR->fillArc(120,120,115,105,230,250,RED);        // 1 red
      else if(bikePhoneBattery < 26) tftR->fillArc(120,120,115,105,230,250,YELLOW);     // 1 yellow
      else if(bikePhoneBattery < 51) tftR->fillArc(120,120,115,105,230,270,GREEN);      // 2 green
      else if(bikePhoneBattery < 76) tftR->fillArc(120,120,115,105,230,290,GREEN);      // 3 green
      else                        tftR->fillArc(120,120,115,105,230,310,GREEN);      // 4 green

      tftR->fillArc(120,120,115,105,249,251,BLACK);
      tftR->fillArc(120,120,115,105,269,271,BLACK);
      tftR->fillArc(120,120,115,105,289,291,BLACK);

      tftR->setFont(FONT_SMALL);
      tftR->setCursor(105,45);
      tftR->print(bikePhoneBattery);
      tftR->print("%");
    }

    if (bitRead(bikeDataChanged,11)){   // MAPS STREET
      DEBUG_TFT_PRINTLN("MAPS STREET CHG");
      tftR->fillRect(50,180,145,40,BLACK); //Clear text
      tftR->setFont(FONT_SMALL);
      tftR->setCursor(50,200);
      //Center text on screen
      int i=0;
      int l = (MAPS_TXT_SIZE - mapsStreet.substring(0,MAPS_TXT_SIZE).length()) /2;
      for (i=0; l>i; i++) 
        tftR->print(" ");
      tftR->print(mapsStreet.substring(0,MAPS_TXT_SIZE));
    }
    
    if (bikeNotif == STATUS_CRIT){  // PHONE MESSAGE
      DEBUG_TFT_PRINTLN("NOTIFICATIONS CHG");
      DEBUG_MSG_PRINTLN("TFT TITLE: " + tftMsgApp);
      DEBUG_MSG_PRINTLN("TFT WHO: " + tftMsgTitle);
      DEBUG_MSG_PRINTLN("TFT BODY: " + tftMsgBody);
      /*
        BT Data received: #Jhon Jones
        BT Data received: $Text
        BT Data received: @WhatsApp
        BT Data received: #Home Phone
        BT Data received: $Dialing
        BT Data received: @Phone
       */      
      tftS->fillScreen(BLACK);  // Clear square lcd
      tftR->fillRect(70,70,100,100,BLACK); // Clear image

      if (strcmp(tftMsgApp.substring(0,8).c_str(), "WhatsApp") == 0) drawBmpR("/icons/whatsapp.bmp",70,70,100,100);
      else if (strcmp(tftMsgApp.substring(0,5).c_str(), "Phone") == 0) drawBmpR("/icons/phone.bmp",70,70,100,100);
      else if (strcmp(tftMsgApp.substring(0,8).c_str(), "Telegram") == 0) drawBmpR("/icons/telegram.bmp",70,70,100,100);
      else if (strcmp(tftMsgApp.substring(0,8).c_str(), "Messages") == 0) drawBmpR("/icons/sms.bmp",70,70,100,100);
      else if (strcmp(tftMsgApp.substring(0,4).c_str(), "Flow") == 0) drawBmpR("/icons/calendar.bmp",70,70,100,100);
      else drawBmpR("/icons/alarm.bmp",70,70,100,100);
      
      // Write message
      tftS->setTextColor(CYAN);
      tftS->setFont(FONT_MEDIUM2);
      tftS->setCursor(1,50);
      tftS->print(tftMsgTitle.substring(0,30));
      tftS->setTextColor(WHITE);
      tftS->setFont(FONT_MEDIUM2);
      tftS->setCursor(1,140);
      //tftS->print(tftMsgBody.substring(0,36));
      tftS->print(tftMsgBody);
      
      bikeNotif = STATUS_WARN;
      
    } else if (bikeNotif == STATUS_WARN) {
      if (timerMsg < millis()){       // END NOTIFICATION
        bikeNotif = STATUS_OK;

        tftS->fillScreen(BLACK);
        bitSet(bikeDataChanged, 2);
        bitSet(bikeDataChanged, 5);
        bitSet(bikeDataChanged, 6);
        bitSet(bikeDataChanged, 7);
        bitSet(bikeDataChanged, 9);
        bitSet(bikeDataChanged, 12);
        bitSet(bikeDataChanged, 13);
        bitSet(bikeDataChanged, 14);
        bitSet(bikeDataChanged, 15);
        sendDisplay(); // Show again
      }
      
    } else {    // NO PHONE MESSAGES -> SHOW WALL DISTANCE OR OTHERS
      if (bitRead(bikeDataChanged,12) || bitRead(bikeDataChanged,15)){   // SPEED LIMIT or DISTANCE
        DEBUG_TFT_PRINTLN("SPEED LIMIT OR DISTANCE CHG");
        tftR->fillRect(70,70,100,100,BLACK);  // Clear image
        switch(bikeDistance){                 // Round signal
              case 5: tftR->fillArc(120,120,49,40,0,360,RED);     break;  // Red
              case 4: tftR->fillArc(120,120,49,40,0,360,YELLOW);  break;  // Yellow
              case 3: tftR->fillArc(120,120,49,40,0,360,GREEN);   break;  // Green
              case 2: tftR->fillArc(120,120,49,40,0,360,CYAN);    break;  // Cyan
              default: tftR->fillArc(120,120,49,40,0,360,WHITE);  break;  // White
        }
        tftR->setTextColor(WHITE);
        tftR->setFont(FONT_SIGNAL);
        
        if (mapsSpeedAlert) {
          tftR->fillCircle(120,120,40,YELLOW);
          tftR->setTextColor(BLACK);
        }

        if (mapsSpeed > 0) {
          if (mapsSpeed < 100) tftR->setCursor(100,135);
          else tftR->setCursor(85,135);
          tftR->print(mapsSpeed);
        }        
      }
      
      if (bitRead(bikeDataChanged,6)){    // TIME
        DEBUG_TFT_PRINTLN("HOUR CHG");
        tftS->fillRect(120,0,120,210,BLACK);
        String thora=String(bikeHour);
        String tminuto=String(bikeMinute);
        if(bikeHour < 10) thora="0"+thora;
        if(bikeMinute < 10) tminuto="0"+tminuto;
        
        tftS->setFont(FONT_BIG);
        tftS->setTextColor(YELLOW);
        tftS->setCursor(120,95);
        tftS->print(thora);
        tftS->setTextColor(CYAN);
        tftS->setCursor(120,200);
        tftS->print(tminuto);
      }
      
      if (bitRead(bikeDataChanged,14)){   // WEATHER LOCATION
        tftS->fillRect(1,210,240,35,BLACK);
        tftS->setFont(FONT_MEDIUM2);
        tftS->setTextColor(WHITE);
        tftS->setCursor(1,235);
        //Center text on screen
        int i=0;
        int l = (WEATHER_TXT_SIZE - weatherLocation.substring(0,WEATHER_TXT_SIZE).length()) /2;
        for (i=0; l>i; i++) 
          tftS->print(" ");
        tftS->print(weatherLocation.substring(0,WEATHER_TXT_SIZE));
      }

      if (bitRead(bikeDataChanged,2)){  // GPRS STATUS
        DEBUG_TFT_PRINTLN("GPRS CHG");
        //drawBmpS("/icons/hill.bmp",13,248,32,32);
        if (bikeGPRS == STATUS_OK) drawBmpS("/icons/gprsOk.bmp",13,248,32,32);
        else if(bikeGPRS == STATUS_WARN) drawBmpS("/icons/gprsWarn.bmp",13,248,32,32);
        else drawBmpS("/icons/gprsCrit.bmp",13,248,32,32);
      }

      if (bitRead(bikeDataChanged,5)){  // GPS HEIGHT
        DEBUG_TFT_PRINTLN("HEIGHT CHG");
        tftS->fillRect(1,245,160,3,BLACK);  // Space between img and city text
        tftS->fillRect(45,245,109,35,BLACK);
        tftS->setFont(FONT_MEDIUM2);
        tftS->setTextColor(WHITE);
        tftS->setCursor(51,278);
        tftS->print(bikeHeigh);
        tftS->print("m");
      }
    
      if (bitRead(bikeDataChanged,9)){    // GPS STATUS
        DEBUG_TFT_PRINTLN("GPS CHG");
        tftS->fillRect(208,245,32,3,BLACK); // Space between gps img and city text
        if (bikeGPS == STATUS_OK) drawBmpS("/icons/gpsOk.bmp",193,248,32,32);
        else if(bikeGPS == STATUS_WARN) drawBmpS("/icons/gpsWarn.bmp",193,248,32,32);
        else drawBmpS("/icons/gpsCrit.bmp",193,248,32,32);
      }
  
      if (bitRead(bikeDataChanged,13)){ // GPS SATELLITES
        DEBUG_TFT_PRINTLN("GPS SAT CHG");
        tftS->fillRect(155,245,38,35,BLACK);
        
        tftS->setFont(FONT_MEDIUM2);
        tftS->setTextColor(WHITE);
        tftS->setCursor(155,278);
        if(bikeSatellites < 10) tftS->print(" ");
        tftS->print(bikeSatellites);
      }

      /*if(bikeDistance != STATUS_OK){     // WALL DETECTED
        if(bitRead(bikeDataChanged,15)){
          DEBUG_TFT_PRINTLN("DIST CHG");
          tftS->fillRect(1,1,120,210,BLACK);
          drawBmpS("/icons/moto.bmp",5,10,110,75);
  
          switch(bikeDistance){
              case 5: tftS->fillRoundRect(90,120,20,80,5,RED);    // Red
              case 4: tftS->fillRoundRect(60,140,20,60,5,YELLOW); // Yellow
              case 3: tftS->fillRoundRect(30,160,20,40,5,GREEN);  // Green2
              case 2: tftS->fillRoundRect(5,180,20,20,5,GREEN);   // Green1
              case 1: break;
          }
        }
      } else { */// SHOW OTHERS
        if (bitRead(bikeDataChanged,7)){  // DATE
          DEBUG_TFT_PRINTLN("DATE CHG");
          tftS->fillRect(1,1,120,110,BLACK);
          tftS->setFont(FONT_MEDIUM2);
          tftS->setTextColor(WHITE);
          tftS->setCursor(1,45);
          tftS->print(get_dow());
          tftS->setCursor(1,85);
          tftS->print(bikeDay); tftS->print(" "); tftS->print(get_month());
        }
        
        if (bitRead(bikeDataChanged,14)){ // WEATHER ICON
          tftS->fillRect(1,110,120,100,BLACK); // Clear image
          printWeatherIcon(weatherIcon);
        }        
      //} //-- else SHOW OTHERS
    } // -- else if (bikeNotif == STATUS_WARN) 
    
    bikeDataChanged = 0;
  } // -- if (bikeDataChanged !=0...
}

void printWeatherIcon(int id) {
  int x=5;
  int y=100;

 switch(id){
  case 800: drawBmpS("/weather/clear.bmp",x,y,100,100); break;
  case 801: 
  case 802: drawBmpS("/weather/fewCloud.bmp",x,y,100,100); break;
  case 803: 
  case 804: drawBmpS("/weather/cloud.bmp",x,y,100,100); break;
  
  case 200: 
  case 201: 
  case 202: 
  case 210: 
  case 211: 
  case 212: 
  case 221: 
  case 230: 
  case 231: 
  case 232: drawBmpS("/weather/thunderstorm.bmp",x,y,100,100); break;

  case 300: 
  case 301: 
  case 302: 
  case 310: 
  case 311: 
  case 312: 
  case 313: 
  case 314: 
  case 321: drawBmpS("/weather/lightRain.bmp",x,y,100,100); break;

  case 500: 
  case 501: 
  case 502: 
  case 503: 
  case 504: 
  case 511: drawBmpS("/weather/lightRain.bmp",x,y,100,100); break;
  case 520: 
  case 521: drawBmpS("/weather/moderateRain.bmp",x,y,100,100); break;
  case 522: 
  case 531: drawBmpS("/weather/heavyRain.bmp",x,y,100,100); break;

  case 600: drawBmpS("/weather/lightSnow.bmp",x,y,100,100); break;
  case 601: 
  case 602: drawBmpS("/weather/snow.bmp",x,y,100,100); break;
  case 611: 
  case 612: 
  case 615: 
  case 616: 
  case 620: drawBmpR("/weather/lightSnow.bmp",x,y,100,100); break;
  case 621: 
  case 622: drawBmpR("/weather/snow.bmp",x,y,100,100); break;

  case 701: 
  case 711: 
  case 721: 
  case 731: 
  case 741: 
  case 751: 
  case 761: 
  case 762: 
  case 771: 
  case 781: drawBmpS("/weather/fog.bmp",x,y,100,100); break;

  default:break; // Draws nothing
 }
}

String get_dow(){   //Return day of week (short)
  switch(weekday()){
    case 1: return("Sun");break;
    case 2: return("Mon");break;
    case 3: return("Tue");break;
    case 4: return("Wed");break;
    case 5: return("Thu");break;
    case 6: return("Fri");break;
    case 7: return("Sat");break;
    default: return("");
  }
}

String get_month(){   //Return month (sort)
  switch(bikeMonth){
    case 1: return("Jan");break;
    case 2: return("Feb");break;
    case 3: return("Mar");break;
    case 4: return("Apr");break;
    case 5: return("May");break;
    case 6: return("Jun");break;
    case 7: return("Jul");break;
    case 8: return("Ago");break;
    case 9: return("Sep");break;
    case 10: return("Oct");break;
    case 11: return("Nov");break;
    case 12: return("Dec");break;
    default: return("");
  }
}

static void bmpDrawCallbackS(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h){
  tftS->draw16bitRGBBitmap(x, y, bitmap, w, h);
}

static void bmpDrawCallbackR(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h){
  tftR->draw16bitRGBBitmap(x, y, bitmap, w, h);
}

void drawBmpS(const char *filename, int16_t x, int16_t y, int16_t xx, int16_t yy) {
  fs::File bmpFS;
  // Open requested file on SPIFFS
  bmpFS = SPIFFS.open(filename, "r");
  if (!bmpFS){ 
    DEBUG_TFT_PRINTLN("File not found"); 
    return;
  }
  
  bmpClass.draw(&bmpFS, bmpDrawCallbackS, false /* useBigEndian */,x,y,xx,yy);
  bmpFS.close();
}

void drawBmpR(const char *filename, int16_t x, int16_t y, int16_t xx, int16_t yy) {
  fs::File bmpFS;
  // Open requested file on SPIFFS
  bmpFS = SPIFFS.open(filename, "r");
  if (!bmpFS){ 
    DEBUG_TFT_PRINTLN("File not found"); 
    return;
  }
  
  bmpClass.draw(&bmpFS, bmpDrawCallbackR, false /* useBigEndian */,x,y,xx,yy);
  bmpFS.close();
}