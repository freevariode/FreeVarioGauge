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
#define XC_WK 14                      // Automatic mode through flaps or XCSoar; Connect button to GND, connect 10 kOhm pull-up resistor between 3.3V and pin
#define PTT 27                        // VarioSound off by pressing the radio button; Connect button to GND, connect 10 kOhm pull-up resistor between 3.3V and pin
#define FNC_PIN 4                     // Can be any digital IO pin                    

#include "FS.h"
#include "SPIFFS.h"
#include <Streaming.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <AD9833.h>                   // Include the library
AD9833 gen(FNC_PIN);                  // Defaults to 25MHz internal reference frequency
WebServer server(80);

Print &cout = Serial;
TaskHandle_t SoundTask;

const int Varioschalter = 15;         // Connect button to GND, connect 10 kOhm pull-up resistor between 3.3V and pin
const int STFSchalter = 5;            // Connect button to GND, connect 10 kOhm pull-up resistor between 3.3V and pin
const int STFAuto = 19;               // Flap connection; Connect button to GND, connect 10 kOhm pull-up resistor between 3.3V and pin´

const String SOFTWARE_VERSION = "  V1.3 - 2023";

const char *host = "FreeVario_Soundboard";
const char *ssid = "FV_Soundboard";
const char *password = "12345678";

String mod;                           // current mode
String rem = "A";                     // current Remot Stick mode
String st;
String content;
String soundMode = "Waiting ...";
String soundIP = "";

int valueMuteAsInt = 1;               // mute via PTT is active
int valueAttenAsInt = 2;              // Attenuation ist set
int count = 0;                        // Counter for STF Sound
int i = 0;
int n = 0;
int statusCode;
int Wificount = 0;
int Loopcount = 0;
int changeMode = 0;
int oldChangeMode = 0;
int varioSchalter_state, stfSchalter_state, stfAuto_state, xc_WK_state;

byte RXD2 = 16;
byte TXD2 = -1;                       // -1 means it not used beacause of trouble with Dispolay-ESP32. Set TXD2 to 17 if you like to use

bool error = false;
bool updatemode = false;

float sf = 0;
float stfValue = 0;                        // Speed to Fly value
float sfOld = 0;
float var = 0;
float varOld = 0;
float tas;
float freqValue = 0;
float freqValueOld = 350;
float freqValueNeg = 0;
float freqValueInc = 0 ;
float errorFreq = 1000;
float valueTasAsFloat = 0;
static float stf = 0.0;

long pulseStarts = 0;
long pulsEnds = 0;
long startTime = 0;
long connectTime = 0;

unsigned long pulseTime = 0;
unsigned long startTimePulse = 0;
unsigned long loopTime = 8000;

/////////////////////
// function to calculate pulse length
/////////////////////
float calculatePulse(float liftIn) {
  float calculatedPulseLength = 0;
  if (digitalRead(STF_MODE) == LOW) {
    if (liftIn > 0.5 && liftIn < 7) {
      calculatedPulseLength = (3.5 - (44100 / (48000 / (liftIn * 0.5)))) * 100;
    }
    else if (liftIn > 7) {
      calculatedPulseLength = 30;
    }
    else {
      calculatedPulseLength = (44100 / (48000 * 2));
    }
    return calculatedPulseLength;
  }
  if (digitalRead(STF_MODE) == HIGH) {
    if (liftIn > 0.5 && liftIn < 7) {
      calculatedPulseLength = (4 - (44100 / (48000 / (liftIn * 0.2)))) * 100;
    }
    else if (liftIn > 7) {
      calculatedPulseLength = 270;
    }
    else {
      calculatedPulseLength = (44100 / (48000 * 2));
    }
    return calculatedPulseLength;
  }
}

/////////////////////
// function to calculate frequency
/////////////////////
float calculateNewFreq(float newValue, float oldValue) {
  if (digitalRead(STF_MODE) == LOW && (newValue >= 0) && (newValue < 8) && (newValue != oldValue)) {
    freqValue = (350 + (120 * newValue));
  }
  else if (digitalRead(STF_MODE) == LOW && (newValue < 0) && (newValue > -8) && (newValue != oldValue)) {
    freqValue = (350 / (1 - (0.1 * newValue)));
  }
  else if (digitalRead(STF_MODE) == LOW && (newValue > 8) && (newValue != oldValue)) {
    freqValue = 1310;
  }
  else if (digitalRead(STF_MODE) == HIGH && (newValue >= 0) && (newValue < 8) && (newValue != oldValue)) {
    freqValue = (350 + (120 * newValue));
  }
  else if (digitalRead(STF_MODE) == HIGH && (newValue < 0) && (newValue > -8) && (newValue != oldValue)) {
    freqValue = (350 / (1 - (0.1 * newValue)));
  }
  else if (digitalRead(STF_MODE) == HIGH && (newValue > 8) && (newValue != oldValue)) {
    freqValue = 1310;
  }
  freqValueInc = (freqValue - freqValueOld) / 8;
  freqValueOld = freqValue;
  return freqValue;
}

