void SerialScan (void *p) {
  Serial.println("Serial Scan Task Created");
  char serialString;
  int pos, pos1, pos2;
  while (1) {
    String dataString;
    if (Serial2.available()) {
      serialString = Serial2.read();
      if (serialString == '$') {
        long timeSystemReady = millis() - lastTimeReady;
        while (serialString != 10) {
          dataString += serialString;
          serialString = Serial2.read();
          if (dataString.length() > 300) {
            dataString = "ERROR";
            Serial.println("Break serial Read!");
            break;
          }
        }
        if (timeSystemReady < 2000) {
          Serial2.println("$PFV,M,S,0.5*59");
          Serial2.println("$PFV,Q,S,1013*6D");
        }
        //Serial.println(dataString);
      }
    }

    if (dataString.startsWith("$PFV")) {
      if (serial2Error == true) {
        serial2Error = false;
        Serial.println("Error detected");

        // synchronize QNH after reboot of OV
        String qnhStr = ("$PFV,Q,S," + String(valueQnhAsFloat) + "*");
        int checksumQnh = calculateChecksum(qnhStr);
        String checksumQnhAsString = String (checksumQnh, HEX);
        String syncQnh = (qnhStr + checksumQnhAsString);
        Serial2.println(syncQnh);

        // synchronize Bug after reboot of OV
        String bugStr = ("$PFV,B,S," + String(valueBugAsFloat) + "*");
        int checksumBug = calculateChecksum(bugStr);
        String checksumBugAsString = String (checksumBug, HEX);
        String syncBug = (bugStr + checksumBugAsString);
        Serial2.println(syncBug);

        // synchronize MC after reboot of OV
        String mcStr = ("$PFV,M,S," + valueMacAsString + "*");
        int checksumMc = calculateChecksum(mcStr);
        String checksumMcAsString = String (checksumMc, HEX);
        String synMc = (mcStr + checksumMcAsString);
        Serial2.println(synMc);
      }

      lastTimeSerial2 = millis();
      //Serial2.println(DataString);
      int pos = dataString.indexOf(',');
      dataString.remove(0, pos + 1);
      int pos1 = dataString.indexOf(',');                   //findet den Ort des ersten ,
      String variable = dataString.substring(0, pos1);      //erfasst den ersten Datensatz
      int pos2 = dataString.indexOf('*', pos1 + 1 );        //findet den Ort des *
      String wert = dataString.substring(pos1 + 1, pos2);   //erfasst den zweiten Datensatz
      float wertAsFloat = wert.toFloat();                   // der Wert als float

      //**********************************
      //****  analyse vertical speed  ****
      //**********************************
      if (variable == "VAR") {
        if (var !=  wertAsFloat) {
          varWasUpdated = true;
        }
        var = wertAsFloat;
      }

      //******************************************
      //****  analyse average vertical speed  ****
      //******************************************
      else if (variable == "VAA") {
        if (valueVaaAsFloat !=  wertAsFloat) {
          vaaWasUpdated = true;
        }
        valueVaaAsFloat = wertAsFloat;
        char buf[20];
        // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
        if (valueVaaAsFloat >= 0) {
          valueVaaAsString = dtostrf(abs(valueVaaAsFloat), 3, 1, buf);
          valueVaaAsString = "+" + valueVaaAsString;
        }
        else {
          valueVaaAsString = dtostrf(abs(valueVaaAsFloat), 3, 1, buf);
          valueVaaAsString = "-" + valueVaaAsString;
        }
      }

      //****************************************
      //****  analyse netto vertical speed  ****
      //****************************************
      else if (variable == "VAN") {
        if (valueVanAsFloat !=  wertAsFloat) {
          vanWasUpdated = true;
        }
        valueVanAsFloat = wertAsFloat;
        char buf[20];
        // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
        if (valueVanAsFloat >= 0) {
          valueVanAsString = dtostrf(abs(valueVanAsFloat), 3, 1, buf);
          valueVanAsString = "+" + valueVanAsString;
        }
        else {
          valueVanAsString = dtostrf(abs(valueVanAsFloat), 3, 1, buf);
          valueVanAsString = "-" + valueVanAsString;
        }
      }

      //*******************************************
      //****  analyse internal McCready value  ****
      //*******************************************
      else if (variable == "MCI") {

        if (valueMacAsFloat !=  wertAsFloat) {
          mcWasUpdated = true;
        }
        valueMacAsFloat = wertAsFloat;
        mci = true;
        char buf[20];
        // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
        valueMacAsString = dtostrf(valueMacAsFloat, 3, 1, buf);
      }

      //*******************************************
      //****  analyse external McCready value  ****
      //*******************************************
      else if ((variable == "MCE") && (mci == false)) {
        if (valueMacAsFloat !=  wertAsFloat) {
          mcWasUpdated = true;
        }
        valueMacAsFloat = wertAsFloat;
        char buf[20];
        // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
        valueMacAsString = dtostrf(valueMacAsFloat, 3, 1, buf);
      }

      //***************************************
      //****  analyse current XCSoar mode  ****
      //***************************************
      else if (variable == "MOD") {
        mod = wert;
      }

      //********************************
      //****  analyse speed to fly  ****
      //********************************
      else if (variable == "STF") {
        stfValue = wert.toFloat();
        if (stf_mode_state == 1 && valueTasAsFloat > 10) {
          int FF = (valueAttenAsInt * 10) + 1;
          stf = filter(stfValue, FF);
        }
        else {
          stf = valueTasAsFloat;
        }
      }

      //*********************************
      //****  analyse true airspeed  ****
      //*********************************
      else if (variable == "TAS") {

        if (valueTasAsFloat != wertAsFloat) {
          tasWasUpdated = true;
        }
        valueTasAsFloat = wertAsFloat;
        char buf[20];
        // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
        valueTasAsString = dtostrf(valueTasAsFloat, 3, 0, buf);
      }

      //*******************************
      //****  analyse groundspeed  ****
      //*******************************
      else if (variable == "GRS") {
        if (valueGrsAsFloat != wertAsFloat) {
          grsWasUpdated = true;
        }
        valueGrsAsFloat = wertAsFloat;
        char buf[20];
        // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
        valueGrsAsString = dtostrf(valueGrsAsFloat, 3, 0, buf);
      }

      //*****************************
      //****  analyse hight MSL  ****
      //*****************************
      else if (variable == "HIG") {
        if (valueHigAsFloat != wertAsFloat) {
          higWasUpdated = true;
        }
        valueHigAsFloat = wertAsFloat;
        char buf[20];
        // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
        valueHigAsString = dtostrf(valueHigAsFloat, 4, 0, buf);
      }

      //********************************************
      //****  analyse hight about ground level  ****
      //********************************************
      else if (variable == "HAG") {
        if (valueHagAsFloat != wertAsFloat) {
          hagWasUpdated = true;
        }
        valueHagAsFloat = wertAsFloat;
        char buf[20];
        // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
        valueHagAsString = dtostrf(valueHagAsFloat, 4, 0, buf);
      }

      //*****************************************
      //****  analyse average wind strength  ****
      //*****************************************
      else if (variable == "AWS") {

        if (valueAwsAsFloat != wertAsFloat) {
          awsWasUpdated = true;
        }
        valueAwsAsFloat = wertAsFloat;
        char buf[20];
        // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
        valueAwsAsString = dtostrf(valueAwsAsFloat, 3, 0, buf);
      }

      //*****************************************
      //****  analyse current wind strength  ****
      //*****************************************
      else if (variable == "CWS") {

        if (valueCwsAsFloat != wertAsFloat) {
          cwsWasUpdated = true;
        }
        valueCwsAsFloat = wertAsFloat;
        char buf[20];
        // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
        valueCwsAsString = dtostrf(valueCwsAsFloat, 3, 0, buf);
      }

      //******************************************
      //****  analyse average wind direction  ****
      //******************************************
      else if (variable == "AWD") {

        if (valueAwdAsFloat != wertAsFloat) {
          awdWasUpdated = true;
        }
        valueAwdAsFloat = wertAsFloat;
      }

      //******************************************
      //****  analyse current wind direction  ****
      //******************************************
      else if (variable == "CWD") {

        if (valueCwdAsFloat != wertAsFloat) {
          cwdWasUpdated = true;
        }
        valueCwdAsFloat = wertAsFloat;
      }

      //***************************
      //****  analyse heading  ****
      //***************************
      else if (variable == "HEA") {

        if (valueHeaAsFloat != wertAsFloat) {
          heaWasUpdated = true;
        }
        valueHeaAsFloat = wertAsFloat;
      }

      //******************************
      //****  analyse temperatur  ****
      //******************************
      else if (variable == "TEM") {
        if (tem != wertAsFloat) {
          tempWasUpdated = true;
        }
        tem = wertAsFloat;
      }

      //***********************
      //****  analyse QNH  ****
      //***********************
      else if (variable == "QNH") {
        if ((valueQnhAsFloat != wertAsFloat) && (wertAsFloat < 2000)) {
          qnhWasUpdated = true;
        }
        valueQnhAsFloat = wertAsFloat;
        valueQnhAsString = String(valueQnhAsFloat, 0);
      }

      //***********************
      //****  analyse bug  ****
      //***********************
      else if (variable == "BUG") {
        if (valueBugAsFloat != wertAsFloat) {
          bugWasUpdated = true;
        }
        valueBugAsFloat = wertAsFloat;
        valueBugAsString = String(valueBugAsFloat, 0);
      }

      //************************
      //****  analyse Mute  ****
      //************************
      else if (variable == "MUT") {
        muteWasUpdated = true;
        valueMuteAsInt = wert.toInt();
      }

      //*******************************
      //****  analyse Attenuation  ****
      //*******************************
      else if (variable == "ATT") {
        attWasUpdated = true;
        valueAttenAsInt = wert.toInt();
      }
    }

    else if ((millis() - lastTimeSerial2) > 10000) {
      serial2Error = true;
    }
    dataString = "";
    vTaskDelay(20);
  }
}
