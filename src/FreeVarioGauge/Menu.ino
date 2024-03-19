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

void Menu () {
  const long TIME_SINCE_BOOT = 5000;

  long timeSystemRuns = millis() - lastTimeBoot;

  //******************************
  //****  Calculate  Actions  ****
  //******************************

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
  if (pushButtonIsLongpress && !pushButtonPressed && !menuWasTriggered && !subMenuTriggered && !subMenuLevelTwoTriggered && !encoderWasMoved && timeSystemRuns > TIME_SINCE_BOOT) {
    menuWasTriggered = true;
    menuActiveSince = millis(); // set time to now
    // Wait for release of pushButton
    while (digitalRead(VE_PB) == LOW) {}
    setDrawMenuLevel(selectedMenu, 1);
    requestFontRepaint();
    pushButtonIsLongpress = false;
  }

  else if (!pushButtonPressed && !menuWasTriggered && !subMenuTriggered && !subMenuLevelTwoTriggered && encoderWasMoved) {
    changeMCvalue(encoderRight);
  }
  else if (!pushButtonPressed && menuWasTriggered && !subMenuTriggered && encoderWasMoved) {
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
