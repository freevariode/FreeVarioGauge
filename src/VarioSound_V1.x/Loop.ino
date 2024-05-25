void loop() {
//***********************************
//****  Analysis automatic mode  ****
//***********************************
  varioSchalter_state = digitalRead(Varioschalter);
  stfSchalter_state = digitalRead(STFSchalter);
  stfAuto_state = digitalRead(STFAuto);
  xc_WK_state = digitalRead(XC_WK);

  if (!SourceIsLarus) {
    if ((varioSchalter_state == 1) && (stfSchalter_state == 1)) {
      digitalWrite(STF_AUTO, HIGH);
    }
    else {
      digitalWrite(STF_AUTO, LOW);
    }
    if (((varioSchalter_state == 1) && (stfSchalter_state == 1) && (rem == "A") && (xc_WK_state == 0) && (stfAuto_state == 1)) ||
        ((varioSchalter_state == 1) && (stfSchalter_state == 1) && (rem == "A") && (xc_WK_state == 1) && (mod == "C")) ||
        ((varioSchalter_state == 1) && (stfSchalter_state == 1) && (rem == "C")) ||
        ((varioSchalter_state == 0) && (stfSchalter_state == 1))) {
      digitalWrite(STF_MODE, LOW);
    }
    else if (((varioSchalter_state == 1) && (stfSchalter_state == 1) && (rem == "A") && (xc_WK_state == 0) && (stfAuto_state == 0)) ||
             ((varioSchalter_state == 1) && (stfSchalter_state == 1) && (rem == "A") && (xc_WK_state == 1) && (mod == "S")) ||
             ((varioSchalter_state == 1) && (stfSchalter_state == 1) && (rem == "S")) ||
             ((varioSchalter_state == 1) && (stfSchalter_state == 0))) {
      digitalWrite(STF_MODE, HIGH);
    }
    UpdateMode();
    SerialScan ();
  }
}
