void ValueRefresh(void *parameter) {
  String valueSetting, valueSpeed, valueHight;
  String unitSpeed, unitHight, unitSetting;
  while (showBootscreen) {
    vTaskDelay(1000);
  }
  background.setColorDepth(8);
  background.createSprite(240, 320);
  background.fillSprite(TFT_BLACK);
  while (true) {
    if (nameHight == "MSL") {
      valueHight = valueHigAsString;
    }
    else if (nameHight == "AGL") {
      valueHight = valueHagAsString;
    }
    else if (nameHight == "FL") {
      valueHight = valueFLAsString;
    }
    if (nameSpeed == "GS") {
      valueSpeed = valueGrsAsString;
    }
    else if (nameSpeed == "TAS") {
      valueSpeed = valueTasAsString;
    }

    //void DrawText(TFT_eSprite fontOfName, TFT_eSprite fontOfInfo, String spriteName, String value,
    //String unit, int spriteNameWidth, int spriteValueHight, int spriteValueWidth, int spriteunitWidth, int x, int y)
    background.setPivot(154, 160);
    background.fillSprite(TFT_BLACK);
    ArcRefresh();

    //*****************************************
    //****  Calculate and Refresh Needles  ****
    //*****************************************
    if ((valueWindAsInt == 1) && (valueAwdAsFloat != -1000)) {

      if (valueAwdAsFloat < 0) {
        valueAwdAsFloat + 360;
      }

      if (valueCwdAsFloat < 0) {
        valueCwdAsFloat + 360;
      }

      needleBlue.createSprite(20, 130);
      needleBlue.drawWedgeLine(11, 0, 11, 130, 1, 10, BLUE);
      needleBlue.pushRotated(&background, valueAwdAsFloat, TFT_BLACK);

      if (valueCwdAsFloat != -1000) {
        needleGreen.createSprite(20, 130);
        needleGreen.drawWedgeLine(11, 0, 11, 130, 1, 10, TFT_GREEN);
        needleGreen.pushRotated(&background, valueCwdAsFloat, TFT_BLACK);
      }
    }

    //****************************
    //****  Draw Menu Frames  ****
    //****************************
    if (requestMenuPaint) {
      if (requestDrawMenuLevel == 2) {
        if (requestDrawMenu == 1) {
          if ((valueWindAsInt == 1) && (valueAwdAsFloat != -1000)) {
            offset = 0;
          }
          else {
            offset = -18;
          }
          drawRectangle.createSprite(170, 41);
          drawRectangle.fillSprite(TFT_BLACK);
          drawRectangle.drawRect(2, 2, 166, 37, TFT_RED);
          drawRectangle.drawRect(1, 1, 168, 39, TFT_RED);
          drawRectangle.drawRect(0, 0, 170, 41, TFT_RED);
          drawRectangle.pushToSprite(&background, 55, 126 + offset, TFT_BLACK);
          drawRectangle.deleteSprite();
        }

        if (requestDrawMenu == 2) {
          if ((valueWindAsInt == 1) && (valueAwdAsFloat != -1000)) {
            offset = 0;
          }
          else {
            offset = -12;
          }
          drawRectangle.createSprite(151, 41);
          drawRectangle.fillSprite(TFT_BLACK);
          drawRectangle.drawRect(2, 2, 148, 37, TFT_RED);
          drawRectangle.drawRect(1, 1, 150, 39, TFT_RED);
          drawRectangle.drawRect(0, 0, 152, 41, TFT_RED);
          drawRectangle.pushToSprite(&background, 73, 164 + offset, TFT_BLACK);
          drawRectangle.deleteSprite();
        }

        if (requestDrawMenu == 3) {
          if ((valueWindAsInt == 1) && (valueAwdAsFloat != -1000)) {
            offset = 0;
          }
          else {
            offset = -6;
          }
          drawRectangle.createSprite(151, 41);
          drawRectangle.fillSprite(TFT_BLACK);
          drawRectangle.drawRect(2, 2, 147, 37, TFT_RED);
          drawRectangle.drawRect(1, 1, 149, 39, TFT_RED);
          drawRectangle.drawRect(0, 0, 151, 41, TFT_RED);
          drawRectangle.pushToSprite(&background, 74, 201 + offset, TFT_BLACK);
          drawRectangle.deleteSprite();
        }
      }
      else if (requestDrawMenuLevel == 3) {
        drawRectangle.createSprite(150, 3);
        drawRectangle.fillSprite(TFT_BLACK);
        drawRectangle.drawLine(0, 2, 150, 2, TFT_RED);;
        drawRectangle.drawLine(0, 1, 150, 1, TFT_RED);
        drawRectangle.drawLine(0, 0, 150, 0, TFT_RED);
        drawRectangle.pushToSprite(&background, 74, 241 + offset, TFT_BLACK);
        drawRectangle.deleteSprite();
      }
    }

    //**************************
    //****  Refresh Values  ****
    //**************************

    if ((valueWindAsInt == 1) && (valueAwdAsFloat != -1000)) {
      offset = 0;
    }
    else {
      offset = -24;
    }
    if (stf_mode_state == 0) {
      if ( xSemaphoreTake( xTFTSemaphore, ( TickType_t ) 5 ) == pdTRUE ) {
        DrawText(nameOfField, TFT_WHITE, "large", "Avg.", valueVaaAsString, 34, 40, 99, 88, 84 + offset); //+1
        xSemaphoreGive(xTFTSemaphore);
      }
    }

    if (stf_mode_state == 1) {
      if ( xSemaphoreTake( xTFTSemaphore, ( TickType_t ) 5 ) == pdTRUE )
      {
        DrawText(nameOfField, TFT_WHITE, "large", "Net.", valueVanAsString, 34, 40, 99, 88, 84 + offset);
        xSemaphoreGive(xTFTSemaphore);
      }
    }

    if ((valueWindAsInt == 1) && (valueAwdAsFloat != -1000)) {
      if (valueCwsAsFloat >= 99) {
        valueCwsAsString = "99";
      }
      if (valueAwsAsFloat >= 99) {
        valueAwsAsString = "99";
      }
      if ( xSemaphoreTake( xTFTSemaphore, ( TickType_t ) 5 ) == pdTRUE )
      {
        DrawText(nameOfField, TFT_WHITE, "small", "Wind", "", 36, 25, 0, 105, 45);
        xSemaphoreGive(xTFTSemaphore);
      }
      if (valueCwdAsFloat != -1000) {
        if ( xSemaphoreTake( xTFTSemaphore, ( TickType_t ) 5 ) == pdTRUE )
        {
          DrawText(nameOfField, TFT_GREEN, "small", "", valueCwsAsString, 0, 25, 40, 140, 40);
          //avsWasUpdated = true;
          //casWasUpdated = false;
          xSemaphoreGive(xTFTSemaphore);
        }
      }
      if ( xSemaphoreTake( xTFTSemaphore, ( TickType_t ) 5 ) == pdTRUE )
      {
        DrawText(nameOfField, BLUE, "small", "", valueAwsAsString, 0, 25, 40, 176, 40);
        xSemaphoreGive(xTFTSemaphore);
      }
    }

    if ((valueWindAsInt == 1) && (valueAwdAsFloat != -1000)) {
      offset = 0;
    }
    else {
      offset = -18;
    }

    if ( xSemaphoreTake( xTFTSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      if ((requestDrawMenuLevel == 1 ) && (requestDrawMenu == 1) || (requestDrawMenuLevel == 2 ) && (requestDrawMenu == 1)) {
        DrawText(nameOfField, TFT_RED, "small", nameSpeed, valueSpeed + " km/h", 28, 25, 130, 63, 136 + offset); //+18
      }
      else {
        DrawText(nameOfField, TFT_WHITE, "small", nameSpeed, valueSpeed + " km/h", 28, 25, 130, 63, 136 + offset);
      }
      xSemaphoreGive(xTFTSemaphore);
    }


    if ((valueWindAsInt == 1) && (valueAwdAsFloat != -1000)) {
      offset = 0;
    }
    else {
      offset = -12;
    }

    if (!SourceIsLarus || (SourceIsLarus && nameHight != "FL")) {
      if ( xSemaphoreTake( xTFTSemaphore, ( TickType_t ) 5 ) == pdTRUE )
      {

        if ((requestDrawMenuLevel == 1 ) && (requestDrawMenu == 2) || (requestDrawMenuLevel == 2 ) && (requestDrawMenu == 2)) {
          DrawText(nameOfField, TFT_RED, "small", nameHight, valueHight + " m", 31, 25, 108, 82, 173 + offset); //+12
        }
        else {
          DrawText(nameOfField, TFT_WHITE, "small", nameHight, valueHight + " m", 31, 25, 108, 82, 173 + offset);
        }
        xSemaphoreGive(xTFTSemaphore);
      }
    }
    else if (SourceIsLarus && nameHight == "FL") {
      if ( xSemaphoreTake( xTFTSemaphore, ( TickType_t ) 5 ) == pdTRUE )
      {

        if ((requestDrawMenuLevel == 1 ) && (requestDrawMenu == 2) || (requestDrawMenuLevel == 2 ) && (requestDrawMenu == 2)) {
          DrawText(nameOfField, TFT_RED, "small", nameHight, valueHight + "", 31, 25, 108, 82, 173 + offset); //+12
        }
        else {
          DrawText(nameOfField, TFT_WHITE, "small", nameHight, valueHight + "", 31, 25, 108, 82, 173 + offset);
        }
        xSemaphoreGive(xTFTSemaphore);
      }
    }

    if ((valueWindAsInt == 1) && (valueAwdAsFloat != -1000)) {
      offset = 0;
    }
    else {
      offset = -6;
    }
    if (nameSetting == "MC") {
      if ( xSemaphoreTake( xTFTSemaphore, ( TickType_t ) 5 ) == pdTRUE )
      {
        valueSetting = valueMacAsString;
        if ((requestDrawMenuLevel == 1 ) && (requestDrawMenu == 3)) {
          DrawText(nameOfField, TFT_RED, "small", "MC", valueSetting + " m/s", 24, 25, 114, 83, 210 + offset); //+6
        }
        else {
          DrawText(nameOfField, TFT_WHITE, "small", "MC", valueSetting + " m/s", 24, 25, 114, 83, 210 + offset);
        }
        xSemaphoreGive(xTFTSemaphore);
      }
    }

    else if (nameSetting == "Time") {
      int TimeHour = UTCHour.toInt() + TimeDifference;
      String Time = String(TimeHour) + ":" + UTCMinute;
      String TimeType = "UTC";
      if (TimeDifference != 0) {
        TimeType = "Local";
      }

      if ( xSemaphoreTake( xTFTSemaphore, ( TickType_t ) 5 ) == pdTRUE )
      {
        if ((requestDrawMenuLevel == 1 ) && (requestDrawMenu == 3)) {
          DrawText(nameOfField, TFT_RED, "small", TimeType, Time, 35, 25, 103, 83, 210 + offset);
        }
        else {
          DrawText(nameOfField, TFT_WHITE, "small", TimeType, Time, 35, 25, 103, 83, 210 + offset);
        }
        xSemaphoreGive(xTFTSemaphore);
      }
    }

    else if (nameSetting == "QNH") {
      if ( xSemaphoreTake( xTFTSemaphore, ( TickType_t ) 5 ) == pdTRUE )
      {
        valueSetting = valueQnhAsString;
        if ((requestDrawMenuLevel == 2 ) && (requestDrawMenu == 3) || (requestDrawMenuLevel == 3 ) && (requestDrawMenu == 3)) {
          DrawText(nameOfField, TFT_RED, "small", "QNH", valueSetting, 50, 25, 88, 83, 210 + offset);
        }
        else {
          DrawText(nameOfField, TFT_WHITE, "small", "QNH", valueSetting, 50, 25, 88, 83, 210 + offset);
        }
        xSemaphoreGive(xTFTSemaphore);
      }
    }

    else if (nameSetting == "Bug") {
      if ( xSemaphoreTake( xTFTSemaphore, ( TickType_t ) 5 ) == pdTRUE )
      {
        valueSetting = valueBugAsString;
        if ((requestDrawMenuLevel == 2 ) && (requestDrawMenu == 3) || (requestDrawMenuLevel == 3 ) && (requestDrawMenu == 3)) {
          DrawText(nameOfField, TFT_RED, "small", "Bug", valueSetting + " %", 39, 25, 99, 83, 210 + offset);
        }
        else {
          DrawText(nameOfField, TFT_WHITE, "small", "Bug", valueSetting + " %", 39, 25, 99, 83, 210 + offset);
        }
        xSemaphoreGive(xTFTSemaphore);
      }
    }

    else if (nameSetting == "ATTEN") {
      if ( xSemaphoreTake( xTFTSemaphore, ( TickType_t ) 5 ) == pdTRUE )
      {
        valueSetting = valueAttenAsString;
        if ((requestDrawMenuLevel == 2 ) && (requestDrawMenu == 3) || (requestDrawMenuLevel == 3 ) && (requestDrawMenu == 3)) {
          DrawText(nameOfField, TFT_RED, "small", "ATT", valueSetting, 39, 25, 99, 83, 210 + offset);
        }
        else {
          DrawText(nameOfField, TFT_WHITE, "small", "ATT", valueSetting, 39, 25, 99, 83, 210 + offset);
        }
        xSemaphoreGive(xTFTSemaphore);
      }
    }

    else if (nameSetting == "Mute") {
      if ( xSemaphoreTake( xTFTSemaphore, ( TickType_t ) 5 ) == pdTRUE )
      {
        valueSetting = valueMuteAsString;
        if ((requestDrawMenuLevel == 2 ) && (requestDrawMenu == 3) || (requestDrawMenuLevel == 3 ) && (requestDrawMenu == 3)) {
          DrawText(nameOfField, TFT_RED, "small", "Mute", valueSetting, 39, 25, 99, 83, 210 + offset);
        }
        else {
          DrawText(nameOfField, TFT_WHITE, "small", "Mute", valueSetting, 39, 25, 99, 83, 210 + offset);
        }
        xSemaphoreGive(xTFTSemaphore);
      }
    }

    else if (nameSetting == "Wind") {
      if ( xSemaphoreTake( xTFTSemaphore, ( TickType_t ) 5 ) == pdTRUE )
      {
        valueSetting = valueWindAsString;
        if ((requestDrawMenuLevel == 2 ) && (requestDrawMenu == 3) || (requestDrawMenuLevel == 3 ) && (requestDrawMenu == 3)) {
          DrawText(nameOfField, TFT_RED, "small", "Wind", valueSetting, 39, 25, 99, 83, 210 + offset);
        }
        else {
          DrawText(nameOfField, TFT_WHITE, "small", "Wind", valueSetting, 39, 25, 99, 83, 210 + offset);
        }
        xSemaphoreGive(xTFTSemaphore);
      }
    }

    else if (nameSetting == "STF") {
      if ( xSemaphoreTake( xTFTSemaphore, ( TickType_t ) 5 ) == pdTRUE )
      {
        valueSetting = valueSTFAsString;
        if ((requestDrawMenuLevel == 2 ) && (requestDrawMenu == 3) || (requestDrawMenuLevel == 3 ) && (requestDrawMenu == 3)) {
          DrawText(nameOfField, TFT_RED, "small", "STF", valueSetting, 39, 25, 99, 83, 210 + offset);
        }
        else {
          DrawText(nameOfField, TFT_WHITE, "small", "STF", valueSetting, 39, 25, 99, 83, 210 + offset);
        }
        xSemaphoreGive(xTFTSemaphore);
      }
    }

    else if (nameSetting == "TimeDifference") {
      if ( xSemaphoreTake( xTFTSemaphore, ( TickType_t ) 5 ) == pdTRUE )
      {
        valueSetting = String(TimeDifference);
        if (TimeDifference >= 0) {
          valueSetting = "+" + String(TimeDifference);
        }
        if ((requestDrawMenuLevel == 2 ) && (requestDrawMenu == 3) || (requestDrawMenuLevel == 3 ) && (requestDrawMenu == 3)) {
          DrawText(nameOfField, TFT_RED, "small", "Diff.", valueSetting, 39, 25, 99, 83, 210 + offset);
        }
        else {
          DrawText(nameOfField, TFT_WHITE, "small", "Diff.", valueSetting, 39, 25, 99, 83, 210 + offset);
        }
        xSemaphoreGive(xTFTSemaphore);
      }
    }

    if (!SourceIsLarus) {
      if ( xSemaphoreTake( xTFTSemaphore, ( TickType_t ) 5 ) == pdTRUE )
      {
        DrawText(nameOfField, TFT_WHITE, "small", "Mode", stf_mode, 38, 25, 75, 105, 248);
        xSemaphoreGive(xTFTSemaphore);
      }
    }
    else {
      if ( xSemaphoreTake( xTFTSemaphore, ( TickType_t ) 5 ) == pdTRUE )
      {
        DrawText(nameOfField, TFT_WHITE, "small", "Bat.", valueVoltageAsString + " V", 25, 45, 90, 105, 248);
        xSemaphoreGive(xTFTSemaphore);
      }
    }

    vTaskDelay(15);
    background.pushSprite(0, 0);
  }
  if (requestMenuPaint) {
    requestMenuPaint = false;
  }
}
