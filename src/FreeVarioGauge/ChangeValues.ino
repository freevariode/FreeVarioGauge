void changeMCvalue(bool mcUp) {
  if (mci == true) {
    mce = ("$PFV,M,S," + String((float)valueMacAsFloat) + "*");
    int checksum = calculateChecksum(mce);
    Serial2.printf("%s%X\n", mce.c_str(), checksum); //set MCE to MCI
    mci = false;
  }
  if (mcUp) {
    Serial2.println("$PFV,M,U*58");  //McCready Up
  }
  else {
    Serial2.println("$PFV,M,D*49");  //McCready Down
  }
  nameSetting = "MC";
  mcWasUpdated = true;
}

void changeSpeedOption () {
  if (nameSpeed == "TAS") {
    nameSpeed = "GS";
    grsWasUpdated = true;
  }
  else {
    nameSpeed = "TAS";
    tasWasUpdated = true;
  }
  prefs.begin("settings", false);
  prefs.putString("nameSpeed", nameSpeed);
  prefs.end();
}

void changeHighOption () {
  if (nameHight == "AGL") {
    nameHight = "MSL";
    higWasUpdated = true;
  }
  else {
    nameHight = "AGL";
    hagWasUpdated = true;
  }
  prefs.begin("settings", false);
  prefs.putString("nameHight", nameHight);
  prefs.end();
}

void changeValueOptionRight () {
  if ( nameSetting == "Bug") {
    nameSetting = "ATTEN";
    attWasUpdated = true;
  }
  else if ( nameSetting == "QNH") {
    nameSetting = "Bug";
    bugWasUpdated = true;
  }
  else if ( nameSetting == "ATTEN") {
    nameSetting = "Mute";
    muteWasUpdated = true;
  }
  else if ( nameSetting == "Mute") {
    nameSetting = "Wind";
    windWasUpdated = true;
  }

  else if ( nameSetting == "Wind") {
    nameSetting = "QNH";
    qnhWasUpdated = true;
  }
}

void changeValueOptionLeft () {
  if ( nameSetting == "Bug") {
    nameSetting = "QNH";
    qnhWasUpdated = true;
  }
  else if ( nameSetting == "QNH") {
    nameSetting = "Wind";
    windWasUpdated = true;
  }
  else if ( nameSetting == "ATTEN") {
    nameSetting = "Bug";
    bugWasUpdated = true;
  }
  else if ( nameSetting == "Mute") {
    nameSetting = "ATTEN";
    attWasUpdated = true;
  }
  else if ( nameSetting == "Wind") {
    nameSetting = "Mute";
    muteWasUpdated = true;
  }
}
void changeLevelTwoMenu (bool changeLevelTwoValue) {
  if (nameSetting == "QNH") {
    if (changeLevelTwoValue && valueQnhAsFloat < 1300) {
      valueQnhAsFloat = valueQnhAsFloat + 1;
    }
    else if (!changeLevelTwoValue && valueQnhAsFloat > 850) {
      valueQnhAsFloat = valueQnhAsFloat - 1;
    }
    String qnhStr = ("$PFV,Q,S," + String(valueQnhAsFloat) + "*");
    int checksum = calculateChecksum(qnhStr);
    char buf[20];
    // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
    valueQnhAsString = dtostrf(valueQnhAsFloat, 4, 0, buf);
    qnhWasUpdated = true;
    Serial2.printf("%s%X\n", qnhStr.c_str(), checksum);                //send QNH to XCSoar
  }

  if (nameSetting == "Bug") {
    if (changeLevelTwoValue && valueBugAsFloat < 50) {
      valueBugAsFloat = valueBugAsFloat + 1;
    }
    else if (!changeLevelTwoValue && valueBugAsFloat > 0) {
      valueBugAsFloat = valueBugAsFloat - 1;
    }
    String bugStr = ("$PFV,B,S," + String(valueBugAsFloat) + "*");
    int checksum = calculateChecksum(bugStr);
    char buf[20];
    // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
    valueBugAsString = dtostrf(valueBugAsFloat, 2, 0, buf);
    bugWasUpdated = true;
    Serial2.printf("%s%X\n", bugStr.c_str(), checksum);                //send bug to XCSoar
  }
  if (nameSetting == "ATTEN") {
    if (changeLevelTwoValue && valueAttenAsInt < 3) {
      valueAttenAsInt = valueAttenAsInt + 1;
    }
    else if (!changeLevelTwoValue && valueAttenAsInt > 0) {
      valueAttenAsInt = valueAttenAsInt - 1;
    }
    valueAttenAsString = String(valueAttenAsInt);
    prefs.begin("settings", false);
    prefs.putUInt("ATTEN", valueAttenAsInt);
    prefs.end();
    attWasUpdated = true;
    String attStr = ("$PFV,A,S," + valueAttenAsString + "*");
    int checksum = calculateChecksum(attStr);
    char buf[20];
    // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
    Serial2.printf("%s%X\n", attStr.c_str(), checksum);
  }
}
void changeLevelTwoMenuTurn (bool changeLevelTwoValue) {
  if (nameSetting == "Mute") {
    if (changeLevelTwoMenuTurn && valueMuteAsInt == 1) {
      valueMuteAsInt = 0;
      valueMuteAsString = "OFF";
    }
    else if (changeLevelTwoMenuTurn && valueMuteAsInt == 0) {
      valueMuteAsInt = 1;
      valueMuteAsString = "ON";
    }
    prefs.begin("settings", false);
    prefs.putUInt("Mute", valueMuteAsInt);
    prefs.end();
    muteWasUpdated = true;
    String muteStr = ("$PFV,S,S," + String(valueMuteAsInt) + "*");
    int checksum = calculateChecksum(muteStr);
    char buf[20];
    // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
    Serial2.printf("%s%X\n", muteStr.c_str(), checksum);
  }
  if (nameSetting == "Wind") {
    if (changeLevelTwoMenuTurn && valueWindAsInt == 1) {
      valueWindAsInt = 0;
      valueWindAsString = "OFF";
    }
    else if (changeLevelTwoMenuTurn && valueWindAsInt == 0) {
      valueWindAsInt = 1;
      valueWindAsString = "ON";
    }
    prefs.begin("settings", false);
    prefs.putUInt("Wind", valueWindAsInt);
    prefs.end();
    windWasUpdated = true;
    String windStr = ("$PFV,S,S," + String(valueWindAsInt) + "*");
    int checksum = calculateChecksum(windStr);
    char buf[20];
    // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
    Serial2.printf("%s%X\n", windStr.c_str(), checksum);
  }
}
