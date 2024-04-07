//    OpenVarioGauge is a programm to generate the vario display using NMEA Output
//    of OpenVario.
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
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

/** ToDo
    Ton durch ESP erzeugen lassen
*/

//*************************************************
//****  Screen and SPIFFS Headers and Defines  ****
//*************************************************
#include <Streaming.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Preferences.h>
Preferences prefs;

#include "LogoOV.h"
#include "FS.h"
#include "SPIFFS.h"
#include <ESP32Encoder.h>

ESP32Encoder Vario_Enc;

#define BLACK   0x0000
#define RED     0x001F
#define BLUE    0xfd40
#define GREEN   0x07E0
#define CYAN    0xFFE0
#define MAGENTA 0xF81F
#define YELLOW  0x07FF
#define WHITE   0xFFFF
#define GREY    0x632c
#define RXD2 16
#define TXD2 17
#define VE_PB 27
#define DEG2RAD 0.0174532925
#define STF_MODE 13
#define STF_AUTO 33
#define OuterRadius 160
#define InnerRadius 130
#define xCenter 160
#define yCenter 160

static TFT_eSPI tft = TFT_eSPI();
TFT_eSprite nameOfField = TFT_eSprite(&tft);
TFT_eSprite Name = TFT_eSprite(&tft);
TFT_eSprite smallFont = TFT_eSprite(&tft);
TFT_eSprite needleGreen = TFT_eSprite(&tft);
TFT_eSprite needleBlue = TFT_eSprite(&tft);
TFT_eSprite drawOuterLine = TFT_eSprite(&tft);
TFT_eSprite drawColoredArc = TFT_eSprite(&tft);
TFT_eSprite drawRectangle = TFT_eSprite(&tft);
TFT_eSprite number = TFT_eSprite(&tft);
TFT_eSprite background = TFT_eSprite(&tft);
WebServer server(80);

Print &cout = Serial;
TaskHandle_t SerialScanTask, TaskValueRefresh;

SemaphoreHandle_t xTFTSemaphore;

long NOT_SET = -1;
long pushButtonPressTime = NOT_SET;

const String SOFTWARE_VERSION = "  V2.0 - 2024";

const char *host = "FreeVario_Displayboard";
const char *ssid = "FV_Displayboard";
const char *password = "12345678";


String displayMode = "Waiting ...";
String soundMode = "Waiting ...";
String displayIP = "";
String soundIP = "";
String nameSetting = "QNH";
String nameSpeed = "NA";
String nameHight = "NA";
String stf_mode;
String valueQnhAsString = "1013";
String valueBugAsString = "0";
String valueMuteAsString = "NA";
String valueWindAsString = "NA";
String valueAttenAsString = "2";
String valueGrsAsString = "0";
String valueTasAsString = "0";
String valueVaaAsString = "+0.0";
String valueVanAsString = "+0.0";
String valueHigAsString = "0";
String valueHagAsString = "0";
String valueAwsAsString = "0";
String valueCwsAsString = "0";
String valueMacAsString = "0.0";

extern uint16_t logoOV[];

float var = 0;
float stf = 0.0;
float valueTasAsFloat = 0;
float valueMacAsFloat = 0.5;
float valueAwsAsFloat = -1000;
float valueCwsAsFloat = -1000;
float valueAwdAsFloat = -1000;
float valueCwdAsFloat = -1000;
float valueHeaAsFloat = 0;
float encoderPosition = (float) - 999;
float menuActiveSince = 0;                  // Will be updated in menu run

double valueQnhAsFloat = 1013;
double valueBugAsFloat = 0;

bool loadMenuFont = true;
bool loadMenuArc = true;
bool updatemode = false;
bool showBootscreen = true;
bool mci = false;
bool pushButtonIsLongpress = false;
bool pushButtonIsShortpress = false;
bool pushButtonPressed = false;
bool encoderLeft = false;
bool encoderRight = false;
bool encoderWasMoved = false;
bool menuWasTriggered = false;
bool subMenuTriggered = false;
bool subMenuLevelTwoTriggered = false;
bool requestMenuPaint = false;
bool WasSend = false;
bool SourceIsXCSoar = false;
bool SourceIsLarus = false;

int MENU_SPEED_TYP = 1;
int MENU_HIGHT_TYP = 2;
int MENU_VALUE_TYP = 3;
int MENU_VALUE_QNH = 1;
int MENU_VALUE_BUG = 2;
int MENU_VALUE_MUTE = 3;
int k = 0;
int stf_mode_state;
int startAngle, segmentDraw, segmentCount;
int Wificount = 0;
int valueMuteAsInt;
int valueAttenAsInt;
int valueWindAsInt;
int changeMode;
int oldChangeMode;
int offset = 0;
int selectedMenu = MENU_SPEED_TYP;
int requestDrawMenu = 1;
int requestDrawMenuLevel = 0;

unsigned long lastTimeBoot = 0;
unsigned long lastTimeReady = 0;

// ************************************
// ****  Initialize SPIFFS memory  ****
// ************************************
void SPIFFSstart() {
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nInitialisation done.");
}
