#include "weather.h"
#include "my_def.h"
#include "my_debug.h"

void getWeather(){
  if (!gsm.waitForNetwork()) {
    DEBUG_GPRS_PRINTLN("GPRS-W Network NOT ready");
    if (bikeGPRS != STATUS_CRIT) bitSet(bikeDataChanged,2);
    bikeGPRS = STATUS_CRIT;
  } else if (!gsm.gprsConnect(gprs_apn, gprs_user, gprs_pass)){
    DEBUG_GPRS_PRINTLN("GPRS-W Network NOT connected");
    if (bikeGPRS != STATUS_CRIT) bitSet(bikeDataChanged,2);
    bikeGPRS = STATUS_CRIT;
  } else {
    String http_url;

    // Without GPS data failback to default city
    if (bikeGPS == STATUS_OK) http_url = "/data/2.5/forecast?lat="+String(bikeLatitude,4)+"&lon="+String(bikeLongitude,4)+"&units=metric&lang=es&cnt=1&appid="+weather_apikey;
    else http_url = "/data/2.5/forecast?id="+weather_cityId+"&units=metric&lang=es&cnt=1&appid="+weather_apikey;
    DEBUG_GPRS_PRINT("Weather URL: ");
    DEBUG_GPRS_PRINTLN(http_url);
    
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
  }
}
