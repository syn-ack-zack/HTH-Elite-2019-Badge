
#include <SPI.h>
// include the library
#include <LoRa.h>
#include "core.h"
#include "textinput.h"

extern Adafruit_NeoPixel pixels;
extern WS2812FX ws2812fx;
//extern byte btncounter;
extern SSD_13XX mydisp;
extern volatile byte btnid;
extern byte appmode;

//SPI defs for LoRa radio
#define SS 5
#define RST 25
#define DI0 17
// #define BAND 429E6

// LoRa Settings
#define spreadingFactor 7
#define SignalBandwidth 125E3
#define codingRateDenominator 8
#define preambleLength 8


// misc vars
String msg;
String displayName;
String sendMsg;
char chr;
int i = 0;
int counter = 0;
SPIClass * vspi = NULL;

// Helpers func.s for LoRa
String mac2str(int mac)
{
  String s;
  byte *arr = (byte*) &mac;
  for (byte i = 0; i < 6; ++i)
  {
    char buff[3];
    // yea, this is a sprintf... fite me...
    sprintf(buff, "%2X", arr[i]);
    s += buff;
    if (i < 5) s += ':';
  }
  return s;
}


// let's set stuff up! 
void lora_init() {
  
  mydisp.clearScreen();
  mydisp.setCursor(0, 0);
  mydisp.setTextScale(1);
  mydisp.setTextColor(GREEN);

  // reset the screen
  while (!Serial); //If just the the basic function, must connect to a computer

  mydisp.print("HTH NET Initializing...\n");
  SPIClass * vspi = NULL;
  
  vspi = new SPIClass(VSPI);
  LoRa.setSPI(*vspi);
  LoRa.setPins(5,25,17);

  Serial.println(F("HTH Node"));
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.print("LoRa Spreading Factor: ");
  Serial.println(spreadingFactor);
  LoRa.setSpreadingFactor(spreadingFactor);
  LoRa.setSyncWord(0x34);

  Serial.print("LoRa Signal Bandwidth: ");
  Serial.println(SignalBandwidth);
  LoRa.setSignalBandwidth(SignalBandwidth);

  LoRa.setCodingRate4(codingRateDenominator);

  LoRa.setPreambleLength(preambleLength);

  mydisp.clearScreen();
  mydisp.setTextColor(GREEN);
  mydisp.println(F("HTH NET Initial OK!"));
  delay(500);
  Serial.println(F("HTH NET Initial OK!"));
  mydisp.clearScreen();
  mydisp.setCursor(5, 20);
  mydisp.print(F("HTH NET is running!"));
  ////mydisp.display();
  delay(2000);
  Serial.println(F("HTH NET"));
  // get MAC as initial Nick
  int MAC = ESP.getEfuseMac();
  displayName = mac2str(MAC);
  //displayName.trim(); // remove the newline
  Serial.print(F("Initial nick is ")); Serial.println(displayName);
  mydisp.clearScreen();
//  mydisp.println(F("Nickname set:"));
//  mydisp.println(displayName);
  //mydisp.display();
  mydisp.println();
  mydisp.println("Press Right To Type Message");
  mydisp.println();
  mydisp.println("Listening...");
  Serial.println("Send message via Serial here");
  delay(1000);

  btnid = 0;
  appmode=1;
  counter = 0;
  while (btnid != 4){
         chat_loop();
    }
  appmode=0;
  //btncounter++;
}

void KeyboardInput(){
  appmode=1;
  btnid = 0;
  mydisp.clearScreen();
  mydisp.setCursor(0, 0);
  mydisp.setTextColor(GREEN);
  mydisp.setTextScale(1);
  String t = "";
  TextInput(F("Input text via buttons:"), t, 16);
  if (t) { 
     LoRa.beginPacket();
     LoRa.print(t);
     LoRa.endPacket();
     mydisp.println("Sent!");
  }
  delay(500);
  btnid = 4;
  btncounter++;
}


void chat_loop() {
  if(btnid == 1){
    KeyboardInput();
  }
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String data="";
    mydisp.clearScreen();
    mydisp.setCursor(3, 0);
    mydisp.println(F("Received Message!"));
   // mydisp.display();
   while (LoRa.available()) {
      data += (char) LoRa.read();
    }
    //mydisp.println(data);
    //mydisp.setCursor(3, 22);
     mydisp.println("");
     mydisp.println(data);

    //mydisp.display();
    Serial.println(data);
  } // once we're done there, we read bytes from Serial
  if (Serial.available()) {
    chr = Serial.read();
    Serial.print(chr); // so the user can see what they're doing :P
    if (chr == '\n' || chr == '\r') {
      msg += chr; //msg+='\0'; // should maybe terminate my strings properly....
      if (msg.startsWith("/")) {
//        // clean up msg string...
//        msg.trim(); msg.remove(0, 1);
//        // process command...
//        char cmd[1]; msg.substring(0, 1).toCharArray(cmd, 2);
//        switch (cmd[0]){
//          case '?':
//            Serial.println(F("Supported Commands:"));
//            Serial.println(F("h - this message..."));
//            Serial.println(F("n - change Tx nickname..."));
//            Serial.println(F("d - print Tx nickname..."));
//            break;
//          case 'n':
//            displayName = msg.substring(2);
//            Serial.print(F("Display name set to: ")); Serial.println(displayName);
//            break;
//          case 'd':
//            Serial.print(F("Your display name is: ")); Serial.println(displayName);
//            break;
//          default:
//            Serial.println(F("command not known... use 'h' for help..."));
//        }
//        msg = "";
      }
      else {
        Serial.print(msg);
        // assemble message
        sendMsg += (String) displayName;
        sendMsg += "> ";
        sendMsg += (String) msg;
//        // send message
          // send packet
        LoRa.beginPacket();
        LoRa.print(msg);
        LoRa.endPacket();
        mydisp.clearScreen();
        mydisp.setCursor(1, 0);
        mydisp.print(sendMsg);
        ////mydisp.display();
        // clear the strings and start again :D
        msg = "";
        sendMsg = "";
      }
    }
    else {
      msg += chr;
    }
  }
}

