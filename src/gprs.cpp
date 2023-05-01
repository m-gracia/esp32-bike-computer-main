#include "gprs.h"
#include "my_def.h"
#include "my_debug.h"


void initGPRS(){
  DEBUG_GPRS_PRINTLN("GPRS Init start");

  gsmDev.begin(9600,SERIAL_8N1,GPRS_PIN_RX,GPRS_PIN_TX,false); // Try at default poweron baudrate
  if (!gsm.init()){
    DEBUG_GPRS_PRINTLN("GPRS Init fail at 9600");
    gsmDev.begin(115200,SERIAL_8N1,GPRS_PIN_RX,GPRS_PIN_TX,false);  //Try faster
    if (!gsm.init()) DEBUG_GPRS_PRINTLN("GPRS Init fail also at 115200");
  }
  else{
    gsm.setBaud(115200);
    gsmDev.end();
    gsmDev.begin(115200,SERIAL_8N1,GPRS_PIN_RX,GPRS_PIN_TX,false);
  }
  
  if (!gsm.enableGPS()){ 
    DEBUG_GPS_PRINTLN("GPS Init fail");
    DEBUG_GPS_PRINT("GPS ERROR: ");
    DEBUG_GPS_PRINTLN(gsm.getGPSraw());
    delay(200);
    if (!gsm.enableGPS()){
      DEBUG_GPS_PRINTLN("GPS Init fail2");
      DEBUG_GPS_PRINT("GPS ERROR: ");
      DEBUG_GPS_PRINTLN(gsm.getGPSraw());
    }
  }
  
  bikeGPRS = STATUS_CRIT; // Initialized but without data
  bikeGPS = STATUS_CRIT; // Initialized but without data
  DEBUG_GPRS_PRINTLN("GPRS Init end");
  DEBUG_GPS_PRINTLN("GPS Init end");
  //DEBUG_GPRS_PRINT("SIM Status: "); DEBUG_GPRS_PRINTLN(gsm.getSimStatus());
  //DEBUG_GPRS_PRINT("MODEM Connected: "); DEBUG_GPRS_PRINTLN(gsm.isNetworkConnected());
  //DEBUG_GPRS_PRINT("Signal Quality: "); DEBUG_GPRS_PRINTLN(gsm.getSignalQuality());
  //DEBUG_GPRS_PRINT("Local IP: "); DEBUG_GPRS_PRINTLN(gsm.getLocalIP());
}


