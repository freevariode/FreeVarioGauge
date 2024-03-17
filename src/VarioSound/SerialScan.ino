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
        //Serial.println(DataString);
      }
      if (DataString.startsWith("$PFV")) {
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
      DataString = "";
      vTaskDelay(20);
    }
  }
}
