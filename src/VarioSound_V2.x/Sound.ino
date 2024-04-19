void Sound(void *) {
  while (true) {
    sf = (tas - stf) / 10;


    //***********************************
    //****  mute function using PTT  ****
    //***********************************
    if (digitalRead(PTT) == LOW && valueMuteAsInt ) {
      stopSinus();
      delay(1);
    }


    //*********************************
    //****  calculate Vario sound  ****
    //*********************************
    else if (digitalRead(STF_MODE) == LOW && var > 0.5 && startSound) {
      startTimePulse = millis();
      pulseTime = 0;
      while (pulseTime < calculatePulse(var)) {
        stopSinus();
        pulseTime = millis() - startTimePulse;
        delay(1);
      }
      do  {
        calculateNewFreq(var, varOld);
        freqValue = sinusSetFrequency(freqValue);
        pulseTime = millis() - startTimePulse;
        delay(1);
      } while (pulseTime < (calculatePulse(var) + (calculatePulse(var) / 2)));
    }
    else if (digitalRead(STF_MODE) == LOW && var <= 0.5 && startSound) {
      calculateNewFreq(var, varOld);
      freqValue = sinusSetFrequency(freqValue);
      delay(1);
    }


    //*******************************
    //****  calculate STF sound  ****
    //*******************************
    else if (digitalRead(STF_MODE) == HIGH && ((sf > 1) || (sf < -1)) && startSound) {
      if ((count = 0) || (pulseTime > 1050)) {
        startTimePulse = millis();
        pulseTime = 0;
      }
      if ((pulseTime < 300) || ((pulseTime > 450) && (pulseTime < 750))) {
        calculateNewFreq(sf, sfOld);
        freqValue = sinusSetFrequency(freqValue);
        pulseTime = millis() - startTimePulse;
        delay(1);
      }

      else if ((pulseTime >= 300) && (pulseTime <= 450) || ((pulseTime >= 750) && (pulseTime <= 1050))) {
        stopSinus();
        pulseTime = millis() - startTimePulse;
        delay(1);
      }
      count = 0;
    }

    else if (digitalRead(STF_MODE) == HIGH && sf > -1 && sf < 1 && startSound) {
      stopSinus();
      delay(1);
    }
    else {
      delay(1);
    }
  }
  varOld = var;
  sfOld = sf;
  vTaskDelay(1);
}