void getWeather(){
  /*if (!gsm.waitForNetwork(2000L,true)) {
    DEBUG_GPRS_PRINTLN("GPRS-W Network NOT ready");
    if (bikeGPRS != STATUS_CRIT) bitSet(bikeDataChanged,2);
    bikeGPRS = STATUS_CRIT;
  } else if (!gsm.gprsConnect(gprs_apn, gprs_user, gprs_pass)){
    DEBUG_GPRS_PRINTLN("GPRS-W Network NOT connected");
    if (bikeGPRS != STATUS_CRIT) bitSet(bikeDataChanged,2);
    bikeGPRS = STATUS_CRIT;
  } else {*/
    String http_url;

    // Without GPS data failback to default city
    if (bikeGPS == STATUS_OK || bikeGPS == STATUS_WARN) http_url = "/data/2.5/forecast?lat="+String(bikeLatitude,4)+"&lon="+String(bikeLongitude,4)+"&units=metric&lang=es&cnt=1&appid="+weather_apikey;
    else http_url = "/data/2.5/forecast?id="+weather_cityId+"&units=metric&lang=es&cnt=1&appid="+weather_apikey;
    DEBUG_GPRS_PRINT("Weather URL: ");
    DEBUG_GPRS_PRINTLN(http_url);

    HttpClient weather_client(gsm_client, weather_server, weather_port);
    
    if(weather_client.get(http_url) != 0){
      DEBUG_GPRS_PRINTLN("GPRS-W fail getting data");
      if (bikeGPRS != STATUS_WARN) bitSet(bikeDataChanged,2);
      bikeGPRS = STATUS_WARN;
    } else {
      DEBUG_GPRS_PRINTLN("GPRS-W connected OK");
      if (bikeGPRS != STATUS_OK) bitSet(bikeDataChanged,2);
      bikeGPRS = STATUS_OK;
      
      String result = weather_client.responseBody();
      DEBUG_GPRS_PRINTLN("GPRS-W Readed:");
      DEBUG_GPRS_PRINTLN(result);
      result.replace('[', ' ');
      result.replace(']', ' ');
      
      char jsonArray [result.length()+1];
      result.toCharArray(jsonArray,sizeof(jsonArray));
      jsonArray[result.length() + 1] = '\0';
      
      StaticJsonBuffer<1024> json_buf;
      JsonObject &root = json_buf.parseObject(jsonArray);
      if (!root.success()) { 
        DEBUG_GPRS_PRINTLN("parseObject() failed");
        return;
      }

      weatherLocation = (const char*)root["city"]["name"];
      String idString = (const char*)root["list"]["weather"]["id"];
      weatherIcon = idString.toInt();
      //weatherTemperature = (const char*)root["list"]["main"]["temp"];
      //weatherWeather = (const char*)root["list"]["weather"]["main"];
      //weatherDescription = (const char*)root["list"]["weather"]["description"];
      //String windspeed = (const char*)root["list"]["wind"]["speed"];
      //weatherWind = windspeed.toFloat();

      DEBUG_GPRS_PRINT("GPRS-W Icon:");
      DEBUG_GPRS_PRINTLN(weatherIcon);
      bitSet(bikeDataChanged,14);
      bitSet(bikeDataChanged,15);  // Weather
    }
    weather_client.stop();
    DEBUG_GPRS_PRINTLN("GPRS-W Disconnected");
  //}
}

