void changeMCvalue(bool mcUp) {
  String mce;
  if (mci == true) {
    mce = ("$PFV,M,S," + String((float)valueMacAsFloat) + "*");
    int checksum = calculateChecksum(mce);
    Serial2.printf("%s%X\n", mce.c_str(), checksum); //set MCE to MCI
    mci = false;
  }
  if (mcUp && (millis() - mcSend) > 100) {
    Serial2.println("$PFV,M,U*58");  //McCready Up
    mcSend = millis();
  }
  else if (!mcUp && (millis() - mcSend) > 100) {
    Serial2.println("$PFV,M,D*49");  //McCready Down
    mcSend = millis();
  }
  nameSetting = "MC";
}

void changeSpeedOption () {
  if (nameSpeed == "TAS") {
    nameSpeed = "GS";
  }
  else {
    nameSpeed = "TAS";
  }
  prefs.begin("settings", false);
  prefs.putString("nameSpeed", nameSpeed);
  prefs.end();
}

void changeHighOption () {
  if (nameHight == "AGL" && !SourceIsLarus) {
    nameHight = "MSL";
  }
  else if (nameHight == "MSL" && !SourceIsLarus) {
    nameHight = "AGL";
  }
  else if (nameHight == "MSL" && SourceIsLarus) {
    nameHight = "FL";
  }
  else if (nameHight == "FL" && SourceIsLarus) {
    nameHight = "MSL";
  }

  prefs.begin("settings", false);
  prefs.putString("nameHight", nameHight);
  prefs.end();
}

void changeValueOptionRight () {
  if ( nameSetting == "QNH" && !SourceIsLarus) {
    nameSetting = "Bug";
  }
  else if ( nameSetting == "Bug" && !SourceIsLarus) {
    nameSetting = "ATTEN";
  }
  else if ( nameSetting == "ATTEN" && !SourceIsLarus) {
    nameSetting = "Mute";
  }
  else if ( nameSetting == "Mute") {
    nameSetting = "Wind";
  }
  else if ( nameSetting == "Wind") {
    nameSetting = "STF";
  }
  else if ( nameSetting == "STF" && !SourceIsLarus) {
    nameSetting = "QNH";
  }
  else if ( nameSetting == "STF" && SourceIsLarus) {
    nameSetting = "Mute";
  }
}

void changeValueOptionLeft () {
  if ( nameSetting == "QNH" && !SourceIsLarus) {
    nameSetting = "STF";
  }
  else if ( nameSetting == "STF") {
    nameSetting = "Wind";
  }
  else if ( nameSetting == "Wind") {
    nameSetting = "Mute";
  }
  else if ( nameSetting == "Mute" && !SourceIsLarus) {
    nameSetting = "ATTEN";
  }
  else if ( nameSetting == "Mute"  && SourceIsLarus) {
    nameSetting = "STF";
  }
  else if ( nameSetting == "ATTEN"  && !SourceIsLarus) {
    nameSetting = "Bug";
  }
  else if ( nameSetting == "Bug"  && !SourceIsLarus) {
    nameSetting = "QNH";
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
  }
  if (nameSetting == "STF") {
    if (changeLevelTwoMenuTurn && valueSTFAsInt == 0) {
      valueSTFAsInt = 1;
      valueSTFAsString = "OV";
    }
    else if (changeLevelTwoMenuTurn && valueSTFAsInt == 1) {
      valueSTFAsInt = 0;
      valueSTFAsString = "Flaps";
    }
    prefs.begin("settings", false);
    prefs.putUInt("STF", valueSTFAsInt);
    prefs.end();
    String STFStr = ("$PFV,D,S," + String(valueSTFAsInt) + "*");
    int checksum = calculateChecksum(STFStr);
    char buf[20];
    // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
    Serial2.printf("%s%X\n", STFStr.c_str(), checksum);
  }
}
