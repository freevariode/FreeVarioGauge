//    VarioSound is a programm to generate the vario sound using NMEA Output of OpenVario.
//    Copyright (C) 2019  Dirk Jung Blaubart@gmx.de
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#define STF_MODE 13
#define STF_AUTO 33
//#define Free 14                      // Automatic mode through flaps or XCSoar; Connect button to GND, connect 10 kOhm pull-up resistor between 3.3V and pin
#define PTT 27                        // VarioSound off by pressing the radio button; Connect button to GND, connect 10 kOhm pull-up resistor between 3.3V and pin   
#define SINFAKT 127.0             

#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc.h"
#include "driver/dac.h"
#include "driver/i2s.h"
#include "FS.h"
#include "SPIFFS.h"
#include <Streaming.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
WebServer server(80);

Print &cout = Serial;
TaskHandle_t SoundTask;

const int Varioschalter = 15;         // Connect button to GND, connect 10 kOhm pull-up resistor between 3.3V and pin
const int STFSchalter = 5;            // Connect button to GND, connect 10 kOhm pull-up resistor between 3.3V and pin
const int STFAuto = 19;               // Flap connection; Connect button to GND, connect 10 kOhm pull-up resistor between 3.3V and pin´

const String SOFTWARE_VERSION = "  V2.4.4 - 2025";

const char *host = "FreeVario_Soundboard";
const char *ssid = "FV_Soundboard";
const char *password = "12345678";

String mod;                           // current mode
String rem = "A";                     // current Remot Stick mode
String st;
String content;
String soundMode = "Waiting ...";
String soundIP = "";
String valueTasAsString = "0";

int valueMuteAsInt = 1;               // mute via PTT is active
int valueAttenAsInt = 2;              // Attenuation ist set
int valueSTFAsInt = 1;
int count = 0;                        // Counter for STF Sound
int i = 0;
int n = 0;
int statusCode;
int Wificount = 0;
int Loopcount = 0;
int changeMode = 0;
int oldChangeMode = 0;
int varioSchalter_state, stfSchalter_state, stfAuto_state, xc_WK_state;
int BaudValue = 115200;
int baudDetect = 0;

byte RXD2 = 16;
byte TXD2 = -1;                       // -1 means it not used beacause of trouble with Dispolay-ESP32. Set TXD2 to 17 if you like to use

bool error = false;
bool updatemode = false;
bool SourceIsXCSoar = false;
bool SourceIsLarus = false;
bool initDone = false;
bool startSound = false;
bool varAvailable = false; 

float sf = 0;
float stfValue = 0;                        // Speed to Fly value
float sfOld = 0;
float var = 0;
float varOld = 0;
float tas;
float freqValue = 25;
float errorFreq = 1000;
float valueTasAsFloat = 0;
float stf = 0.0;
uint8_t ratio = 50;

long pulseStarts = 0;
long pulsEnds = 0;
long startTime = 0;
long connectTime = 0;
long ChangeBaud = 0;

unsigned long pulseTime = 0;
unsigned long startTimePulse = 0;
unsigned long loopTime = 8000;
