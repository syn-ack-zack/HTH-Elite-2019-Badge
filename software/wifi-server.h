#ifndef HW_WIFISERVER_H
#define HW_WIFISERVER_H

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

extern AsyncWebSocket ws;

void APStart();
void APStop();
void APDisplay();
void WebServerStart();
void WebServerStop();
void WebServerDisplay();


#endif
