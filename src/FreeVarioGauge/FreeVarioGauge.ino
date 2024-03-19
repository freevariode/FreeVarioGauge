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
    Performance verbessern
    Menü testen
    Gerät testen
    Treiber um Windinfo erweitern
    DrawData in Tab verschieben
*/

//*************************************************
//****  Screen and SPIFFS Headers and Defines  ****
//*************************************************
#include <Streaming.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <EEPROM.h>
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
TFT_eSprite needleGreen = TFT_eSprite(&tft);
TFT_eSprite needleBlue = TFT_eSprite(&tft);
TFT_eSprite drawOuterLine = TFT_eSprite(&tft);
TFT_eSprite drawColoredArc = TFT_eSprite(&tft);
TFT_eSprite drawRectangle = TFT_eSprite(&tft);
TFT_eSprite number = TFT_eSprite(&tft);
TFT_eSprite background = TFT_eSprite(&tft);
WebServer server(80);

Print &cout = Serial;
TaskHandle_t SerialScanTask, TaskEncoder, TaskValueRefresh, ArcRefreshTask;

SemaphoreHandle_t xTFTSemaphore;

const long NOT_SET = -1;
const long LONGPRESS_TIME = 500;
const long SHORTPRESS_TIME_MIN = 20;
const long SHORTPRESS_TIME_MAX = 50;
long pushButtonPressTime = NOT_SET;

const String SOFTWARE_VERSION = "  V2.0 - 2024";

const char *host = "FreeVario_Displayboard";
const char *ssid = "FV_Displayboard";
const char *password = "12345678";

String st;
String content;
String displayMode = "Waiting ...";
String soundMode = "Waiting ...";
String displayIP = "";
String soundIP = "";

static String mod;
static String mce;
static String nameSetting = "QNH";
static String nameSpeed = "GS";
static String nameHight = "MSL";
static String valueSetting, valueSpeed, valueHight;
static String unitSpeed, unitHight, unitSetting;
static String stf_mode;
static String valueQnhAsString = "1013";
static String valueBugAsString = "0";
static String valueMuteAsString = "ON";
static String valueWindAsString = "ON";
static String valueAttenAsString = "2";
static String valueGrsAsString = "0";
static String valueTasAsString = "0";
static String valueVaaAsString = "+0.0";
static String valueVanAsString = "+0.0";
static String valueHigAsString = "0";
static String valueHagAsString = "0";
static String valueAwsAsString = "0";
static String valueCwsAsString = "0";
static String valueMacAsString = "0.0";
static String menuSpeedColor = "WHITE";
static String menuHightColor = "WHITE";
static String menuValueColor = "WHITE";

extern uint16_t logoOV[];

static float var = 0;
static float valueVaaAsFloat = 0;
static float valueVanAsFloat = 0;
static float valueTasAsFloat = 0;
static float valueGrsAsFloat = 0;
static float valueMacAsFloat = 0.5;
static float valueHagAsFloat = 0;
static float valueHigAsFloat = 0;
static float valueAwsAsFloat = -1000;
static float valueCwsAsFloat = -1000;
static float valueAwdAsFloat = 1;
static float valueCwdAsFloat = 1;
static float avgWindAngle = 0;
static float instWindAngle = 0;
static float valueHeaAsFloat = 0;
static float tem = 0;
static float stf = 0.0;
static float menuActiveSince = 0;                  // Will be updated in menu run
float encoderPosition = (float) - 999;

static double stfValue = 0;
static double valueQnhAsFloat = 1013;
static double valueBugAsFloat = 0;

static bool varWasUpdated = true;
static bool vaaWasUpdated = true;
static bool vanWasUpdated = true;
static bool tasWasUpdated = true;
static bool grsWasUpdated = true;
static bool mcWasUpdated =  true;
static bool hagWasUpdated = true;
static bool higWasUpdated = true;
static bool awsWasUpdated = true;
static bool cwsWasUpdated = true;
static bool awdWasUpdated = true;
static bool cwdWasUpdated = true;
static bool heaWasUpdated = true;
static bool tempWasUpdated = true;
static bool qnhWasUpdated = true;
static bool bugWasUpdated = true;
static bool attWasUpdated = true;
static bool muteWasUpdated = true;
static bool windWasUpdated = true;
static bool stfModeWasUpdate = true;
static bool serial2Error = false;

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

