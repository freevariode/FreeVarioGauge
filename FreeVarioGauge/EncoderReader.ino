void EncoderReader() {

  //*****************************************
  //****  Read Encoder & Buttons / Dips  ****
  //*****************************************
  long encoderPositionNew = Vario_Enc.getCount();
  long LONGPRESS_TIME = 500;
  long SHORTPRESS_TIME_MIN = 20;
  long SHORTPRESS_TIME_MAX = 50;
  int DEBOUNCE_DELAY = 20;

  if ((encoderPositionNew > encoderPosition) && !pushButtonPressed) {
    encoderRight = true;
    encoderWasMoved = true;
  }
  else if ((encoderPositionNew < encoderPosition) && !pushButtonPressed) {
    encoderLeft = true;
    encoderWasMoved = true;
  }
  else {
    encoderWasMoved = false;
    encoderRight = false;
    encoderLeft = false;
  }
  encoderPosition = encoderPositionNew;

  if (digitalRead(VE_PB) == LOW) {
    pushButtonPressed = true;
  }
  else {
    pushButtonPressed = false;
  }

  if (pushButtonPressed && pushButtonPressTime == NOT_SET) {
    vTaskDelay(DEBOUNCE_DELAY);
    pushButtonPressTime = millis();
  }

  else if (pushButtonPressed && pushButtonPressTime != NOT_SET) {}
  else {
    pushButtonPressTime = NOT_SET;
    pushButtonPressed = false;
    vTaskDelay(DEBOUNCE_DELAY);
  }

  if (pushButtonPressTime != NOT_SET && (millis() - pushButtonPressTime >= LONGPRESS_TIME)) {
    pushButtonIsLongpress = true;
    pushButtonIsShortpress = false;
  }
  else if (pushButtonIsLongpress && !pushButtonPressed) {
    pushButtonPressTime = NOT_SET;
  }
  else if (pushButtonPressTime != NOT_SET && (millis() - pushButtonPressTime >= SHORTPRESS_TIME_MIN) && (millis() - pushButtonPressTime < SHORTPRESS_TIME_MAX)) {
    pushButtonIsShortpress = true;
  }
  else if (pushButtonIsLongpress && !pushButtonPressed) {
    pushButtonPressTime = NOT_SET;
  }
}
