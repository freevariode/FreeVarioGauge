//**********************************************
//****  function to calculate pulse length  ****
//**********************************************
float calculatePulse(float liftIn) {
  float calculatedPulseLength = 0;
  if (digitalRead(STF_MODE) == LOW) {
    if (liftIn > 0.5 && liftIn < 7) {
      calculatedPulseLength = (3.5 - (44100 / (48000 / (liftIn * 0.5)))) * 100;
    }
    else if (liftIn > 7) {
      calculatedPulseLength = 30;
    }
    else {
      calculatedPulseLength = (44100 / (48000 * 2));
    }
    return calculatedPulseLength;
  }
  if (digitalRead(STF_MODE) == HIGH) {
    if (liftIn > 0.5 && liftIn < 7) {
      calculatedPulseLength = (4 - (44100 / (48000 / (liftIn * 0.2)))) * 100;
    }
    else if (liftIn > 7) {
      calculatedPulseLength = 270;
    }
    else {
      calculatedPulseLength = (44100 / (48000 * 2));
    }
    return calculatedPulseLength;
  }
}


//*******************************************
//****  function to calculate frequency  ****
//*******************************************
float calculateNewFreq(float newValue, float oldValue) {
  if (digitalRead(STF_MODE) == LOW && (newValue >= 0) && (newValue < 8) && (newValue != oldValue)) {
    freqValue = (350 + (120 * newValue));
  }
  else if (digitalRead(STF_MODE) == LOW && (newValue < 0) && (newValue > -8) && (newValue != oldValue)) {
    freqValue = (350 / (1 - (0.1 * newValue)));
  }
  else if (digitalRead(STF_MODE) == LOW && (newValue > 8) && (newValue != oldValue)) {
    freqValue = 1310;
  }
  else if (digitalRead(STF_MODE) == HIGH && (newValue >= 0) && (newValue < 8) && (newValue != oldValue)) {
    freqValue = (350 + (120 * newValue));
  }
  else if (digitalRead(STF_MODE) == HIGH && (newValue < 0) && (newValue > -8) && (newValue != oldValue)) {
    freqValue = (350 / (1 - (0.1 * newValue)));
  }
  else if (digitalRead(STF_MODE) == HIGH && (newValue > 8) && (newValue != oldValue)) {
    freqValue = 1310;
  }
  return freqValue;
}

//**************************
//****  Filter for STF  ****
//**************************
float filter(float filteredSTF, uint16_t filterfactor) {
  static uint16_t count = 0;
  // so that at the beginning the value is close to the measured value
  if (count < filterfactor) {
    filterfactor = count++;
  }
  stf = ((stf * filterfactor) + filteredSTF) / (filterfactor + 1);
  return stf;
}
