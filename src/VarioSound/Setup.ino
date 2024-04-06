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
  ChangeBaud = millis();
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
