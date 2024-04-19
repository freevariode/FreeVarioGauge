/////////////////////
// read serial port
/////////////////////
void SerialScan () {
  if (updatemode == false) {
    WiFi.mode(WIFI_OFF);
    char Data;
    String DataString;
    if (Serial2.available()) {
      Data = Serial2.read();
      if (Data == '$') {
        while (Data != 10) {
          DataString += Data;
          Data = Serial2.read();
        }
        startSound = true;
        //Serial.println(DataString);
      }
      else {
        if ((!SourceIsXCSoar && !SourceIsLarus) && (baudDetect == 0) && (millis() - ChangeBaud <= 5000)) {
          Serial2.end();
          Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
          baudDetect = 1;
        }
        else if ((!SourceIsXCSoar && !SourceIsLarus) && (baudDetect == 1) &&  (millis() - ChangeBaud > 5000) && (millis() - ChangeBaud <= 10000)) {
          Serial2.end();
          Serial2.begin(38400, SERIAL_8N1, RXD2, TXD2);
          baudDetect = 0;
        }
        else if ((!SourceIsXCSoar && !SourceIsLarus) && (millis() - ChangeBaud > 10000)) {
          ChangeBaud = millis();
        }
      }
      if (DataString.startsWith("$PFV")) {
        SourceIsXCSoar = true;
        //Serial2.println(DataString);
        int pos = DataString.indexOf(',');
        DataString.remove(0, pos + 1);
        int pos1 = DataString.indexOf(',');                   //finds the place of the first,
        String variable = DataString.substring(0, pos1);      //captures the first record
        int pos2 = DataString.indexOf('*', pos1 + 1 );        //finds the place of *
        String wert = DataString.substring(pos1 + 1, pos2);   //captures the second record


        /////////////////////
        // Analysis of the climb rate
        /////////////////////
        if (variable == "VAR") {
          var = wert.toFloat();
        }

        /////////////////////
        // Analysis of the current XCSoar mode
        /////////////////////
        if (variable == "MOD") {
          mod = wert;
        }

        /////////////////////
        // Analysis of the current Remote control mode
        /////////////////////
        if (variable == "REM") {
          rem = wert;
        }

        /////////////////////
        // Analysis of the true airspeed
        /////////////////////
        if (variable == "TAS") {
          tas = wert.toFloat();
        }

        /////////////////////
        // Analysis of speed to fly
        /////////////////////
        if (variable == "STF") {
          stfValue = wert.toFloat();
          if (digitalRead(STF_MODE) == 1 && tas > 10) {
            int FF = (valueAttenAsInt * 10) + 1;
            stf = filter(stfValue, FF);
          }
          else {
            stf = tas;
          }
        }

        /////////////////////
        // Analysis Mute
        /////////////////////
        else if (variable == "MUT") {
          valueMuteAsInt = wert.toInt();
        }

        /////////////////////
        // Analysis Attenuation
        /////////////////////
        else if (variable == "ATT") {
          valueAttenAsInt = wert.toInt();
        }

        sf = (tas - stf) / 10;
      }
      if (DataString.startsWith("$PLAR")) {
        SourceIsLarus = true;
      }
      DataString = "";
      vTaskDelay(20);
    }
  }
}
