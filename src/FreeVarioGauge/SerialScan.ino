void SerialScan (void *p) {
  Serial.println("Serial Scan Task Created");
  float valueVaaAsFloat = 0;
  float valueVanAsFloat = 0;
  float valueGrsAsFloat = 0;
  float valueHagAsFloat = 0;
  float valueHigAsFloat = 0;
  float valueFLAsFloat = 0;
  float valueVoltageAsFloat = 0;
  float tem = 0;
  float hea = 0;
  double stfValue = 0;
  char serialString;
  String mod;
  bool serial2Error = false;
  unsigned long lastTimeSerial2 = 0;
  int pos, pos1, pos2;
  int oldstf_mode_state = digitalRead(STF_MODE);

  while (showBootscreen) {
    vTaskDelay(1000);
  }
  while (1) {
    String dataString;
    if (Serial2.available()) {
      serialString = Serial2.read();
      if (serialString == '$') {
        long timeSystemReady = millis() - lastTimeReady;
        while (serialString != 10) {
          if (serialString >= 32 && serialString <= 126) {
            dataString += serialString;
          }
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
        //Serial.println(dataString);                //aktivieren zum debuggen der seriellen Schnittstelle
      }
    }
    //****************************
    //****  XCSoar is source  ****
    //****************************
    if ((dataString.startsWith("$PFV,VAR")) || (dataString.startsWith("$PFV,VAN"))) {
      if (!SourceIsXCSoar) {
        SourceIsXCSoar = true;
        SourceIsLarus = false;
      }
    }

    if (dataString.startsWith("$PFV") && SourceIsXCSoar == true) {
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
        var = wertAsFloat;
      }

      //******************************************
      //****  analyse average vertical speed  ****
      //******************************************
      else if (variable == "VAA") {
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
        if (valueTasAsFloat > 10) {
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
        valueTasAsFloat = wertAsFloat;
        char buf[20];
        // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
        valueTasAsString = dtostrf(valueTasAsFloat, 3, 0, buf);
      }

      //*******************************
      //****  analyse groundspeed  ****
      //*******************************
      else if (variable == "GRS") {
        valueGrsAsFloat = wertAsFloat;
        char buf[20];
        // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
        valueGrsAsString = dtostrf(valueGrsAsFloat, 3, 0, buf);
      }

      //*****************************
      //****  analyse hight MSL  ****
      //*****************************
      else if (variable == "HIG") {
        valueHigAsFloat = wertAsFloat;
        char buf[20];
        // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
        valueHigAsString = dtostrf(valueHigAsFloat, 4, 0, buf);
      }

      //********************************************
      //****  analyse hight about ground level  ****
      //********************************************
      else if (variable == "HAG") {
        valueHagAsFloat = wertAsFloat;
        char buf[20];
        // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
        valueHagAsString = dtostrf(valueHagAsFloat, 4, 0, buf);
      }

      //*****************************************
      //****  analyse average wind strength  ****
      //*****************************************
      else if (variable == "AWS") {
        valueAwsAsFloat = wertAsFloat * 3.6;
        char buf[20];
        // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
        valueAwsAsString = dtostrf(valueAwsAsFloat, 3, 0, buf);
      }

      //*****************************************
      //****  analyse current wind strength  ****
      //*****************************************
      else if (variable == "CWS") {
        valueCwsAsFloat = wertAsFloat * 3.6;
        char buf[20];
        // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
        valueCwsAsString = dtostrf(valueCwsAsFloat, 3, 0, buf);
      }

      //******************************************
      //****  analyse average wind direction  ****
      //******************************************
      else if (variable == "AWD") {
        valueAwdAsFloat = wertAsFloat;
      }

      //******************************************
      //****  analyse current wind direction  ****
      //******************************************
      else if (variable == "CWD") {
        valueCwdAsFloat = wertAsFloat;
      }

      //******************************
      //****  analyse temperatur  ****
      //******************************
      else if (variable == "TEM") {
        tem = wertAsFloat;
      }

      //***********************
      //****  analyse QNH  ****
      //***********************
      else if (variable == "QNH") {
        valueQnhAsFloat = wertAsFloat;
        valueQnhAsString = String(valueQnhAsFloat, 0);
      }

      //***********************
      //****  analyse bug  ****
      //***********************
      else if (variable == "BUG") {
        valueBugAsFloat = wertAsFloat;
        valueBugAsString = String(valueBugAsFloat, 0);
      }

      //************************
      //****  analyse Mute  ****
      //************************
      else if (variable == "MUT") {
        valueMuteAsInt = wert.toInt();
      }

      //*******************************
      //****  analyse Attenuation  ****
      //*******************************
      else if (variable == "ATT") {
        valueAttenAsInt = wert.toInt();
      }
    }

    //***************************
    //****  Larus is source  ****
    //***************************

    //****************************
    //****  analyse headingd  ****
    //****************************
    if (dataString.startsWith("$PLARA")) {

      if (!SourceIsLarus) {
        nameSpeed = "TAS";
        nameSetting = "Time";
        SourceIsLarus = true;
        SourceIsXCSoar = false;
      }

      if (serial2Error == true) {
        serial2Error = false;
        Serial.println("Error detected");
      }

      int pos0 = dataString.indexOf('*');
      String dataToCheck = dataString.substring(0, pos0);
      dataString.remove(0, pos0 + 1);
      String CheckSum = dataString;
      CheckSum.toLowerCase();
      CheckSum.trim();
      int checksum = calculateChecksum(dataToCheck);
      String checksumString = String(checksum, HEX);
      if (CheckSum == checksumString) {
        lastTimeSerial2 = millis();
        //Serial2.println(dataString);
        dataToCheck.remove(0, 7);
        int pos1 = dataToCheck.indexOf(',');                   //findet den Ort des ersten ,
        int pos2 = dataToCheck.indexOf(',', pos1 + 1);         //findet den Ort des zweiten ,
        String HEA = dataToCheck.substring(pos2 + 1, pos0);    //erfasst das aktuelle Heading
        hea = HEA.toFloat();                                   //wandelt das aktuelle Heading in float
      }
    }

    //***************************************************************************
    //****  analyse current and average climb rate, hight and true airspeed  ****
    //***************************************************************************
    if (dataString.startsWith("$PLARV")) {

      if (!SourceIsLarus) {
        nameSpeed = "TAS";
        nameSetting = "Time";
        SourceIsLarus = true;
        SourceIsXCSoar = false;
      }

      if (serial2Error == true) {
        serial2Error = false;
        Serial.println("Error detected");
      }

      int pos0 = dataString.indexOf('*');
      String dataToCheck = dataString.substring(0, pos0);
      dataString.remove(0, pos0 + 1);
      String CheckSum = dataString;
      CheckSum.toLowerCase();
      CheckSum.trim();
      int checksum = calculateChecksum(dataToCheck);
      String checksumString = String(checksum, HEX);
      if (CheckSum == checksumString) {

        lastTimeSerial2 = millis();
        //Serial2.println(dataString);
        dataToCheck.remove(0, 7);
        int pos1 = dataToCheck.indexOf(',');                   //findet den Ort des ersten ,
        String VAR = dataToCheck.substring(0, pos1);           //erfasst das aktuelle Steigen
        var = VAR.toFloat();                                   //wandelt das aktuelle Steigen in float

        int pos2 = dataToCheck.indexOf(',', pos1 + 1);         //findet den Ort des zweiten ,
        String VAA = dataToCheck.substring(pos1 + 1, pos2);    //erfasst das gemittelte Steigen

        valueVaaAsFloat = VAA.toFloat();                       //wandelt das gemittelte Steigen in float
        char buf[20];                                          //wandelt das gemittelte Steigen in String inkl. Vorzeichen um
        // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
        if (valueVaaAsFloat >= 0) {
          valueVaaAsString = dtostrf(abs(valueVaaAsFloat), 3, 1, buf);
          valueVaaAsString = "+" + valueVaaAsString;
        }
        else {
          valueVaaAsString = dtostrf(abs(valueVaaAsFloat), 3, 1, buf);
          valueVaaAsString = "-" + valueVaaAsString;
        }

        int pos3 = dataToCheck.indexOf(',', pos2 + 1);         //findet den Ort des dritten ,
        String HIG = dataToCheck.substring(pos2 + 1, pos3);    //erfasst die barometrischen Höhe
        valueHigAsFloat = HIG.toFloat();                       //wandelt die barometrischen Höhe in float
        valueFLAsFloat = valueHigAsFloat / 30;                 //errechnet die Flugfläche
        char buf1[20];
        char buf2[20];
        // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
        valueHigAsString = dtostrf(valueHigAsFloat, 4, 0, buf1);
        valueFLAsString = dtostrf(valueFLAsFloat, 4, 0, buf2);

        int pos4 = dataToCheck.indexOf(',', pos3 + 1);         //findet den Ort des vierten ,
        String TAS = dataToCheck.substring(pos3 + 1, pos4);    //erfasst die TAS
        valueTasAsFloat = TAS.toFloat();                       //wandelt die TAS in float
        char buf3[20];
        // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
        valueTasAsString = dtostrf(valueTasAsFloat, 3, 0, buf3);
      }
    }

    //**************************************************
    //****  analyse instantaneous and average wind  ****
    //**************************************************
    if (dataString.startsWith("$PLARW")) {

      if (!SourceIsLarus) {
        nameSpeed = "TAS";
        nameSetting = "Time";
        SourceIsLarus = true;
        SourceIsXCSoar = false;
      }

      if (serial2Error == true) {
        serial2Error = false;
        Serial.println("Error detected");
      }

      int pos0 = dataString.indexOf('*');
      String dataToCheck = dataString.substring(0, pos0);
      dataString.remove(0, pos0 + 1);
      String CheckSum = dataString;
      CheckSum.toLowerCase();
      CheckSum.trim();
      int checksum = calculateChecksum(dataToCheck);
      String checksumString = String(checksum, HEX);
      if (CheckSum == checksumString) {
        lastTimeSerial2 = millis();
        //Serial2.println(dataString);
        dataToCheck.remove(0, 7);
        int pos1 = dataToCheck.indexOf(',');                   //findet den Ort des ersten ,
        String WD = dataToCheck.substring(0, pos1);            //erfasst die Windrichtung
        float valueWdAsFloat = WD.toFloat();                   //wandelt die Windrichtung in float

        int pos2 = dataToCheck.indexOf(',', pos1 + 1);         //findet den Ort des zweiten ,
        String WS = dataToCheck.substring(pos1 + 1, pos2);     //erfasst die Windstärke
        float valueWsAsFloat = WS.toFloat();                   //wandelt die Windstärke in float

        int pos3 = dataToCheck.indexOf(',', pos2 + 1);         //findet den Ort des dritten ,
        String WTYP = dataToCheck.substring(pos2 + 1, pos3);   //erfasst die Windart

        if (WTYP == "I") {                                     //legt Windart fest
          valueCwdAsFloat = valueWdAsFloat - hea + 180;
          valueCwsAsFloat = valueWsAsFloat;
          char buf[20];
          // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
          valueCwsAsString = dtostrf(valueCwsAsFloat, 3, 0, buf);
        }
        else if (WTYP == "A") {
          valueAwdAsFloat = valueWdAsFloat - hea + 180;
          valueAwsAsFloat = valueWsAsFloat;
          char buf[20];
          // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
          valueAwsAsString = dtostrf(valueAwsAsFloat, 3, 0, buf);
        }
      }
    }

    //***********************************
    //****  analyse battery voltage  ****
    //***********************************
    if (dataString.startsWith("$PLARB")) {

      if (!SourceIsLarus) {
        nameSpeed = "TAS";
        nameSetting = "Time";
        SourceIsLarus = true;
        SourceIsXCSoar = false;
      }

      if (serial2Error == true) {
        serial2Error = false;
        Serial.println("Error detected");
      }

      int pos0 = dataString.indexOf('*');
      String dataToCheck = dataString.substring(0, pos0);
      dataString.remove(0, pos0 + 1);
      String CheckSum = dataString;
      CheckSum.toLowerCase();
      CheckSum.trim();
      int checksum = calculateChecksum(dataToCheck);
      String checksumString = String(checksum, HEX);
      if (CheckSum == checksumString) {
        lastTimeSerial2 = millis();
        //Serial2.println(dataString);
        dataToCheck.remove(0, 7);
        int pos1 = dataToCheck.indexOf(',');                   //findet den Ort des ersten ,
        voltage = dataToCheck.substring(0, pos1);              //erfasst die Spannung
        float valueVoltageAsFloat = voltage.toFloat();         //wandelt die Spannung in float
        char buf[20];
        // dtostrf(floatvar, stringlength, digits_after_decimal, charbuf);
        valueVoltageAsString = dtostrf(valueVoltageAsFloat, 3, 1, buf);
      }
    }

    //***********************
    //****  analyse UTC  ****
    //***********************
    if (dataString.startsWith("$GPRMC")) {

      if (!SourceIsLarus) {
        nameSpeed = "TAS";
        nameSetting = "Time";
        SourceIsLarus = true;
        SourceIsXCSoar = false;
      }

      if (serial2Error == true) {
        serial2Error = false;
        Serial.println("Error detected");
      }

      int pos0 = dataString.indexOf('*');
      String dataToCheck = dataString.substring(0, pos0);
      dataString.remove(0, pos0 + 1);
      String CheckSum = dataString;
      CheckSum.toLowerCase();
      CheckSum.trim();
      int checksum = calculateChecksum(dataToCheck);
      String checksumString = String(checksum, HEX);
      if (CheckSum == checksumString) {
        lastTimeSerial2 = millis();
        //Serial2.println(dataString);
        dataToCheck.remove(0, 7);
        int pos1 = dataToCheck.indexOf(',');                   //findet den Ort des ersten ,
        UTCHour = dataToCheck.substring(0, 2);                 //erfasst die Stunde
        UTCMinute = dataToCheck.substring(2, 4);               //erfasst die Minuten
      }
    }

    else if ((millis() - lastTimeSerial2) > 10000) {
      serial2Error = true;
    }

    //*****************************
    //****  Check Flight Mode  ****
    //*****************************
    if (SourceIsXCSoar == true) {
      if (nameHight == "FL") {
        nameHight = "MSL";
      }
      stf_mode_state = digitalRead(STF_MODE);
      if (oldstf_mode_state != stf_mode_state || !WasSend) {
        if (digitalRead(STF_MODE) == LOW && digitalRead(STF_AUTO) == LOW) {
          Serial2.println("$POV,C,VAR*4F");  //Vario-Mode
          Serial2.println("$PFV,F,C*45");    //Vario-Mode
          WasSend = true;
          AutoWasSend = false;
          oldstf_mode_state = digitalRead(STF_MODE);
        }
        else if (digitalRead(STF_MODE) == HIGH && digitalRead(STF_AUTO) == LOW) {
          Serial2.println("$POV,C,STF*4B");  //STF-Mode
          Serial2.println("$PFV,F,S*55");    //STF-Mode
          WasSend = true;
          AutoWasSend = false;
          oldstf_mode_state = digitalRead(STF_MODE);
        }
      }
      if (digitalRead(STF_AUTO) == HIGH && !AutoWasSend) {
        Serial2.println("$PFV,F,A*47");    //Auto-Mode
        AutoWasSend = true;
        WasSend = false;
      }
      if (digitalRead(STF_MODE) == HIGH) {
        stf_mode = "STF";
      }
      else if (digitalRead(STF_MODE) == LOW) {
        stf_mode = "Vario";
      }
    }
    else if (SourceIsLarus == true) {
      if (nameHight == "AGL") {
        nameHight = "MSL";
      }
      stf_mode = "Vario";
    }

    dataString = "";
    vTaskDelay(20);
  }
}
