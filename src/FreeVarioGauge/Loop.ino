void loop() {
  if (showBootscreen) {
    UpdateMode();
  }
  else {
    //*****************************
    //****  Check Flight Mode  ****
    //*****************************
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
    EncoderReader();
    Menu ();
  }
}