//**************************
//****  Filter for STF  ****
//**************************
float filter(float filteredSTF, uint16_t filterfactor) {
  static uint16_t count = 0;
  // so that at the beginning the value is close to the measured value
  if (count < filterfactor) {
    filterfactor = count++;
  }
  stf = ((stf * filterfactor) + filteredSTF) / (filterfactor + 1);
  return stf;
}

void setup() {
  server.on("/SoftwareVersion", []() {
    server.send(200, "application/json",  "\"" + SOFTWARE_VERSION + "\"");
  });

  Serial.begin(115200, SERIAL_8N1);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  gen.Begin();
  gen.ApplySignal(SINE_WAVE, REG0, 0);
  gen.EnableOutput(true);             // Turn ON the output - it defaults to OFF
  AD9833 gen(FNC_PIN);                // Defaults to 25MHz internal reference frequency
  pinMode(STF_MODE, OUTPUT);
  pinMode(XC_WK, INPUT_PULLUP);
  pinMode(PTT, INPUT_PULLUP);
  pinMode(Varioschalter, INPUT_PULLUP);
  pinMode(STFSchalter, INPUT_PULLUP);
  pinMode(STFAuto, INPUT_PULLUP);
  xTaskCreate(Sound, "Create Sound", 1000, NULL, 50, &SoundTask);
  SPIFFSstart();
  server.serveStatic("/B612-Bold.ttf", SPIFFS, "/B612-Bold.ttf");
  server.serveStatic("/B612-Regular.ttf", SPIFFS, "/B612-Regular.ttf");
  server.serveStatic("/style.css", SPIFFS, "/style.css");
  server.serveStatic("/script.js", SPIFFS, "/script.js");
  server.serveStatic("/serverIndex", SPIFFS, "/sound-update.html");
  server.serveStatic("/", SPIFFS, "/sound-login.html");
}

