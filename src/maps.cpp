#include "maps.h"
#include "my_def.h"
#include "my_debug.h"

void getMaps(){
  if (!gsm.waitForNetwork()) {
    DEBUG_GPRS_PRINTLN("GPRS-M Network NOT ready");
    if (bikeGPRS != STATUS_CRIT) bitSet(bikeDataChanged,2);
    bikeGPRS = STATUS_CRIT;
  } else if (!gsm.gprsConnect(gprs_apn, gprs_user, gprs_pass)){
    DEBUG_GPRS_PRINTLN("GPRS-M Network NOT connected");
    if (bikeGPRS != STATUS_CRIT) bitSet(bikeDataChanged,2);
    bikeGPRS = STATUS_CRIT;
  } else {  // GPRS Ready

    if (bikeGPS == STATUS_OK){  // GPS data available
      String http_url;
      http_url = "/REST/v1/Routes/SnapToRoad?pts="+String(bikeLatitude,4)+","+String(bikeLongitude,4)+"&intpl=false&spdl=true&spu=KPH&key="+maps_apikey;
      DEBUG_GPRS_PRINT("Maps URL: ");
      DEBUG_GPRS_PRINTLN(http_url);


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
        // TODO - Use an array and a loop insted (more "elegant" way)
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
  }
}
