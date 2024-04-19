void UpdateMode() {

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
      while (millis() - toneTime <= 150) {
        gen.ApplySignal(SINE_WAVE, REG0, 600);
      }
      while (millis() - toneTime <= 200) {
        gen.ApplySignal(SINE_WAVE, REG0, 0);
      }
      while (millis() - toneTime <= 350) {
        gen.ApplySignal(SINE_WAVE, REG0, 800);
      }
      while (millis() - toneTime <= 400) {
        gen.ApplySignal(SINE_WAVE, REG0, 0);
      }
      while (millis() - toneTime <= 550) {
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
      //pinMode(LED_BUILTIN, OUTPUT);
      Serial.println();
      Serial.println("Startup Update Mode");
      connectTime = millis();
      while (millis() - connectTime <= 10000) {
        Serial2.println("$PFV," + soundMode + "," + soundIP + "*");
        delay(500);
      }
      //use mdns for host name resolution//
      if (!MDNS.begin(host)) { //http://esp32.local
        Serial.println("Error setting up MDNS responder!");
        while (1) {
          delay(1000);
        }
      }
      Serial.println("mDNS responder started");
      //return index page which is stored in serverIndex //
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
      //handling uploading firmware file //
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
          // flashing firmware to ESP//
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
}
