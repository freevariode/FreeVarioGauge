void DrawMenu(int selectedMenuNumber, int level) {
  setDrawMenuLevel(selectedMenuNumber, level);
  requestMenuPaint = true;
}

void setDrawMenuLevel(int selectedMenuNumber, int level) {
  requestDrawMenu = selectedMenuNumber;
  requestDrawMenuLevel = level;
}

void settingStartValueType () {
  if (!SourceIsLarus) {
    nameSetting = "QNH";
  }
  else {
    nameSetting = "Mute";
  }
}

void settingStandardValueType () {
  if (!SourceIsLarus) {
    nameSetting = "MC";
  }
  else {
    nameSetting = "Mute";
  }
}

void Menu () {
  const long TIME_SINCE_BOOT = 5000;
  long timeSystemRuns = millis() - lastTimeBoot;

  //******************************
  //****  Calculate  Actions  ****
  //******************************

  if (menuWasTriggered && encoderRight && !SourceIsLarus) {
    selectedMenu ++;
  }
  else if  (menuWasTriggered && encoderLeft && !SourceIsLarus) {
    selectedMenu --;
  }

  if (selectedMenu > MENU_VALUE_TYP && !SourceIsLarus) {
    selectedMenu = MENU_SPEED_TYP;
  }
  else if (selectedMenu < MENU_SPEED_TYP && !SourceIsLarus) {
    selectedMenu = MENU_VALUE_TYP;
  }
  else if (menuWasTriggered  && SourceIsLarus) {  //beim Larus werden das Speed- und das Hightmenü übersprungen. Es geht direkt zu den Einstellungen von QNH, Bug. etc.
    subMenuTriggered = true;
    menuWasTriggered = false;
    selectedMenu = MENU_VALUE_TYP;
    settingStartValueType();
    DrawMenu(selectedMenu, 2);
    setDrawMenuLevel(selectedMenu, 2);
  }

  if (pushButtonIsLongpress && !pushButtonPressed && !menuWasTriggered && !subMenuTriggered && !subMenuLevelTwoTriggered && !encoderWasMoved && timeSystemRuns > TIME_SINCE_BOOT) {
    menuWasTriggered = true;
    menuActiveSince = millis(); // set time to now
    // Wait for release of pushButton
    while (digitalRead(VE_PB) == LOW) {}
    setDrawMenuLevel(selectedMenu, 1);
    pushButtonIsLongpress = false;
  }

  else if (!pushButtonPressed && !menuWasTriggered && !subMenuTriggered && !subMenuLevelTwoTriggered && encoderWasMoved && !SourceIsLarus) {
    changeMCvalue(encoderRight);
  }
  else if (!pushButtonPressed && menuWasTriggered && !subMenuTriggered && encoderWasMoved) {
    if (selectedMenu == MENU_SPEED_TYP) {
      setDrawMenuLevel(selectedMenu, 1);
    }
    else if (selectedMenu == MENU_HIGHT_TYP) {
      setDrawMenuLevel(selectedMenu, 1);
    }
    else if (selectedMenu == MENU_VALUE_TYP) {
      setDrawMenuLevel(selectedMenu, 1);
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
    //Serial.println("Welcher Wert soll eingestellt werden?");
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
    //Serial.println("Einzustellnder Wert wird ausgewählt");
    if (selectedMenu == MENU_SPEED_TYP || selectedMenu  == MENU_HIGHT_TYP) {
      subMenuTriggered = false;
      selectedMenu = MENU_SPEED_TYP;
      setDrawMenuLevel(selectedMenu, 0);
    }
    else if (selectedMenu == MENU_VALUE_TYP) {
      menuActiveSince = millis(); // set time to now
      subMenuTriggered = false;
      subMenuLevelTwoTriggered = true;
      setDrawMenuLevel(selectedMenu, 3);
    }
  }

  else if (!menuWasTriggered && !subMenuTriggered && !pushButtonIsShortpress && !pushButtonIsLongpress  && !pushButtonPressed && subMenuLevelTwoTriggered && encoderWasMoved) {
    //Serial.println("Wert wird eingestellt");
    changeLevelTwoMenu(encoderRight);
    changeLevelTwoMenuTurn(encoderWasMoved);
    menuActiveSince = millis(); // set time to now
  }

  else if (!menuWasTriggered && !subMenuTriggered && pushButtonIsShortpress && subMenuLevelTwoTriggered && !encoderWasMoved) {
    // Wait for release of pushButton
    while (digitalRead(VE_PB) == LOW) {
      pushButtonIsShortpress = false;
    }
    //Serial.println("eingestellter Wert wird gespeichert");
    subMenuLevelTwoTriggered = false;
    settingStandardValueType();
    if (!SourceIsLarus) {
      selectedMenu = MENU_SPEED_TYP;
    }
    setDrawMenuLevel(selectedMenu, 0);
  }

  // check run time in menu and exit if time > 10000
  if ((millis() - menuActiveSince) > 10000 && subMenuLevelTwoTriggered) {
    subMenuLevelTwoTriggered = false;
    settingStandardValueType();
    selectedMenu = MENU_SPEED_TYP;
    setDrawMenuLevel(selectedMenu, 0);
  }
  else if ((millis() - menuActiveSince) > 10000 && subMenuTriggered) {
    subMenuTriggered = false;
    settingStandardValueType();
    selectedMenu = MENU_SPEED_TYP;
    setDrawMenuLevel(selectedMenu, 0);
  }
  else if ((millis() - menuActiveSince) > 10000 && menuWasTriggered) {
    menuWasTriggered = false;
    settingStandardValueType();
    selectedMenu = MENU_SPEED_TYP;
    setDrawMenuLevel(selectedMenu, 0);
  }
}
