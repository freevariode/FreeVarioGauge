//************************************
//****  Draw ValueBoxes and Data  ****
//************************************
void DrawText(TFT_eSprite fontOfName, uint32_t color, String infoType, String spriteName, String value, int spriteNameWidth, int spriteValueHight, int spriteValueWidth, int x, int y) {
  if (loadMenuFont) {
    Name.loadFont("micross15");
    loadMenuFont = false;
  }
  Name.createSprite(spriteNameWidth, 25);
  Name.setTextColor(color, color);
  Name.fillSprite(BLACK);
  Name.fillSprite(TFT_BLACK);
  Name.setTextDatum(TR_DATUM);
  Name.drawString(spriteName, spriteNameWidth, 2);
  Name.pushToSprite(&background, x, y, TFT_BLACK);
  Name.deleteSprite();

  if (infoType == "small") {
    if (k == 1) {
      smallFont.loadFont("micross30");
    }
    smallFont.createSprite(spriteValueWidth, spriteValueHight);
    smallFont.setTextColor(color, BLACK);
    smallFont.fillSprite(BLACK);
    smallFont.fillSprite(TFT_BLACK);
    smallFont.setTextDatum(TR_DATUM);
    smallFont.drawString(value, spriteValueWidth, 2);
    smallFont.pushToSprite(&background, x + spriteNameWidth, y, TFT_BLACK);
    smallFont.deleteSprite();
  }

  else if (infoType == "large") {
    fontOfName.loadFont("micross50");
    fontOfName.createSprite(spriteValueWidth, spriteValueHight);
    fontOfName.setTextColor(color, BLACK);
    fontOfName.fillSprite(BLACK);
    fontOfName.fillSprite(TFT_BLACK);
    fontOfName.setTextDatum(TR_DATUM);
    fontOfName.drawString(value, spriteValueWidth, 2);
    fontOfName.pushToSprite(&background, x + spriteNameWidth, y, TFT_BLACK);
    fontOfName.deleteSprite();

    //fontOfName.createSprite(spriteunitWidth + 5, 25);
  }
  k++;
  if (k == 7) {
    k = 0;
  }
}
