void updateScreen() {
  String modeMessage = "Update Mode";
  tft.loadFont("micross20");
  TFT_eSprite bootSprite = TFT_eSprite(&tft);
  bootSprite.loadFont("micross20_boot");
  bootSprite.createSprite(195, 25);
  bootSprite.fillSprite(TFT_WHITE);
  bootSprite.setCursor(0, 2);
  bootSprite.setTextColor(TFT_DARKGREY, TFT_BLACK);
  tft.fillScreen(TFT_WHITE);
  bootSprite.createSprite(195, 25);
  bootSprite.fillSprite(TFT_WHITE);
  bootSprite.setCursor(0, 2);
  bootSprite.println(modeMessage);
  bootSprite.pushSprite(85, 35);
  bootSprite.deleteSprite();
  bootSprite.createSprite(195, 25);
  bootSprite.fillSprite(TFT_WHITE);
  bootSprite.setCursor(0, 2);
  bootSprite.println("FV_Displayboard:");
  bootSprite.pushSprite(35, 85);
  bootSprite.deleteSprite();
  bootSprite.createSprite(195, 25);
  bootSprite.fillSprite(TFT_WHITE);
  bootSprite.setCursor(0, 2);
  bootSprite.println(displayMode);
  bootSprite.pushSprite(35, 105);
  bootSprite.deleteSprite();
  bootSprite.createSprite(195, 25);
  bootSprite.fillSprite(TFT_WHITE);
  bootSprite.setCursor(0, 2);
  bootSprite.println("IP: " + displayIP);
  bootSprite.pushSprite(35, 125);
  bootSprite.deleteSprite();
  bootSprite.createSprite(195, 25);
  bootSprite.fillSprite(TFT_WHITE);
  bootSprite.setCursor(0, 2);
  bootSprite.println("FV_Soundboard:");
  bootSprite.pushSprite(35, 175);
  bootSprite.deleteSprite();
  bootSprite.createSprite(195, 25);
  bootSprite.fillSprite(TFT_WHITE);
  bootSprite.setCursor(0, 2);
  bootSprite.println(soundMode);
  bootSprite.pushSprite(35, 195);
  bootSprite.deleteSprite();
  bootSprite.createSprite(195, 25);
  bootSprite.fillSprite(TFT_WHITE);
  bootSprite.setCursor(0, 2);
  bootSprite.println("IP: " + soundIP);
  bootSprite.pushSprite(35, 215);
  bootSprite.deleteSprite();
}
