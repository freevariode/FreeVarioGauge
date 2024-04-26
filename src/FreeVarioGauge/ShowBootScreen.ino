void showBootScreen(String versionString) {
  String waitingMessage = "Waiting for Data ...";
  String dataString;
  long showVersionTime = millis();
  long ChangeBaud = millis();
  int serial2IsReady = 0;
  int baudDetect = 0;
  unsigned long loopTime = 5000;
  tft.loadFont("micross20");
  TFT_eSprite bootSprite = TFT_eSprite(&tft);
  bootSprite.loadFont("micross20_boot");
  bootSprite.createSprite(195, 25);
  bootSprite.fillSprite(TFT_WHITE);
  bootSprite.setCursor(0, 2);
  bootSprite.setTextColor(TFT_DARKGREY, TFT_BLACK);
  tft.fillScreen(TFT_WHITE);
  drawLogo();
  bootSprite.println(versionString);
  bootSprite.pushSprite(40, 245);
  bootSprite.deleteSprite();
  lastTimeBoot = millis();
  changeMode = digitalRead(STF_MODE);
  oldChangeMode = changeMode;
  while (millis() - showVersionTime <= loopTime) {
    changeMode = digitalRead(STF_MODE);
    if (oldChangeMode != changeMode) {
      updatemode = true;
      loopTime = millis() + 500;
      bootSprite.loadFont("micross20_boot");
      bootSprite.createSprite(195, 25);
      bootSprite.fillSprite(TFT_WHITE);
      bootSprite.setCursor(0, 2);
      bootSprite.setTextColor(TFT_DARKGREY, TFT_BLACK);
      tft.fillScreen(TFT_WHITE);
      tft.setWindow(40, 55, 40 + 193, 55 + 155);
      tft.pushColors(logoOV, 194 * 156);
      bootSprite.println("starting Update Mode");
      bootSprite.pushSprite(40, 245);
      bootSprite.deleteSprite();
    }
    oldChangeMode = changeMode;
  }
  if (updatemode == false) {
    WiFi.mode(WIFI_OFF);
    bootSprite.createSprite(195, 25);
    bootSprite.fillSprite(TFT_WHITE);
    bootSprite.setCursor(0, 2);
    bootSprite.println(waitingMessage);
    bootSprite.pushSprite(40, 245);
    bootSprite.deleteSprite();

    //********************************************************
    //****  Waiting until XCSoar delivers correct values  ****
    //********************************************************
    do {
      if (Serial2.available()) {
        char serialString = Serial2.read();
        if (serialString == '$') {
          while (serialString != 10) {
            dataString += serialString;
            serialString = Serial2.read();
          }
          Serial.print("dataString: ");
          Serial.println(dataString);
        } else {
          if ((!SourceIsXCSoar && !SourceIsLarus) && (baudDetect == 0) && (millis() - ChangeBaud <= 5000)) {
            Serial2.end();
            Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
            baudDetect = 1;
          } else if ((!SourceIsXCSoar && !SourceIsLarus) && (baudDetect == 1) && (millis() - ChangeBaud > 5000) && (millis() - ChangeBaud <= 10000)) {
            Serial2.end();
            Serial2.begin(38400, SERIAL_8N1, RXD2, TXD2);
            baudDetect = 0;
          } else if ((!SourceIsXCSoar && !SourceIsLarus) && (millis() - ChangeBaud > 10000)) {
            ChangeBaud = millis();
          }
        }
        if ((dataString.startsWith("$PFV")) || (dataString.startsWith("$PLAR"))) {
          serial2IsReady = 1;
        }
        dataString = "";
      }
    } while (serial2IsReady == 0);  //0 = waiting for XCSoar, 1 = start without waiting
    bootSprite.unloadFont();
    tft.fillScreen(TFT_BLACK);
    lastTimeReady = millis();
    showBootscreen = false;
  }
}

void drawLogo() {

  fs::File bmpFS;

  if (SPIFFS.exists("/FreeVario_194x156.bmp")) {
    bmpFS = SPIFFS.open("/FreeVario_194x156.bmp", "r");
    drawBmp(bmpFS, 40, 55);
    bmpFS.close();
  } else {
    Serial.println("Logo not found. Using deprecated LogoOV. Update of SPIFFS required.");
    tft.setWindow(40, 55, 40 + 193, 55 + 155);
    tft.pushColors(logoOV, 194 * 156);
    return;
  }


}
