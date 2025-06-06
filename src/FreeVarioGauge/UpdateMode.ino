void UpdateMode() {
  if (updatemode == true) {

    pushButtonPressTime = millis();
    if (Wificount == 0) {
      updateScreen();
      Wificount = 1;
      WiFi.softAP(ssid, password);
      delay(100);
      IPAddress Ip(192, 168, 2, 1);  //setto IP Access Point same as gateway
      IPAddress NMask(255, 255, 255, 0);
      WiFi.softAPConfig(Ip, Ip, NMask);
      IPAddress myIP = WiFi.softAPIP();
      server.begin();
      displayIP = myIP.toString();
      if (displayIP != "" && displayIP != "0.0.0.0") {
        displayMode = "Ready to connect";
      } else {
        displayMode = "WiFi Error";
      }
      delay(10);
      Serial.println();
      Serial.println("Startup");

      while ((soundIP == "") || (soundIP == "0.0.0.0")) {
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
            int pos1 = DataString.indexOf(',');                  //finds the place of the first,
            String variable = DataString.substring(0, pos1);     //captures the first record
            int pos2 = DataString.indexOf('*', pos1 + 1);        //finds the place of *
            String wert = DataString.substring(pos1 + 1, pos2);  //captures the second record

            if (variable == "Ready") {
              soundIP = wert;
              soundMode = "Ready to connect";
            }

            if (variable == "Error") {
              soundIP = wert;
              soundMode = "WiFi Error";
            }
          }
          DataString = "";
          vTaskDelay(50);
        }
      }

      //use mdns for host name resolution
      if (!MDNS.begin(host)) {  //http://esp32.local
        Serial.println("Error setting up MDNS responder!");
        while (1) {
          delay(1000);
        }
      }
      Serial.println("mDNS responder started");
      //return index page which is stored in serverIndex //
      server.on("/", HTTP_GET, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", "/display-login.html");
      });
      server.on("/serverIndex", HTTP_GET, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", "/display-update.html");
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
      server.on(
        "/update", HTTP_POST, []() {
          server.sendHeader("Connection", "close");
          server.send((Update.hasError()) ? 422 : 200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
          delay(1000);
          ESP.restart();
        },
        []() {
          HTTPUpload& upload = server.upload();
          if (upload.status == UPLOAD_FILE_START) {
            String filename = upload.filename;
            if (filename.endsWith(".bin")) {
              Serial.printf("Update: %s\n", filename.c_str());
              if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {  //start with max available size
                Update.printError(Serial);
              }
            } else if (filename.indexOf("spiffs") != -1) {
              Serial.printf("Update SPIFFS: %s\n", filename.c_str());
              size_t spiffsSize = SPIFFS.totalBytes();    // Get the size of the SPIFFS partition
              if (!Update.begin(spiffsSize, U_SPIFFS)) {  // Start update of SPIFFS partition
                Update.printError(Serial);
              }
            } else {
              Serial.println("Error: filename must end with '.bin' or contain 'spiffs'");
            }
          } else if (upload.status == UPLOAD_FILE_WRITE) {
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
              Update.printError(Serial);
            }
          } else if (upload.status == UPLOAD_FILE_END) {
            if (Update.end(true)) {  //true to set the size to the current progress
              Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
            } else {
              Update.printError(Serial);
            }
          }
        });
      updateScreen();
    }
    server.handleClient();
    delay(1);
  }
}
