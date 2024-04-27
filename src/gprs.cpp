#include "gprs.h"
#include "my_def.h"
#include "my_debug.h"


void initWifi(){
  DEBUG_GPRS_PRINTLN("Wifi Init start");
  wifi_dev.addAP(wifi_ssid01,wifi_passwd01);
  wifi_dev.addAP(wifi_ssid02,wifi_passwd02);
  wifi_dev.run();

  bikeGPRS = STATUS_CRIT; // Initialized but without data
  DEBUG_GPRS_PRINTLN("Wifi Init end");
}

void getWeather(){
  WiFiClient wifi_client;
  String http_url;

  // Without GPS data failback to default city
  if (bikeGPS == STATUS_OK /*|| bikeGPS == STATUS_WARN*/) http_url = "/data/2.5/forecast?lat="+String(bikeLatitude,4)+"&lon="+String(bikeLongitude,4)+"&units=metric&lang=es&cnt=1&appid="+weather_apikey;
  else http_url = "/data/2.5/forecast?id="+weather_cityId+"&units=metric&lang=es&cnt=1&appid="+weather_apikey;
  DEBUG_GPRS_PRINT("Weather URL: ");
  DEBUG_WEB_SECRETPRINTLN(http_url);

  HttpClient weather_client(wifi_client, weather_server, weather_port);
  
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

    JsonDocument root;
    if (deserializeJson(root, result)) { 
      DEBUG_GPRS_PRINTLN("parseObject() failed");
      return;
    }

    weatherLocation = root["city"]["name"].as<String>();
    weatherIcon = root["list"][0]["weather"][0]["id"].as<int>();
    //weatherTemperature = root["list"][0]["main"]["temp"].as<const char*>();
    //weatherWeather = root["list"][0]["weather"][0]["main"].as<const char*>();
    //weatherDescription = root["list"][0]["weather"][0]["description"].as<const char*>();
    //weatherWind = root["list"][0]["wind"]["speed"].as<float>();

    DEBUG_GPRS_PRINT("GPRS-W Icon:");
    DEBUG_GPRS_PRINTLN(weatherIcon);
    bitSet(bikeDataChanged,14);
    bitSet(bikeDataChanged,15);  // Weather
  }
  weather_client.stop();
  DEBUG_GPRS_PRINTLN("GPRS-W Disconnected");
}

void getMaps(){
  if (bikeGPS == STATUS_OK || bikeGPS == STATUS_WARN){  // GPS data available
    WiFiClient wifi_client;
    String http_url;
    http_url = "/REST/v1/Routes/SnapToRoad?pts="+String(bikeLatitude,4)+","+String(bikeLongitude,4)+"&intpl=false&spdl=true&spu=KPH&key="+maps_apikey;
    DEBUG_GPRS_PRINT("Maps URL: ");
    DEBUG_WEB_SECRETPRINTLN(http_url);

    HttpClient maps_client(wifi_client, maps_server, maps_port);

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

      JsonDocument root;
      if (deserializeJson(root, result)) { 
        DEBUG_GPRS_PRINTLN("parseObject() failed");
        return;
      }

      String tmapsStreet = root["resourceSets"][0]["resources"][0]["snappedPoints"][0]["name"].as<const char*>();
      int tmapsSpeed = root["resourceSets"][0]["resources"][0]["snappedPoints"][0]["speedLimit"].as<int>();

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
      if(tmapsStreet.length() > 4 && tmapsStreet.startsWith(String("del "))){
        tmapsStreet = tmapsStreet.substring(4);
      }else if(tmapsStreet.length() > 6 && tmapsStreet.startsWith(String("de la "))){
        tmapsStreet = tmapsStreet.substring(6);
      }else if(tmapsStreet.length() > 7 && tmapsStreet.startsWith(String("de las "))){
        tmapsStreet = tmapsStreet.substring(7);
      }else if(tmapsStreet.length() > 7 && tmapsStreet.startsWith(String("de los "))){
        tmapsStreet = tmapsStreet.substring(7);
      }else if (tmapsStreet.length() > 3 && tmapsStreet.startsWith(String("de "))){
        tmapsStreet = tmapsStreet.substring(3);
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
}

void sendLocation(){
  unsigned char gprsEnvio;
  // Send poweron flag the first time, otherwise send if the phone is connected.
  if(firstGPRS) gprsEnvio = STATUS_POWERON;
  else gprsEnvio = bikeBT;
  
  WiFiClient wifi_client;
  String http_url;
    http_url = String(http_path) + "/p.php?u=0&p=" + String(web_passw) + "&s=" + String(gprsEnvio);
    http_url = http_url + "&a=" + String(bikeHeigh);
    http_url = http_url + "&la=" + String(bikeLatitude,8);
    http_url = http_url + "&l=" + String(bikeLongitude,8);
    DEBUG_GPRS_PRINT("Web URL: ");
    DEBUG_WEB_SECRETPRINTLN(http_url);

  HttpClient http_client(wifi_client, http_server, http_port);

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
  uint8_t wifi_status;
  
  DEBUG_GPRS_PRINTLN("WIFI Testing network");
  wifi_status = wifi_dev.run();

  if (wifi_status != WL_CONNECTED) {
    DEBUG_GPRS_PRINTLN("WIFI Network NOT ready");
    DEBUG_GPRS_PRINT("WIFI Status: "); DEBUG_GPRS_PRINTLN(wifi_status);
    if (wifi_status = WL_NO_SSID_AVAIL) WiFi.disconnect(); // To fix a bug on reconnection
    if (bikeGPRS != STATUS_CRIT) bitSet(bikeDataChanged,2);
    bikeGPRS = STATUS_CRIT;
    timerGPRS = millis() + 10000;    // Recheck in 10 sec
    
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
      timerMaps = millis() + 20000; // 20 sec
    }
  }
}
