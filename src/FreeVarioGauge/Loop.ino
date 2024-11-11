void loop() {
  if (showBootscreen) {
    UpdateMode();
  }
  else {
    EncoderReader();
    Menu ();
    if (millis() - stayAlive >= 9000) {
      String muteStr = ("$PFV,S,S," + String(valueMuteAsInt) + "*");
      int checksum = calculateChecksum(muteStr);
      Serial2.printf("%s%X\n", muteStr.c_str(), checksum);
      stayAlive = millis();
    }
  }
}
