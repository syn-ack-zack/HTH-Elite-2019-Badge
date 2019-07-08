#include <WiFi.h>
#include "FS.h"
#include "SPIFFS.h"
#include <SPI.h>
#include <HTTPClient.h>
//#include "esp_http_client.h"
//#include <ArduinoHttpClient.h>

#include <Adafruit_NeoPixel.h>
#include <WS2812FX.h>
#include <SSD_13XX.h>
#include <LoRa.h>
#include <ArduinoJson.h>
#include "core.h"
#include "lorareceiver.h"

extern Adafruit_NeoPixel pixels;
extern WS2812FX ws2812fx;
extern SSD_13XX mydisp;
extern volatile byte btnid;
extern byte appmode;
extern byte btncounter;

extern byte oledbright;
extern byte ledsbright;
extern byte region_id;
extern byte region_unlocked;
extern String ISSID;
extern String IPSWD;
extern int NTP_OFFSET;
extern String GOOGLEAUTH;

extern void SaveSettings();


/////////LORA///////
//SPI defs for LoRa radio
#define SS 5
#define RST 25
#define DI0 17
// #define BAND 429E6

// LoRa Settings
#define spreadingFactor 7
// #define SignalBandwidth 62.5E3
#define SignalBandwidth 125E3
#define codingRateDenominator 8
#define preambleLength 8



  
////////LORA////////
//////////////////////////////////////////////////////////////////////////////////

String APssid;
String APpassword;

void APDisplay()
{
  mydisp.clearScreen();
  mydisp.setCursor(0, 0);
  mydisp.setTextColor(WHITE);
  mydisp.setTextScale(1);
  mydisp.println(F("Connect to..."));
  mydisp.print(F("ssid: "));
  mydisp.println(APssid);
  mydisp.print(F("password: "));
  mydisp.println(APpassword);
}

void APStart() { 
  //APssid = "HW-1234";
  APssid = "HTH-";
  APssid += random(0,9);
  APssid += random(0,9);
  APssid += random(0,9);
  APssid += random(0,9);
  //APpassword = "badge1234";
  APpassword = "badge";
  APpassword += random(0,9);
  APpassword += random(0,9);
  APpassword += random(0,9);
  APpassword += random(0,9);
  APpassword += random(0,9);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IPAddress(10,0,5,1), IPAddress(10,0,5,1), IPAddress(255,255,255,0));
  WiFi.softAP(APssid.c_str(), APpassword.c_str(), 9, 0);
}

void APStop(){
  WiFi.softAPdisconnect(1);  
}
//////////////////////////////////////////////////////////////////////////////////

// webserver functions remix of https://github.com/spacehuhn/wifi_ducky

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <Update.h>
#include <FS.h>

#include "update.h"
#include "hidarsenal.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");



//websocket start
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->printf("SERVER MESSAGE: Link Established");
    client->ping();
  } else if(type == WS_EVT_DISCONNECT){
    Serial.printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
  } else if(type == WS_EVT_ERROR){
    Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if(type == WS_EVT_PONG){
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
  } else if(type == WS_EVT_DATA){
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    String msg = "";
    if(info->final && info->index == 0 && info->len == len){
      //the whole message is in a single frame and we got all of it's data
      Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);

      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < info->len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < info->len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n",msg.c_str());

      if(info->opcode == WS_TEXT)
        client->text("I got your text message");
      else
        client->binary("I got your binary message");
    } else {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if(info->index == 0){
        if(info->num == 0)
          Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
        Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }

      Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);

      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < info->len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < info->len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n",msg.c_str());

      if((info->index + len) == info->len){
        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if(info->final){
          Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
          if(info->message_opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");
        }
      }
    }
  }
}


//websocket stop

char *multi_tok(char *input, char *delimiter) {
    static char *string;
    if (input != NULL)
        string = input;

    if (string == NULL)
        return string;

    char *end = strstr(string, delimiter);
    if (end == NULL) {
        char *temp = string;
        string = NULL;
        return temp;
    }

    char *temp = string;

    *end = '\0';
    string = end + strlen(delimiter);
    return temp;
}

