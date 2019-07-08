# Hackers Teaching Hackers 2019 Badge
This repository contains the hardware and software source code to fabrciate the HTH 2019 Badge. Details on the use and flashing of the badge can be found below. 

![HTH Badge](https://i.imgur.com/naiMRHil.jpg?1)


## Badge Hardware Specs
* ESP32
* Atmel 32u4
* 96×64 Graphic Full Color OLED Display
* 19x Mini NeoPixel RGB LEDs
* 4 tactile buttons
* 8 Mb Storage
* 1 2x3 "Shitty Add On" Connector
* Powered via 2 AA batteries or USB

## Badge Functions
* LoRa Arsenal
	* LoRa 915Mhz Tactile Button Chat
	* LoRa 915Mhz Web App Chatroom
* WiFi Tools
	* 802.11 channel activity monitor
	* Access point scanner / Open AP detector
	* Packet Monitor and Deauth sniffer
	* Can act as a client or access point (modify config.json via SPIFFS to update AP connection details)
	* Webserver (edit filesystem files, trigger payloads remotely)
	* NTP client
* HID Toolkit (Not Working)
	* An issue with the serial communication currently prevents the ESP32 from controlling the Atmega. 
	* Any HID functionality must be standalone and flashed to the Atmega.
	* This repo will be updated as any changes occur.
	
* LED patterns and OLED graphics
* Misc. extras

## Usage Instructions
### Power Switch
In order to turn on the device, it must be powered via 2 AA batteries or USB. The main power switch is located in the center.

### Buttons / UI

Up and Down is on the left side of the OLED, and Left and Right is on the right side of the OLED.

In order to go into a menu or run a program, click the right button. To exit most applications, click the left button. Some programs like the snake game require all 4 buttons so there is no way to clean exit, a power cycle is needed. 

### HTH NET (LoRa 915Mhz)
This badge contains an SX1278 LoRa transceiver which allows for long range low bandwidth communication using the LoRa protocol over 915Mhz. Each badge is capable of receiving and sending messages using this frequency by utilizing the HTH NET feature of the badge. 

Once you start the chat it will display any received message on the screen. Follow the on screen prompts to type out and send messages. The '~' is used as a delimiter to signify the end of a typed message. 

Alternatively you can utililze the Webserver by first starting up the Access Point at `WiFi Server Tools -> AP Start`. Connect to this network and then start up the Webserver at `WiFi Server Tools -> Webserver Start`. Navigate to the indicated IP, http://10.0.1.5/ to access the web app. HTH Net can be utilized here to have a more convenient interface to send and recieve LoRa messages. 

![](https://pbs.twimg.com/media/D88i2IzWsAAne2L.jpg)

### Webserver
Navigate to WiFi Server Tools -> Webserver Start. You can access the webserver if running an AP or if connected. The pages are protected through basic authentication, username and password generated dynamically and is provided on the screen.

A SPIFFS Editor can be used to edit the flash filesystem on the badge. This is useful to modify config files which are used in several instances. For instance, to have your badge connect to another wireless network, you configure the ESSID and Password in `config.json`. 


### Bling
There are 40+ LED patterns included. You can also trigger each type of pattern individually. 

## Configuration / Customization
### Settings menu in OLED UI
There are individual settings for OLED and LED brightness. Region setting is related to WiFi channel scanning.

### JSON configuration files
There are 4 different json files in SPIFFS. There are a couple ways to edit these files and get them loaded on the badge, but the easiest is through the webserver.
1. config.json = Main device configuration. Includes parameters such as LED and OLED brightness settings, ssid/password if connecting to a wireless client network, region information, and google auth HMAC. 
2. lin.json / win.json = Configuration files for the Linux and Windows KB/M payloads. 
3. static.json = Configuration file for the stored static strings. 


## Compiling Instructions
These instructions will get you a copy of the project up and running on your local machine.
### Prerequisites
What things you need to install the software and how to install them
```
1. Arduino / compiler
2. SparkFun Pro Micro v3 com driver
https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/installing-windows
https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/installing-mac--linux
3. Libraries
  https://raw.githubusercontent.com/sparkfun/Arduino_Boards/master/IDE_Board_Manager/package_sparkfun_index.json
  https://github.com/adafruit/Adafruit_NeoPixel
  https://github.com/adafruit/Adafruit-GFX-Library
  https://github.com/adafruit/Adafruit_CircuitPlayground
  https://github.com/kitesurfer1404/WS2812FX
  https://github.com/me-no-dev/ESPAsyncWebServer
  https://github.com/me-no-dev/AsyncTCP
  https://github.com/bblanchon/ArduinoJson
  https://github.com/me-no-dev/arduino-esp32fs-plugin
  https://github.com/sandeepmistry/arduino-LoRa
  Manually copy the SSD_13XX and qMenuSystem folders to Arduino libraries folder. We included a snapshot of the other libraries used in case newer versions break our code.
4. 3.3v FTDI serial cable
5. Hackers Teaching Hackers 2019 Badge
```
### Compiling / Flashing Atmel Code
* Set the board to SparkFun Pro Micro
* Set the processor as ATmega32u4 3.3v and NOT the 5v one

### Compiling / Flashing ESP Code
* Set the board to SparkFun ESP32 Thing

#### Programming through FTDI port 
To interface with the ESP32 over FTDI serial, you will need a FTDI USB adapter. These can be found online or at some retailers like MicroCenter. [FTDI Adapter on Amazon](https://www.amazon.com/HiLetgo-FT232RL-Converter-Adapter-Breakout/dp/B00IJXZQ7C/ref=pd_lpo_sbs_147_t_1?_encoding=UTF8&psc=1&refRID=TW098QMMC5YX7B7DFNS5)

1. Flash the atmel with the 'atmel-esp32ftdi-pin7' code first via the USB connection. this will toggle pin 7 and the rx/tx switch to change the serial port communiction to the FTDI port. 
2. Connect FTDI cable to the board while holding the left button down (which is on the right side, top button)
3. Upload code.
4. You can optionally flash the 'atmel-km-relay' code back to the atmel. Most serial functionality isn't working correctly at this time, however the mouse jiggler feature is configured to work when plugged in. Note that you may need to reset the the atmel by shorting GND to [Pin 13](https://www.arduino.cc/en/Hacking/PinMapping32u4) before succesfully reflashing. 

#### Programming through USB port / Atmel
Pass through programming doesn't work at the time of writing. Communication is passed through, but something is holding up the ESP32 from fully going into the programming mode

### Uploading SPIFFS files
The keyboard/mouse payloads, configuration files, webserver files are all files that are loaded into a special section in storage marked as SPIFFS. While still having pin 7 set to allow for the FTDI connection, use the 'ESP32 Sketch Data Upload' function in the tools menu. This will compress and compile all the files in the 'data' folder, and upload it. 

## Authors / Contributors
* **@syn-ack-zack**

## Acknowledgments
* Thanks to all contributes and maintainers of any codebase which was used or referenced. 
* Special thanks to Hacker Warehouse and Garret Gee for the work on their [DC26 badge](https://github.com/hackerwarehouse/HW-DC26-Badge). Much of the materials around the core components (esp32 / atmega34u ) was referenced from Hacker Warehouse's open sourced badge. 
