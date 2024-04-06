void setup() {
  server.on("/SoftwareVersion", []() {
    server.send(200, "application/json",  "\"" + SOFTWARE_VERSION + "\"");
  });

  //***********************************************
  //****  Enable the weak pull down resistors  ****
  //***********************************************
  ESP32Encoder::useInternalWeakPullResistors = DOWN;

  if ( xTFTSemaphore == NULL )
  { xTFTSemaphore = xSemaphoreCreateMutex();
    if ( ( xTFTSemaphore ) != NULL )
      xSemaphoreGive( ( xTFTSemaphore ) );
  }

  tft.init();
  tft.setRotation(0);
  //************************************
  //****  set starting count value  ****
  //************************************
  Vario_Enc.attachHalfQuad(23, 32);
  Vario_Enc.setCount(16380);
  pinMode(VE_PB, INPUT_PULLUP);
  Serial.begin(115200, SERIAL_8N1);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  SPIFFSstart();
  server.serveStatic("/B612-Bold.ttf", SPIFFS, "/B612-Bold.ttf");
  server.serveStatic("/B612-Regular.ttf", SPIFFS, "/B612-Regular.ttf");
  server.serveStatic("/style.css", SPIFFS, "/style.css");
  server.serveStatic("/script.js", SPIFFS, "/script.js");
  server.serveStatic("/serverIndex", SPIFFS, "/display-update.html");
  server.serveStatic("/", SPIFFS, "/display-login.html");

  xTaskCreate(SerialScan, "Serial Scan", 3000, NULL, 30, &SerialScanTask);
  xTaskCreate(ValueRefresh, "Value Refresh", 4500, NULL, 200, &TaskValueRefresh);
  //xTaskCreate(printWatermark, "Print Watermark", 2048, NULL, 1, NULL);     //Task zum Messen des benötigten Stacks der anderen Tasks

  prefs.begin("settings", false);
  valueMuteAsInt = prefs.getUInt("Mute", 1);
  valueAttenAsInt = prefs.getUInt("ATTEN", 2);
  valueWindAsInt = prefs.getUInt("Wind", 1);
  nameSpeed = prefs.getString("nameSpeed", "GS");
  nameHight = prefs.getString("nameHight", "MSL");
  prefs.end();

  valueAttenAsString = String(valueAttenAsInt);

  if (valueMuteAsInt == 0) {
    valueMuteAsString = "OFF";
  }
  else if (valueMuteAsInt == 1) {
    valueMuteAsString = "ON";
  }

  if (valueWindAsInt == 0) {
    valueWindAsString = "OFF";
  }
  else if (valueWindAsInt == 1) {
    valueWindAsString = "ON";
  }
  showBootScreen(SOFTWARE_VERSION);
}
