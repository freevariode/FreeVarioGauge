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

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <AD9833.h>                   // Include the library
AD9833 gen(FNC_PIN);                  // Defaults to 25MHz internal reference frequency
WebServer server(80);
IPAddress local_IP(192, 168, 2, 1);

TaskHandle_t SoundTask;

const int Varioschalter = 15;         // Connect button to GND, connect 10 kOhm pull-up resistor between 3.3V and pin
const int STFSchalter = 5;            // Connect button to GND, connect 10 kOhm pull-up resistor between 3.3V and pin
const int STFAuto = 19;               // Flap connection; Connect button to GND, connect 10 kOhm pull-up resistor between 3.3V and pin´

const char* host = "FreeVario_Soundboard";
const char* ssid = "Default_SSID";
const char* passphrase = "Default_Password";

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
int changeMode = 0;

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
static float stf = 0.0;

long pulseStarts = 0;
long pulsEnds = 0;
long startTime = millis();
long connectTime = 0;

unsigned long pulseTime = 0;
unsigned long startTimePulse = 0;
unsigned long loopTime = 6000;

//Function Decalration
bool testWifi(void);
void launchWeb(void);
void setupAP(void);

//**********************
//****  Login page  ****
//**********************
const char* loginIndex =
  "<form name='loginForm'>"
  "<table width='20%' bgcolor='A09F9F' align='center'>"
  "<tr>"
  "<td colspan=2>"
  "<center><font size=4><b>Soundboard Login Page</b></font></center>"
  "<br>"
  "</td>"
  "<br>"
  "<br>"
  "</tr>"
  "<td>Username:</td>"
  "<td><input type='text' size=25 name='userid'><br></td>"
  "</tr>"
  "<br>"
  "<br>"
  "<tr>"
  "<td>Password:</td>"
  "<td><input type='Password' size=25 name='pwd'><br></td>"
  "<br>"
  "<br>"
  "</tr>"
  "<tr>"
  "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
  "</tr>"
  "</table>"
  "</form>"
  "<script>"
  "function check(form)"
  "{"
  "if(form.userid.value=='Sound' && form.pwd.value=='Sound')"
  "{"
  "window.open('/serverIndex')"
  "}"
  "else"
  "{"
  " alert('Error Password or Username')/*displays error message*/"
  "}"
  "}"
  "</script>";

//*****************************
//****  Server Index Page  ****
//*****************************
const char* serverIndex =
  "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
  "<center><font size=4><b>Soundboard Update Page</b></font></center>"
  "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
  "<input type='file' name='update'>"
  "<input type='submit' value='Update'>"
  "</form>"
  "<div id='prg'>progress: 0%</div>"
  "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')"
  "},"
  "error: function (a, b, c) {"
  "}"
  "});"
  "});"
  "</script>";

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
  Serial.begin(115200, SERIAL_8N1);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  gen.Begin();
  gen.ApplySignal(SINE_WAVE, REG0, freqValueOld);
  gen.EnableOutput(true);             // Turn ON the output - it defaults to OFF
  AD9833 gen(FNC_PIN);                // Defaults to 25MHz internal reference frequency
  pinMode(STF_MODE, OUTPUT);
  pinMode(XC_WK, INPUT_PULLUP);
  pinMode(PTT, INPUT_PULLUP);
  pinMode(Varioschalter, INPUT_PULLUP);
  pinMode(STFSchalter, INPUT_PULLUP);
  pinMode(STFAuto, INPUT_PULLUP);
  changeMode = digitalRead(Varioschalter) - digitalRead(STFSchalter);

  xTaskCreate(Sound, "Create Sound", 1000, NULL, 50, &SoundTask);
}

