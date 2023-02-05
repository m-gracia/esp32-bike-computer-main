#include "httpsrv.h"
#include "my_def.h"
#include "my_debug.h"
#include <Arduino.h>
//#include <Update.h>

String webProcessor(const String& var){
  /*if(var == "STATE"){
    if (ledState){
      return "ON";
    }
    else{
      return "OFF";
    }
  }*/
  return String();
}

void initWeb(){
    // Turn on wifi AP. Default IP: 192.168.4.1
    WiFi.mode(WIFI_AP);
    WiFi.softAP(wifi_ssid,wifi_passwd);
    DEBUG_WEB_PRINTLN("WEB AP online");
    DEBUG_WEB_PRINT("WEB IP address: ");
    DEBUG_WEB_PRINTLN(WiFi.softAPIP());

    // Route for root / web page
    webSrv.on("/", HTTP_GET, []() {
        webSrv.sendHeader("Connection", "close");
        //request->send(SPIFFS, "/index.html", String(), false, web_processor);
        //webServer.send(200, "text/html", loginIndex);
        webSrv.send(200, "text/html", "Hello world");
        /*File f = SPIFFS.open("/index.html","r");
        webserver.streamFile(f,"text/html");
        f.close();*/
    });

    /*webSrv.on("/jquery.min.js", HTTP_GET, []() {
        webSrv.setContentLength(jquery_min_js_v3_2_1_gz_len);
		webSrv.sendHeader(F("Content-Encoding"), F("gzip"));
        webSrv.send_P(200, "text/javascript", jquery_min_js_v3_2_1_gz, jquery_min_js_v3_2_1_gz_len);
    });

    webSrv.on("/firmware", HTTP_GET, []() {
        webSrv.sendHeader("Connection", "close");
        //request->send(SPIFFS, "/update.html", String(), false, web_processor);
        webSrv.send(200, "text/html", serverIndex);
        //File f = SPIFFS.open("/update.html","r");
        //webSrv.streamFile(f,"text/html");
        //f.close();
    });

    /*webSrv.on("/update", HTTP_POST, []() {
        webSrv.sendHeader("Connection", "close");
        webSrv.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        ESP.restart();
    }, []() {
        HTTPUpload& upload = webSrv.upload();
        if (upload.status == UPLOAD_FILE_START) {
        DEBUG_WEB_PRINT("Update: ");
        DEBUG_WEB_PRINTLN(upload.filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
            DEBUG_WEB_PRINTLN("Update error size");
        }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
        // flashing firmware to ESP
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            DEBUG_WEB_PRINTLN("Update error write");
        }
        } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { //true to set the size to the current progress
            DEBUG_WEB_PRINT("Update Success, rebooting: ");
            DEBUG_WEB_PRINTLN(upload.totalSize);
        } else {
            DEBUG_WEB_PRINTLN("Update error failed");
        }
        }
    });*/

    // Start web server and OTA
    //updateWebServer.setup(&webSrv);
    webSrv.begin();
    DEBUG_WEB_PRINTLN("WEB Server online");
}

void getWeb(){
    webSrv.handleClient();
    //DEBUG_WEB_PRINTLN(ESP.getFreeHeap());
}