void getMaps(){
  /*if (!gsm.waitForNetwork(2000L,true)) {
    DEBUG_GPRS_PRINTLN("GPRS-M Network NOT ready");
    if (bikeGPRS != STATUS_CRIT) bitSet(bikeDataChanged,2);
    bikeGPRS = STATUS_CRIT;
  } else if (!gsm.gprsConnect(gprs_apn, gprs_user, gprs_pass)){
    DEBUG_GPRS_PRINTLN("GPRS-M Network NOT connected");
    if (bikeGPRS != STATUS_CRIT) bitSet(bikeDataChanged,2);
    bikeGPRS = STATUS_CRIT;
  } else {  // GPRS Ready*/

    if (bikeGPS == STATUS_OK || bikeGPS == STATUS_WARN){  // GPS data available
      String http_url;
      http_url = "/REST/v1/Routes/SnapToRoad?pts="+String(bikeLatitude,4)+","+String(bikeLongitude,4)+"&intpl=false&spdl=true&spu=KPH&key="+maps_apikey;
      DEBUG_GPRS_PRINT("Maps URL: ");
      DEBUG_GPRS_PRINTLN(http_url);

      HttpClient maps_client(gsm_client, maps_server, maps_port);

      if( maps_client.get(http_url) != 0){
        DEBUG_GPRS_PRINTLN("GPRS-M fail getting data");
        if (bikeGPRS != STATUS_WARN) bitSet(bikeDataChanged,2);
        bikeGPRS = STATUS_WARN;
      } else {
        DEBUG_GPRS_PRINTLN("GPRS-M connected OK");
        if (bikeGPRS != STATUS_OK) bitSet(bikeDataChanged,2);
        bikeGPRS = STATUS_OK;
        
        String result = maps_client.responseBody();
        DEBUG_GPRS_PRINTLN("GPRS-M Readed:");
        DEBUG_GPRS_PRINTLN(result);
        result.replace('[', ' ');
        result.replace(']', ' ');
        
        char jsonArray [result.length()+1];
        result.toCharArray(jsonArray,sizeof(jsonArray));
        jsonArray[result.length() + 1] = '\0';
        
        StaticJsonBuffer<1536> json_buf;
        JsonObject &root = json_buf.parseObject(jsonArray);
        if (!root.success()) { 
          DEBUG_GPRS_PRINTLN("parseObject() failed");
          return;
        }
  
        String tmapsStreet = (const char*)root["resourceSets"]["resources"]["snappedPoints"]["name"];
        String StrSpeed = (const char*)root["resourceSets"]["resources"]["snappedPoints"]["speedLimit"];
        int tmapsSpeed = StrSpeed.toInt();

        // Clean the street name
        // TODO - Use an array and a loop instead (more "elegant" way)
        if(tmapsStreet.length() > 6 && tmapsStreet.startsWith(String("Calle "))){
          tmapsStreet = tmapsStreet.substring(6);
        }else if(tmapsStreet.length() > 8 && tmapsStreet.startsWith(String("Avenida "))){
          tmapsStreet = tmapsStreet.substring(8);
        }else if(tmapsStreet.length() > 6 && tmapsStreet.startsWith(String("Paseo "))){
          tmapsStreet = tmapsStreet.substring(6);
        }else if(tmapsStreet.length() > 7 && tmapsStreet.startsWith(String("Camino "))){
          tmapsStreet = tmapsStreet.substring(7);
        }else if(tmapsStreet.length() > 10 && tmapsStreet.startsWith(String("Carretera "))){
          tmapsStreet = tmapsStreet.substring(10);
        }else if(tmapsStreet.length() > 6 && tmapsStreet.startsWith(String("Ronda "))){
          tmapsStreet = tmapsStreet.substring(6);
        }else if(tmapsStreet.length() > 9 && tmapsStreet.startsWith(String("Travesía "))){
          tmapsStreet = tmapsStreet.substring(9);
        }else if(tmapsStreet.length() > 8 && tmapsStreet.startsWith(String("Autovía "))){
          tmapsStreet = tmapsStreet.substring(8);
        }else if(tmapsStreet.length() > 10 && tmapsStreet.startsWith(String("Autopista "))){
          tmapsStreet = tmapsStreet.substring(10);
        }else if(tmapsStreet.length() > 4 && tmapsStreet.startsWith(String("Vía "))){
          tmapsStreet = tmapsStreet.substring(4);
        }
        if (tmapsStreet.length() > 3 && tmapsStreet.startsWith(String("de "))){
          tmapsStreet = tmapsStreet.substring(3);
        }else if(tmapsStreet.length() > 4 && tmapsStreet.startsWith(String("del "))){
          tmapsStreet = tmapsStreet.substring(4);
        }else if(tmapsStreet.length() > 6 && tmapsStreet.startsWith(String("de la "))){
          tmapsStreet = tmapsStreet.substring(6);
        }else if(tmapsStreet.length() > 7 && tmapsStreet.startsWith(String("de las "))){
          tmapsStreet = tmapsStreet.substring(7);
        }else if(tmapsStreet.length() > 7 && tmapsStreet.startsWith(String("de los "))){
          tmapsStreet = tmapsStreet.substring(7);
        }
  
        DEBUG_GPRS_PRINT("GPRS-M Street:");
        DEBUG_GPRS_PRINTLN(tmapsStreet);
        DEBUG_GPRS_PRINT("GPRS-M Speed:");
        DEBUG_GPRS_PRINTLN(tmapsSpeed);

        if (tmapsStreet != mapsStreet){
          mapsStreet = tmapsStreet;
          bitSet(bikeDataChanged,11);
        }

        if (tmapsSpeed != mapsSpeed){
          mapsSpeed = tmapsSpeed;
          bitSet(bikeDataChanged,12);
        }
      }
      
      maps_client.stop();
      DEBUG_GPRS_PRINTLN("GPRS-M Disconnected");

    } else {
      DEBUG_GPRS_PRINTLN("GPRS-M No GPS data");
    }
  //}
}

