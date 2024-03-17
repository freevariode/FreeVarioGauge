void DrawMenu(int selectedMenuNumber, int level) {
  setDrawMenuLevel(selectedMenuNumber, level);
  requestMenuPaint = true;
  requestMenuFontPaint = true;
}

void setDrawMenuLevel(int selectedMenuNumber, int level) {
  requestDrawMenu = selectedMenuNumber;
  requestDrawMenuLevel = level;
}

void settingStartValueType () {
  nameSetting = "QNH";
  qnhWasUpdated = true;
}

void settingStandardValueType () {
  nameSetting = "MC";
  mcWasUpdated = true;
}

void requestFontRepaint() {
  requestMenuFontPaint = true;
}

void EncoderReader(void *p) {
  const int DEBOUNCE_DELAY = 50;
  const long TIME_SINCE_BOOT = 5000;

  pushButtonPressTime = NOT_SET;
  pushButtonIsLongpress = false;
  pushButtonIsShortpress = false;

  const int MENU_SPEED_TYP = 1;
  const int MENU_HIGHT_TYP = 2;
  const int MENU_VALUE_TYP = 3;

  const int MENU_VALUE_QNH = 1;
  const int MENU_VALUE_BUG = 2;
  const int MENU_VALUE_MUTE = 3;

  float encoderPosition = (float) - 999;
  float menuActiveSince = 0;                  // Will be updated in menu run

  pushButtonPressed = false;
  bool menuWasTriggered = false;
  bool subMenuTriggered = false;
  bool subMenuLevelTwoTriggered = false;

  int selectedMenu = MENU_SPEED_TYP;
  int selectedLevelTwoMenu = MENU_VALUE_QNH;
  while (true) {
    // READ ENCODER & BUTTONS / DIPS

    bool encoderLeft = false;
    bool encoderRight = false;
    bool encoderWasMoved = false;
    long encoderPositionNew = Vario_Enc.getCount();
    long timeSystemRuns = millis() - lastTimeBoot;

    if (encoderPositionNew > encoderPosition) {
      encoderRight = true;
      encoderWasMoved = true;
    }
    else if (encoderPositionNew < encoderPosition) {
      encoderLeft = true;
      encoderWasMoved = true;
    }
    else {
      encoderWasMoved = false;
    }
    encoderPosition = encoderPositionNew;

    if (menuWasTriggered && encoderRight) {
      selectedMenu ++;
    }
    else if  (menuWasTriggered && encoderLeft) {
      selectedMenu --;
    }

    if (selectedMenu > MENU_VALUE_TYP ) {
      selectedMenu = MENU_SPEED_TYP;
    }
    else if (selectedMenu < MENU_SPEED_TYP) {
      selectedMenu = MENU_VALUE_TYP;
    }

    if (digitalRead(VE_PB) == LOW) {
      pushButtonPressed = true;
    }
    else {
      pushButtonPressed = false;
    }

    // Check if Long or Shortpress
    // set Time when button was pressed else set to zero or do nothing when already pressed
    if (pushButtonPressed && pushButtonPressTime == NOT_SET) {
      vTaskDelay(DEBOUNCE_DELAY);
      pushButtonPressTime = millis();
    }
    /**
      else if (!pushButtonPressed && pushButtonPressTime != NOT_SET) {
      pushButtonPressTime = NOT_SET;
      }**/

    else if (pushButtonPressed && pushButtonPressTime != NOT_SET) {}
    else {
      pushButtonPressTime = NOT_SET;
      pushButtonPressed = false;
      vTaskDelay(DEBOUNCE_DELAY);
    }

    if (pushButtonPressTime != NOT_SET && (millis() - pushButtonPressTime >= LONGPRESS_TIME) && menuWasTriggered == false) {
      pushButtonIsLongpress = true;
    }
    else if (pushButtonPressTime != NOT_SET && (millis() - pushButtonPressTime >= SHORTPRESS_TIME_MIN) && (millis() - pushButtonPressTime < SHORTPRESS_TIME_MAX) && (menuWasTriggered == true || subMenuTriggered == true || subMenuLevelTwoTriggered == true)) {
      pushButtonIsShortpress = true;
    }

    if (digitalRead(STF_MODE) == LOW) {
      if (stf_mode != "Vario") {
        stfModeWasUpdate = true;
        if (digitalRead(STF_AUTO) == LOW) {
          Serial2.println("$POV,C,VAR*4F");  //Vario-Mode
        }
      }
      stf_mode = "Vario";
    }
    else if (digitalRead(STF_MODE) == HIGH) {
      if (stf_mode != "STF") {
        stfModeWasUpdate = true;
        if (digitalRead(STF_AUTO) == LOW) {
          Serial2.println("$POV,C,STF*4B");  //STF-Mode
        }
      }
      stf_mode = "STF";
    }

    // *********** CALCULATE ACTIONS ********************
    // **************************************************
    if (pushButtonIsLongpress && !pushButtonPressed && !menuWasTriggered && !subMenuTriggered && !encoderWasMoved && timeSystemRuns > TIME_SINCE_BOOT) {
      menuWasTriggered = true;
      menuActiveSince = millis(); // set time to now
      // Wait for release of pushButton
      while (digitalRead(VE_PB) == LOW) {}
      setDrawMenuLevel(selectedMenu, 1);
      requestFontRepaint();
      pushButtonIsLongpress = false;
    }

    else if (!pushButtonIsShortpress && !pushButtonIsLongpress && !pushButtonPressed && !menuWasTriggered && !subMenuTriggered && !subMenuLevelTwoTriggered && encoderWasMoved) {
      changeMCvalue(encoderRight);
    }

    else if (!pushButtonIsShortpress && !pushButtonIsLongpress && !pushButtonPressed && menuWasTriggered && !subMenuTriggered && encoderWasMoved) {

      if (selectedMenu == MENU_SPEED_TYP) {
        setDrawMenuLevel(selectedMenu, 1);
        requestFontRepaint();
      }
      else if (selectedMenu == MENU_HIGHT_TYP) {
        setDrawMenuLevel(selectedMenu, 1);
        requestFontRepaint();
      }
      else if (selectedMenu == MENU_VALUE_TYP) {
        setDrawMenuLevel(selectedMenu, 1);
        requestFontRepaint();
      }
      menuActiveSince = millis(); // set time to now
    }
    else if (menuWasTriggered && !subMenuTriggered && pushButtonIsShortpress) {
      // Wait for release of pushButton
      while (digitalRead(VE_PB) == LOW) {
        pushButtonIsShortpress = false;
      }
      if (selectedMenu == MENU_SPEED_TYP) {
        menuWasTriggered = false;
        subMenuTriggered = true;
        DrawMenu(selectedMenu, 2);
      }
      else if (selectedMenu == MENU_HIGHT_TYP) {
        menuWasTriggered = false;
        subMenuTriggered = true;
        DrawMenu(selectedMenu, 2);
      }
      else if (selectedMenu == MENU_VALUE_TYP) {
        menuWasTriggered = false;
        subMenuTriggered = true;
        settingStartValueType();
        DrawMenu(selectedMenu, 2);
      }
      menuActiveSince = millis(); // set time to now
    }
    else if (!menuWasTriggered && subMenuTriggered && !pushButtonIsShortpress && !pushButtonIsLongpress && !pushButtonPressed &&  encoderWasMoved ) {
      if (selectedMenu == MENU_SPEED_TYP) {
        changeSpeedOption();
      }
      else if (selectedMenu == MENU_HIGHT_TYP) {
        changeHighOption();
      }
      else if (selectedMenu == MENU_VALUE_TYP && encoderRight ) {
        changeValueOptionRight();
      }
      else if (selectedMenu == MENU_VALUE_TYP && encoderLeft ) {
        changeValueOptionLeft();
      }
      menuActiveSince = millis(); // set time to now
    }

    else if (!menuWasTriggered && subMenuTriggered && pushButtonIsShortpress) {
      // Wait for release of pushButton
      while (digitalRead(VE_PB) == LOW) {
        pushButtonIsShortpress = false;
      }

      if (selectedMenu == MENU_SPEED_TYP || selectedMenu  == MENU_HIGHT_TYP) {
        subMenuTriggered = false;
        selectedMenu = MENU_SPEED_TYP;
        setDrawMenuLevel(selectedMenu, 0);
        requestFontRepaint();
      }
      else if (selectedMenu == MENU_VALUE_TYP) {
        menuActiveSince = millis(); // set time to now
        subMenuTriggered = false;
        subMenuLevelTwoTriggered = true;
        setDrawMenuLevel(selectedMenu, 3);
        requestFontRepaint();
      }
    }

    else if (!menuWasTriggered && !subMenuTriggered && !pushButtonIsShortpress && !pushButtonIsLongpress  && !pushButtonPressed && subMenuLevelTwoTriggered && encoderWasMoved) {
      changeLevelTwoMenu(encoderRight);
      changeLevelTwoMenuTurn(encoderWasMoved);
      menuActiveSince = millis(); // set time to now
    }

    else if (!menuWasTriggered && !subMenuTriggered && pushButtonIsShortpress && subMenuLevelTwoTriggered && !encoderWasMoved) {
      // Wait for release of pushButton
      while (digitalRead(VE_PB) == LOW) {
        pushButtonIsShortpress = false;
      }
      subMenuLevelTwoTriggered = false;
      settingStandardValueType();
      selectedMenu = MENU_SPEED_TYP;
      setDrawMenuLevel(selectedMenu, 0);
      requestFontRepaint();
    }

    // check run time in menu and exit if time > 10000
    if ((millis() - menuActiveSince) > 10000 && subMenuLevelTwoTriggered) {
      subMenuLevelTwoTriggered = false;
      settingStandardValueType();
      selectedMenu = MENU_SPEED_TYP;
      setDrawMenuLevel(selectedMenu, 0);
      requestFontRepaint();
    }
    else if ((millis() - menuActiveSince) > 10000 && subMenuTriggered) {
      subMenuTriggered = false;
      settingStandardValueType();
      selectedMenu = MENU_SPEED_TYP;
      setDrawMenuLevel(selectedMenu, 0);
      requestFontRepaint();
    }
    else if ((millis() - menuActiveSince) > 10000 && menuWasTriggered) {
      menuWasTriggered = false;
      settingStandardValueType();
      selectedMenu = MENU_SPEED_TYP;
      setDrawMenuLevel(selectedMenu, 0);
      requestFontRepaint();
    }
  }
}