const int MENU_SPEED_TYP = 1;
const int MENU_HIGHT_TYP = 2;
const int MENU_VALUE_TYP = 3;
const int MENU_VALUE_QNH = 1;
const int MENU_VALUE_BUG = 2;
const int MENU_VALUE_MUTE = 3;
const int DEBOUNCE_DELAY = 20;
int i = 0;
int n = 0;
int statusCode;
int stf_mode_state;
int spriteNameWidthSpeed, spriteValueWidthSpeed, spriteunitWidthSpeed;
int spriteNameWidthHight, spriteValueWidthHight, spriteunitWidthHight;
int spriteNameWidthSetting, spriteValueWidthSetting, spriteunitWidthSetting;
int startAngle, segmentDraw, segmentCount;
int Wificount = 0;
int valueMuteAsInt = 1;
int valueAttenAsInt = 2;
int valueWindAsInt = 1;
int changeMode;
int oldChangeMode;
int offset = 0;
int selectedMenu = MENU_SPEED_TYP;

static int requestDrawMenu = 0;
static int requestDrawMenuLevel = 0;
static bool requestMenuPaint = false;
static bool requestMenuFontPaint = false;

static unsigned long lastTimeBoot = 0;
static unsigned long lastTimeReady = 0;
static unsigned long lastTimeModeWasSend = 0;
static unsigned long lastTimeSerial2 = 0;
unsigned long loopTime = 5000;

//************************************
//****  Draw ValueBoxes and Data  ****
//************************************
void DrawInfo(TFT_eSprite fontOfName, uint32_t color, String infoType, String spriteName, String value, String unit, int spriteNameWidth, int spriteValueHight, int spriteValueWidth, int spriteunitWidth, int x, int y) {
  fontOfName.loadFont("micross15");
  fontOfName.createSprite(spriteNameWidth, 25);
  fontOfName.setTextColor(color, BLACK);
  fontOfName.fillSprite(BLACK);
  fontOfName.fillSprite(TFT_BLACK);
  fontOfName.setTextSize(2);
  fontOfName.setTextDatum(TR_DATUM);
  fontOfName.drawString(spriteName, spriteNameWidth, 2);
  fontOfName.pushToSprite(&background, x, y, TFT_BLACK);
  fontOfName.deleteSprite();
  fontOfName.unloadFont();

  if (infoType == "small") {
    fontOfName.loadFont("micross30");
  }
  else {
    fontOfName.loadFont("micross50");
  }

  fontOfName.createSprite(spriteValueWidth, spriteValueHight);
  fontOfName.setTextColor(color, BLACK);
  fontOfName.fillSprite(BLACK);
  fontOfName.fillSprite(TFT_BLACK);
  fontOfName.setTextSize(3);
  fontOfName.setTextDatum(TR_DATUM);
  fontOfName.drawString(value, spriteValueWidth, 2);
  fontOfName.pushToSprite(&background, x + spriteNameWidth, y, TFT_BLACK);
  fontOfName.deleteSprite();

  fontOfName.createSprite(spriteunitWidth + 5, 25);
  fontOfName.setTextColor(color, BLACK);
  fontOfName.fillSprite(BLACK);
  fontOfName.fillSprite(TFT_BLACK);
  fontOfName.setTextSize(3);
  fontOfName.setTextDatum(TR_DATUM);
  fontOfName.drawString(unit, spriteunitWidth, 2);
  fontOfName.pushToSprite(&background, x + spriteNameWidth + spriteValueWidth, y, TFT_BLACK);
  fontOfName.deleteSprite();
  fontOfName.unloadFont();
}