void msgCallback(int packetSize) {
  // received a packet
  Serial.print("Received packet '");
  String msg = "";
  // read packet
  for (int i = 0; i < packetSize; i++) {
    msg += (char)LoRa.read();
  }

  Serial.print(msg);
  msgReceived = true;
  storedMsg = msg;

}


void WebServerStart()
{
  
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  events.onConnect([](AsyncEventSourceClient *client){
    client->send("hello!",NULL,millis(),1000);
  });
  //server.addHandler(&events);

  server.addHandler(new SPIFFSEditor(SPIFFS,APssid.c_str(),APpassword.c_str()));

///LORA START

  SPIClass * vspi = NULL;
  
  vspi = new SPIClass(VSPI);
  LoRa.setSPI(*vspi);
  LoRa.setPins(5,25,17);
  //vspi->begin(18, 19, 23, 5);
  
  Serial.println(F("HTH Node"));
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.print("LoRa Spreading Factor: ");
  Serial.println(spreadingFactor);
  LoRa.setSpreadingFactor(spreadingFactor);

  Serial.print("LoRa Signal Bandwidth: ");
  Serial.println(SignalBandwidth);
  LoRa.setSignalBandwidth(SignalBandwidth);

  LoRa.setCodingRate4(codingRateDenominator);
  LoRa.setPreambleLength(preambleLength);
 
   // start listening for LoRa packets

  // register the receive callback
  LoRa.onReceive(msgCallback);

  // put the radio into receive mode
  LoRa.receive();
    
//LORA END

  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });

  server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request){
    if(request->hasArg("confirm")) {
      bool confirm = request->arg("confirm");
      if (confirm) 
      {
        request->send(200, "text/plain", "Updating...");
        updateFromFS(SPIFFS);  
      }
    }
    else {
      AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "");
      response->addHeader("Location", "/");
      request->send(response);
    }
  });
  
  server.on("/hthmsg", HTTP_POST, [](AsyncWebServerRequest *request) {
    if(request->hasArg("msg")) {
        delay(1000);
        String msg = request->arg("msg");
        LoRa.beginPacket();
        LoRa.print(msg);
        LoRa.endPacket();
        // put the radio into receive mode
        delay(1000);
        LoRa.receive();
        request->send(200, "text/plain", "true");
    }  
  });


  server.on("/runscript", HTTP_POST, [](AsyncWebServerRequest *request) {
    if(request->hasArg("name")) {
      String file = request->arg("name");
      if (file != "" and SPIFFS.exists(file)) {
        request->send(200, "text/plain", "true");
        runpayload(file);
      }
      else {
        request->send(200, "text/plain", "Nothing to run");
      }
    }
    else {
      request->send(200, "text/plain", "Nothing to run");
    }
  });

  server.on("/runtext", HTTP_POST, [](AsyncWebServerRequest *request) {
    if(request->hasArg("text")) {
      request->send(200, "text/plain", "Running");
      Serial.println(request->arg("text"));

      char input[1024];
      int defaultdelay = 2000;
      strcpy(input, request->arg("text").c_str());
      char *token = multi_tok(input, "@@@");
      while (token != NULL) {
        char *i;
        String line = token;
        String cmd;
        String cmdinput;
        cmd = String(strtok_r(token," ",&i));

        if(cmd == "REM") { } // nothing
        else if(cmd == "DEFAULTDELAY" || cmd == "DEFAULT_DELAY") {
          cmdinput = String(strtok_r(NULL," ",&i));
          String newdefaultdelay = cmdinput;
          defaultdelay = newdefaultdelay.toInt();
        }
        else if(cmd == "DELAY") {
          cmdinput = String(strtok_r(NULL," ",&i));
          String newdefaultdelay = cmdinput;
          int tempdelay = newdefaultdelay.toInt();
          delay(tempdelay);
        }
        else {
          Serial.println(line);
        }
        delay(defaultdelay); //delay between lines in payload, I found running it slower works best
        
        token = multi_tok(NULL, "@@@");
      }
    }
    else {
      request->send(200, "text/plain", "Nothing to run");
    }
  });

  //for spiffseditor
  server.serveStatic("/ace.js.gz", SPIFFS, "/www/ace.js.gz");
  server.serveStatic("/ext-searchbox.js.gz", SPIFFS, "/www/ext-searchbox.js.gz");
  server.serveStatic("/mode-css.js.gz", SPIFFS, "/www/mode-css.js.gz");
  server.serveStatic("/mode-html.js.gz", SPIFFS, "/www/mode-html.js.gz");
  server.serveStatic("/mode-javascript.js.gz", SPIFFS, "/www/mode-javascript.js.gz");
  server.serveStatic("/worker-html.js.gz", SPIFFS, "/www/worker-html.js.gz");

  server.serveStatic("/functions.js", SPIFFS, "/www/functions.js");
  server.serveStatic("/ui.js", SPIFFS, "/www/js/ui.js");
  server.serveStatic("/chat.js", SPIFFS, "/www/js/chat.js");
  server.serveStatic("/skeleton.min.css", SPIFFS, "/www/skeleton.min.css");
  server.serveStatic("/style.css", SPIFFS, "/www/style.css");
  server.serveStatic("/net.css", SPIFFS, "/www/net.css");
  server.serveStatic("/pure-min.css", SPIFFS, "/www/pure-min.css");
  server.serveStatic("/side-menu.css", SPIFFS, "/www/side-menu.css");
  server.serveStatic("/side-menu-old-ie.css", SPIFFS, "/www/side-menu-old-ie.css");
  server.serveStatic("/chat.css", SPIFFS, "/www/chat.css");
  server.serveStatic("/win.json", SPIFFS, "/win.json");
  server.serveStatic("/lin.json", SPIFFS, "/lin.json");
  server.serveStatic("/images/48HN.gif", SPIFFS, "/www/images/48HN.gif");
  server.serveStatic("/images/Yed2.gif", SPIFFS, "/www/images/Yed2.gif");
  server.serveStatic("/images/W9Vo.gif", SPIFFS, "/www/images/W9Vo.gif");
  server.serveStatic("/fonts/KERNEL1.ttf", SPIFFS, "/www/fonts/KERNEL1.ttf");
  server.serveStatic("/fonts/halogen.otf", SPIFFS, "/www/fonts/halogen.otf");
  
  server.serveStatic("/WebSocketTester.html", SPIFFS, "/www/WebSocketTester.html").setAuthentication(APssid.c_str(), APpassword.c_str());
  server.serveStatic("/update.html", SPIFFS, "/www/update.html").setAuthentication(APssid.c_str(), APpassword.c_str());
  server.serveStatic("/net.html", SPIFFS, "/www/net.html").setAuthentication(APssid.c_str(), APpassword.c_str());
  server.serveStatic("/storedpayloads.html", SPIFFS, "/www/storedpayloads.html").setAuthentication(APssid.c_str(), APpassword.c_str());
  server.serveStatic("/livepayload.html", SPIFFS, "/www/livepayload.html").setAuthentication(APssid.c_str(), APpassword.c_str());
  server.serveStatic("/liveinput.html", SPIFFS, "/www/liveinput.html").setAuthentication(APssid.c_str(), APpassword.c_str());
  server.serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html").setAuthentication(APssid.c_str(), APpassword.c_str());

  server.onFileUpload([](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){  });
  server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){  });
  
  server.begin();
}


void WebServerStop()
{
  //there is no service stop!
  //server.stop();
}

void WebServerDisplay()
{
  mydisp.clearScreen();
  mydisp.setTextColor(WHITE);
  mydisp.setTextScale(1);
  mydisp.println(F("Browse to 10.0.5.1"));
  mydisp.print(F("u: "));
  mydisp.println(APssid);
  mydisp.print(F("p: "));
  mydisp.println(APpassword);
}

//////////////////////////////////////////////////////////////////////////////////