void loop() {

  while (millis() - startTime <= loopTime) {
    int varioSchalter_state, stfSchalter_state, stfAuto_state, xc_WK_state;
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
    int oldChangeMode = changeMode;
    changeMode = digitalRead(Varioschalter) - digitalRead(STFSchalter);
    if (oldChangeMode != changeMode) {
      updatemode = true;
      loopTime = millis() + 500;
    }
  }

  if (updatemode == true) {
    if (Wificount == 0) {
      Wificount = 1;
      long toneTime = millis();
      while (millis() - toneTime <= 200) {
        gen.ApplySignal(SINE_WAVE, REG0, 1000);
      }
      Serial2.end();
      RXD2 = 17;
      TXD2 = 16;
      Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
      Serial.println();
      Serial.println("Disconnecting current wifi connection");
      WiFi.disconnect();
      EEPROM.begin(512); //Initialasing EEPROM
      delay(10);
      pinMode(LED_BUILTIN, OUTPUT);
      Serial.println();
      Serial.println();
      Serial.println("Startup");

      //---------------------------------------- Read eeprom for ssid and pass
      Serial.println("Reading EEPROM ssid");
      String esid;
      for (int i = 0; i < 32; ++i)
      {
        esid += char(EEPROM.read(i));
      }
      Serial.println();
      Serial.print("SSID: ");
      Serial.println(esid);
      Serial.println("Reading EEPROM pass");
      String epass = "";
      for (int i = 32; i < 96; ++i)
      {
        epass += char(EEPROM.read(i));
      }
      Serial.print("PASS: ");
      Serial.println(epass);
      WiFi.begin(esid.c_str(), epass.c_str());
      if (testWifi())
      {
        Serial.println("Succesfully Connected!!!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        soundMode = "Connected";
        soundIP = WiFi.localIP().toString();
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
          server.send(200, "text/html", loginIndex);
        });
        server.on("/serverIndex", HTTP_GET, []() {
          server.sendHeader("Connection", "close");
          server.send(200, "text/html", serverIndex);
        });
        /*handling uploading firmware file */
        server.on("/update", HTTP_POST, []() {
          server.sendHeader("Connection", "close");
          server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
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

      else
      {
        Wificount = 1;
        Serial.println("Turning the HotSpot On");
        setupAP();// Setup HotSpot
      }

      while ((WiFi.status() != WL_CONNECTED))
      {
        Serial.print(".");
        delay(100);
        server.handleClient();
      }
    }
    server.handleClient();
    delay(1);
  }

  int varioSchalter_state, stfSchalter_state, stfAuto_state, xc_WK_state;
  varioSchalter_state = digitalRead(Varioschalter);
  stfSchalter_state = digitalRead(STFSchalter);
  stfAuto_state = digitalRead(STFAuto);
  xc_WK_state = digitalRead(XC_WK);


  /////////////////////
  // read serial port
  /////////////////////
  if (updatemode == false) {
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
          int FF = (valueAttenAsInt * 10) + 1;
          stf = filter(stfValue, FF);
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

  /////////////////////
  // Analysis automatic mode
  /////////////////////
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

// **********************************
// ****  Fuctions used for WiFi  ****
// **********************************
bool testWifi(void)
{
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    delay(500);
    Serial.print("*");
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
}
void launchWeb()
{
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED)
    Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer();
  // Start the server
  server.begin();
  Serial.println("Server started");
}
void setupAP(void)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.println(")");
      delay(10);
    }
  }
  Serial.println("");
  st = "<ol>";
  for (int i = 0; i < n; ++i)
  {
    // Print SSID and RSSI for each network found
    st += "<li>";
    st += WiFi.SSID(i);
    st += " (";
    st += WiFi.RSSI(i);
    st += ")";
    st += "</li>";
  }
  st += "</ol>";
  delay(100);
  WiFi.softAP("FreeVario_Soundboard", "");
  delay(100);
  IPAddress Ip(192, 168, 1, 1);    //setto IP Access Point same as gateway
  IPAddress NMask(255, 255, 255, 0);
  WiFi.softAPConfig(Ip, Ip, NMask);
  IPAddress myIP = WiFi.softAPIP();
  connectTime = millis();
  soundMode = "Access";
  soundIP = WiFi.softAPIP().toString();
  while (millis() - connectTime <= 10000) {
    Serial2.println("$PFV," + soundMode + "," + soundIP + "*");
    delay(500);
  }
  Serial.println("Initializing_softap_for_wifi credentials_modification");
  launchWeb();
  Serial.println("over");
}

void createWebServer(void)
{
  {
    server.on("/", []() {
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content = "<!DOCTYPE HTML>\r\n<html>Welcome to Wifi Credentials Update page for the Soundboard of your FreeVario";
      content += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"scan\"></form>";
      content += ipStr;
      content += "<p>";
      content += st;
      content += "In the fields below, please enter the SSID and password of the wireless network you want to use.";
      content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=64><input type='submit'></form>";
      content += "</html>";
      server.send(200, "text/html", content);
    });
    server.on("/scan", []() {
      //setupAP();
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content = "<!DOCTYPE HTML>\r\n<html>go back";
      server.send(200, "text/html", content);
    });
    server.on("/setting", []() {
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");
      if (qsid.length() > 0 && qpass.length() > 0) {
        Serial.println("clearing eeprom");
        for (int i = 0; i < 96; ++i) {
          EEPROM.write(i, 0);
        }
        Serial.println(qsid);
        Serial.println("");
        Serial.println(qpass);
        Serial.println("");
        Serial.println("writing eeprom ssid:");
        for (int i = 0; i < qsid.length(); ++i)
        {
          EEPROM.write(i, qsid[i]);
          Serial.print("Wrote: ");
          Serial.println(qsid[i]);
        }
        Serial.println("writing eeprom pass:");
        for (int i = 0; i < qpass.length(); ++i)
        {
          EEPROM.write(32 + i, qpass[i]);
          Serial.print("Wrote: ");
          Serial.println(qpass[i]);
        }
        EEPROM.commit();
        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
        ESP.restart();
      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
      }
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(statusCode, "application/json", content);
    });
  }
}