void sendLocation(){
  unsigned char gprsEnvio;
  // Send poweron flag the first time, otherwise send if the phone is connected.
  if(firstGPRS) gprsEnvio = STATUS_POWERON;
  else gprsEnvio = bikeBT;
  
  String http_url;
    http_url = String(http_path) + "/p.php?u=0&p=" + String(web_passw) + "&s=" + String(gprsEnvio);
    http_url = http_url + "&a=" + String(bikeHeigh);
    http_url = http_url + "&la=" + String(bikeLatitude,8);
    http_url = http_url + "&l=" + String(bikeLongitude,8);
    DEBUG_GPRS_PRINT("Web URL: ");
    DEBUG_GPRS_PRINTLN(http_url);

  HttpClient http_client(gsm_client, http_server, http_port);

  if(http_client.get(http_url) != 0){
    DEBUG_GPRS_PRINTLN("GPRS fail get http");
    if (bikeGPRS != STATUS_WARN) bitSet(bikeDataChanged,2);
    bikeGPRS = STATUS_WARN;
  } else {
    DEBUG_GPRS_PRINTLN("GPRS http connected OK");
    // Change the status on success
    if(firstGPRS) firstGPRS = false;
    if(bikeGPRS != STATUS_OK) bitSet(bikeDataChanged,2);
    bikeGPRS = STATUS_OK;
  }

  http_client.stop();
  DEBUG_GPRS_PRINTLN("GPRS http disconnected"); 
}

void useGPRS(){
  if (!gsm.waitForNetwork(2000L,true)) {
    DEBUG_GPRS_PRINTLN("GPRS Network NOT ready");
    //DEBUG_GPRS_PRINT("SIM Status: "); DEBUG_GPRS_PRINTLN(gsm.getSimStatus());
    //DEBUG_GPRS_PRINT("MODEM Connected: "); DEBUG_GPRS_PRINTLN(gsm.isNetworkConnected());
    //DEBUG_GPRS_PRINT("Signal Quality: "); DEBUG_GPRS_PRINTLN(gsm.getSignalQuality());
    //DEBUG_GPRS_PRINT("Local IP: "); DEBUG_GPRS_PRINTLN(gsm.getLocalIP());
    if (bikeGPRS != STATUS_CRIT) bitSet(bikeDataChanged,2);
    bikeGPRS = STATUS_CRIT;

  } else if (!gsm.isGprsConnected()){
    DEBUG_GPRS_PRINTLN("GPRS Network NOT connected");
    if (bikeGPRS != STATUS_CRIT) bitSet(bikeDataChanged,2);
    bikeGPRS = STATUS_CRIT;
    gsm.gprsConnect(gprs_apn, gprs_user, gprs_pass);

  } else {
    //Send location to server
    if (timerGPRS < millis()){
      sendLocation();
      if (bikeGPRS != STATUS_OK) timerGPRS = millis() + 10000;    // On error recheck in 10 sec
      else timerGPRS = millis() + 60000; // 1 min
    }

    //Check for Weather information
    if (timerWeather < millis()){
      getWeather();
      if (bikeGPRS != STATUS_OK) timerWeather = millis() + 10000;   // On error recheck in 10 sec
      else timerWeather = millis() + 600000;  // 10 min
    }

    //Check for Maps information
    if (timerMaps < millis()){
      getMaps();
      timerMaps = millis() + 10000; // 10 sec
    }
  }
}


/* getGPS()
 * Feed the GPS and get data
 */
