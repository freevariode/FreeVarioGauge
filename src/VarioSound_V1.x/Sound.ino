void Sound(void *) {
  while (true) {
    sf = (tas - stf) / 10;


    /////////////////////
    // mute function using PTT
    /////////////////////
    if (digitalRead(PTT) == LOW && valueMuteAsInt ) {
      gen.ApplySignal(SINE_WAVE, REG0, 0);
      delay(1);
    }


    /////////////////////
    // calculate Vario sound
    /////////////////////
    else if (digitalRead(STF_MODE) == LOW && var > 0.5 && startSound) {
      startTimePulse = millis();
      pulseTime = 0;
      while (pulseTime < calculatePulse(var)) {
        freqValueNeg = (-1 * freqValueOld) / 8;
        int  i = 0;
        while (i < 8 && freqValueNeg < 0) {
          i = i + 1;
          gen.IncrementFrequency (REG0, freqValueNeg);
          delay(1);
        }
        gen.ApplySignal(SINE_WAVE, REG0, 0);
        freqValueOld = 0;
        pulseTime = millis() - startTimePulse;
      }
      do  {
        calculateNewFreq(var, varOld);
        int  i = 0;
        while (i < 8) {
          i = i + 1;
          gen.IncrementFrequency (REG0, freqValueInc);
          delay(1);
        }
        gen.ApplySignal(SINE_WAVE, REG0, freqValue);
        pulseTime = millis() - startTimePulse;
      } while (pulseTime < (calculatePulse(var) + (calculatePulse(var) / 2)));
    }
    else if (digitalRead(STF_MODE) == LOW && var <= 0.5 && startSound) {
      calculateNewFreq(var, varOld);
      int  i = 0;
      while (i < 8) {
        i = i + 1;
        gen.IncrementFrequency (REG0, freqValueInc);
        delay(1);
      }
      gen.ApplySignal(SINE_WAVE, REG0, freqValue);
    }


    /////////////////////
    // calculate STF sound
    /////////////////////
    else if (digitalRead(STF_MODE) == HIGH && ((sf > 1) || (sf < -1)) && startSound) {
      if ((count = 0) || (pulseTime > 1050)) {
        startTimePulse = millis();
        pulseTime = 0;
      }
      if ((pulseTime < 300) || ((pulseTime > 450) && (pulseTime < 750))) {
        calculateNewFreq(sf, sfOld);
        int  i = 0;
        while (i < 8) {
          i = i + 1;
          gen.IncrementFrequency (REG0, freqValueInc);
          delay(1);
        }
        gen.ApplySignal(SINE_WAVE, REG0, freqValue);
        pulseTime = millis() - startTimePulse;
      }


      else if ((pulseTime >= 300) && (pulseTime <= 450) || ((pulseTime >= 750) && (pulseTime <= 1050))) {
        freqValueNeg = (-1 * freqValueOld) / 8;
        int  i = 0;
        while (i < 8 && freqValueNeg < 0) {
          i = i + 1;
          gen.IncrementFrequency (REG0, freqValueNeg);
          delay(1);
        }
        gen.ApplySignal(SINE_WAVE, REG0, 0);
        freqValueOld = 0;
        pulseTime = millis() - startTimePulse;
      }
      count = 0;
    }

    else if (digitalRead(STF_MODE) == HIGH && sf > -1 && sf < 1 && startSound) {
      gen.ApplySignal(SINE_WAVE, REG0, 0);
      delay(1);
    }
    else {
      delay(1);
    }
  }
  varOld = var;
  sfOld = sf;
  vTaskDelay(10);
}