void loop() {
  /////////////////////
  // Analysis automatic mode
  /////////////////////
  varioSchalter_state = digitalRead(Varioschalter);
  stfSchalter_state = digitalRead(STFSchalter);
  stfAuto_state = digitalRead(STFAuto);
  xc_WK_state = digitalRead(XC_WK);

  if ((varioSchalter_state == 1) && (stfSchalter_state == 1)) {
    digitalWrite(STF_AUTO, HIGH);
  }
  else {
    digitalWrite(STF_AUTO, LOW);
  }
  if (((varioSchalter_state == 1) && (stfSchalter_state == 1) && (rem == "A") && (xc_WK_state == 0) && (stfAuto_state == 1)) ||
      ((varioSchalter_state == 1) && (stfSchalter_state == 1) && (rem == "A") && (xc_WK_state == 1) && (mod == "C")) ||
      ((varioSchalter_state == 1) && (stfSchalter_state == 1) && (rem == "C")) ||
      ((varioSchalter_state == 0) && (stfSchalter_state == 1))) {
    digitalWrite(STF_MODE, LOW);
  }
  else if (((varioSchalter_state == 1) && (stfSchalter_state == 1) && (rem == "A") && (xc_WK_state == 0) && (stfAuto_state == 0)) ||
           ((varioSchalter_state == 1) && (stfSchalter_state == 1) && (rem == "A") && (xc_WK_state == 1) && (mod == "S")) ||
           ((varioSchalter_state == 1) && (stfSchalter_state == 1) && (rem == "S")) ||
           ((varioSchalter_state == 1) && (stfSchalter_state == 0))) {
    digitalWrite(STF_MODE, HIGH);
  }

  /////////////////////
  // Check for update mode
  /////////////////////
  if (Loopcount == 0) {
    Loopcount = 1;
    changeMode = digitalRead(STF_MODE);
    oldChangeMode = changeMode;
    startTime = millis();
  }

  while (millis() - startTime <= loopTime) {
    varioSchalter_state = digitalRead(Varioschalter);
    stfSchalter_state = digitalRead(STFSchalter);
    if ((varioSchalter_state == 0) && (stfSchalter_state == 1)) {
      digitalWrite(STF_MODE, LOW);
    }
    else if ((varioSchalter_state == 1) && (stfSchalter_state == 0)) {
      digitalWrite(STF_MODE, HIGH);
    }
    changeMode = digitalRead(STF_MODE);
    if (oldChangeMode != changeMode) {
      loopTime = 0;
      updatemode = true;
      long toneTime = millis();
      while (millis() - toneTime <= 200) {
        gen.ApplySignal(SINE_WAVE, REG0, 1000);
      }
      gen.ApplySignal(SINE_WAVE, REG0, 0);
    }
  }

  if (updatemode == true) {
    if (Wificount == 0) {
      Wificount = 1;
      Serial2.end();
      WiFi.softAP(ssid, password);
      delay(100);
      IPAddress Ip(192, 168, 3, 1);    //setto IP Access Point same as gateway
      IPAddress NMask(255, 255, 255, 0);
      WiFi.softAPConfig(Ip, Ip, NMask);
      IPAddress myIP = WiFi.softAPIP();
      server.begin();
      soundIP = myIP.toString();
      if (soundIP != "") {
        soundMode = "Ready";
      }
      else {
        soundMode = "Error";
      }

      RXD2 = 17;
      TXD2 = 16;
      Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
      delay(10);
      pinMode(LED_BUILTIN, OUTPUT);
      Serial.println();
      Serial.println("Startup Update Mode");
      connectTime = millis();
      while (millis() - connectTime <= 10000) {
        Serial2.println("$PFV," + soundMode + "," + soundIP + "*");
        delay(500);
      }
      /*use mdns for host name resolution*/
      if (!MDNS.begin(host)) { //http://esp32.local
        Serial.println("Error setting up MDNS responder!");
        while (1) {
          delay(1000);
        }
      }
      Serial.println("mDNS responder started");
      /*return index page which is stored in serverIndex */
      server.on("/", HTTP_GET, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", "/sound-login.html");
      });
      server.on("/serverIndex", HTTP_GET, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", "/sound-update.html");
      });
      server.on("/style.css", HTTP_GET, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/css", "/style.css");
      });
      server.on("/script.js", HTTP_GET, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/js", "/script.js");
      });
      /*handling uploading firmware file */
      server.on("/update", HTTP_POST, []() {
        server.sendHeader("Connection", "close");
        server.send((Update.hasError()) ? 422 : 200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        delay(1000);
        ESP.restart();
      }, []() {
        HTTPUpload& upload = server.upload();
        if (upload.status == UPLOAD_FILE_START) {
          Serial.printf("Update: %s\n", upload.filename.c_str());
          if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
            Update.printError(Serial);
          }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
          /* flashing firmware to ESP*/
          if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Update.printError(Serial);
          }
        } else if (upload.status == UPLOAD_FILE_END) {
          if (Update.end(true)) { //true to set the size to the current progress
            Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
          } else {
            Update.printError(Serial);
          }
        }
      });
      server.begin();
    }
    server.handleClient();
    delay(1);
  }

  /////////////////////
  // read serial port
  /////////////////////
  if (updatemode == false) {
    WiFi.mode(WIFI_OFF);
    char Data;
    String DataString;
    if (Serial2.available()) {
      Data = Serial2.read();
      if (Data == '$') {
        while (Data != 10) {
          DataString += Data;
          Data = Serial2.read();
        }
        //Serial.println(DataString);
      }
      if (DataString.startsWith("$PFV")) {
        //Serial2.println(DataString);
        int pos = DataString.indexOf(',');
        DataString.remove(0, pos + 1);
        int pos1 = DataString.indexOf(',');                   //finds the place of the first,
        String variable = DataString.substring(0, pos1);      //captures the first record
        int pos2 = DataString.indexOf('*', pos1 + 1 );        //finds the place of *
        String wert = DataString.substring(pos1 + 1, pos2);   //captures the second record


        /////////////////////
        // Analysis of the climb rate
        /////////////////////
        if (variable == "VAR") {
          var = wert.toFloat();
        }


        /////////////////////
        // Analysis of the current XCSoar mode
        /////////////////////
        if (variable == "MOD") {
          mod = wert;
        }

        /////////////////////
        // Analysis of the current Remote control mode
        /////////////////////
        if (variable == "REM") {
          rem = wert;
        }

        /////////////////////
        // Analysis of the true airspeed
        /////////////////////
        if (variable == "TAS") {
          tas = wert.toFloat();
        }


        /////////////////////
        // Analysis of speed to fly
        /////////////////////
        if (variable == "STF") {
          stfValue = wert.toFloat();
          if (digitalRead(STF_MODE) == 1 && tas > 10) {
            int FF = (valueAttenAsInt * 10) + 1;
            stf = filter(stfValue, FF);
          }
          else {
            stf = tas;
          }
        }


        /////////////////////
        // Analysis Mute
        /////////////////////
        else if (variable == "MUT") {
          valueMuteAsInt = wert.toInt();
        }


        /////////////////////
        // Analysis Attenuation
        /////////////////////
        else if (variable == "ATT") {
          valueAttenAsInt = wert.toInt();
        }

        sf = (tas - stf) / 10;
      }
      DataString = "";
      vTaskDelay(20);
    }
  }
}