void getGPS(){
  float tLat = 0;
  float tLon = 0;
  float tSpeed = 0; 
  float tAltitud = 0;
  int itAltitud = 0;
  int tSatellites = 0;
  bool fix = false;
  int tYear = 0;
  int tMonth = 0;
  int tDay = 0;
  int tHour = 0;
  int tMinute = 0;
  int tSecond = 0;
  int tbikeGPS = STATUS_OK;
  
  DEBUG_GPSRAW_PRINTLN(gsm.getGPSraw());
  
  //fix = gsm.getGPSTime(&tYear,&tMonth,&tDay,&tHour,&tMinute,&tSecond);
  fix = gsm.getGPS(&tLat,&tLon,&tSpeed,&tAltitud,&tSatellites,NULL,NULL,
    &tYear,&tMonth,&tDay,&tHour,&tMinute,NULL);
  DEBUG_GPS_PRINT(F("DAY "));
  DEBUG_GPS_PRINT(tDay);
  DEBUG_GPS_PRINT(F("/"));
  DEBUG_GPS_PRINT(tMonth);
  DEBUG_GPS_PRINT(F("/"));
  DEBUG_GPS_PRINTLN(tYear);
  DEBUG_GPS_PRINT(F("HOUR "));
  DEBUG_GPS_PRINT(tHour);
  DEBUG_GPS_PRINT(F(":"));
  DEBUG_GPS_PRINTLN(tMinute);
  DEBUG_GPS_PRINT(F("Speed "));
  DEBUG_GPS_PRINTLN(String(tSpeed,1));
  DEBUG_GPS_PRINT(F("Altitude "));
  DEBUG_GPS_PRINTLN(tAltitud);
  DEBUG_GPS_PRINT(F("Position "));
  DEBUG_GPS_PRINT(String(tLat,6));
  DEBUG_GPS_PRINT(F(" | "));
  DEBUG_GPS_PRINTLN(String(tLon,6));
  DEBUG_GPS_PRINT(F("Satellites "));
  DEBUG_GPS_PRINTLN(tSatellites);
  
  if (!fix) {
    tbikeGPS = STATUS_WARN;
    // Failover to less acurate GPRS location and time
    gsm.getGsmLocation(&tLat,&tLon,NULL,&tYear,&tMonth,&tDay,&tHour,&tMinute,NULL);
  }
  
  if (tYear > 2020 and tYear < 2120){    // If the GPS return a realistic date
    setTime(tHour,tMinute,tSecond,tDay,tMonth,tYear); // Update the internal clock
    if (fix) setTime(CE.toLocal(now())); // Change to our timezone only if taken from GPS (UTC)
    
    if (bikeMonth != month()) {
      bikeMonth = month();
      bitSet(bikeDataChanged,7);
    }
    if (bikeDay != day()) {
      bikeDay = day();
      bitSet(bikeDataChanged,7);
    }
    if (bikeHour != hour()) {
      bikeHour = hour();
      bitSet(bikeDataChanged,6);
    }
    if (bikeMinute != minute()) {
      bikeMinute = minute();
      bitSet(bikeDataChanged,6);
    }
  }
  
  bikeLatitude = tLat;
  bikeLongitude = tLon;

  if (tSpeed > 250) tSpeed = 0;   // To solve some parsing problems
  bikeSpeed = (int)tSpeed;

  // Check if we are going 10% faster than we should
  unsigned char tmapsSpeedAlert;
  if ((bikeSpeed * 1,1) > mapsSpeed) tmapsSpeedAlert = true;
  else tmapsSpeedAlert = false;
  if (tmapsSpeedAlert != mapsSpeedAlert){
    mapsSpeedAlert = tmapsSpeedAlert;
    bitSet(bikeDataChanged,12);
  }  

  if (tAltitud < 0 || tAltitud > 3000) tAltitud = 0;  // To solve some parsing problems
  if ( bikeHeigh != tAltitud) {
    bikeHeigh = tAltitud;
    bitSet(bikeDataChanged,5);      
  }

  if (tSatellites < 0 || tSatellites > 99) tSatellites = 0; // To solve some parsing problems
  if ( bikeSatellites != tSatellites) {
    bikeSatellites = tSatellites;
    bitSet(bikeDataChanged,13);
  }

  if (tbikeGPS != bikeGPS){
    bikeGPS = tbikeGPS;
    bitSet(bikeDataChanged,9);
  }
}