void Sound(void *) {
  while (true) {
    sf = (tas - stf) / 10;


    /////////////////////
    // mute function using PTT
    /////////////////////
    if (digitalRead(PTT) == LOW && valueMuteAsInt ) {
      gen.ApplySignal(SINE_WAVE, REG0, 0);
      delay(1);
    }


    /////////////////////
    // calculate Vario sound
    /////////////////////
    else if (digitalRead(STF_MODE) == LOW && var > 0.5) {
      startTimePulse = millis();
      pulseTime = 0;
      while (pulseTime < calculatePulse(var)) {
        freqValueNeg = (-1 * freqValueOld) / 8;
        int  i = 0;
        while (i < 8 && freqValueNeg < 0) {
          i = i + 1;
          gen.IncrementFrequency (REG0, freqValueNeg);
          delay(1);
        }
        gen.ApplySignal(SINE_WAVE, REG0, 0);
        freqValueOld = 0;
        pulseTime = millis() - startTimePulse;
      }
      do  {
        calculateNewFreq(var, varOld);
        int  i = 0;
        while (i < 8) {
          i = i + 1;
          gen.IncrementFrequency (REG0, freqValueInc);
          delay(1);
        }
        gen.ApplySignal(SINE_WAVE, REG0, freqValue);
        pulseTime = millis() - startTimePulse;
      } while (pulseTime < (calculatePulse(var) + (calculatePulse(var) / 2)));
    }
    else if (digitalRead(STF_MODE) == LOW && var <= 0.5) {
      calculateNewFreq(var, varOld);
      int  i = 0;
      while (i < 8) {
        i = i + 1;
        gen.IncrementFrequency (REG0, freqValueInc);
        delay(1);
      }
      gen.ApplySignal(SINE_WAVE, REG0, freqValue);
    }


    /////////////////////
    // calculate STF sound
    /////////////////////
    else if (digitalRead(STF_MODE) == HIGH && ((sf > 1) || (sf < -1))) {
      if ((count = 0) || (pulseTime > 1050)) {
        startTimePulse = millis();
        pulseTime = 0;
      }
      if ((pulseTime < 300) || ((pulseTime > 450) && (pulseTime < 750))) {
        calculateNewFreq(sf, sfOld);
        int  i = 0;
        while (i < 8) {
          i = i + 1;
          gen.IncrementFrequency (REG0, freqValueInc);
          delay(1);
        }
        gen.ApplySignal(SINE_WAVE, REG0, freqValue);
        pulseTime = millis() - startTimePulse;
      }


      else if ((pulseTime >= 300) && (pulseTime <= 450) || ((pulseTime >= 750) && (pulseTime <= 1050))) {
        freqValueNeg = (-1 * freqValueOld) / 8;
        int  i = 0;
        while (i < 8 && freqValueNeg < 0) {
          i = i + 1;
          gen.IncrementFrequency (REG0, freqValueNeg);
          delay(1);
        }
        gen.ApplySignal(SINE_WAVE, REG0, 0);
        freqValueOld = 0;
        pulseTime = millis() - startTimePulse;
      }
      count = 0;
    }

    else if (digitalRead(STF_MODE) == HIGH && sf > -1 && sf < 1) {
      gen.ApplySignal(SINE_WAVE, REG0, 0);
      delay(1);
    }
  }
  varOld = var;
  sfOld = sf;
  vTaskDelay(10);
}

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
