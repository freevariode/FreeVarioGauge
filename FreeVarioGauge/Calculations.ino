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

//********************************
//****  calculate Checksumme  ****
//********************************
int calculateChecksum(String mce) {
  int i, XOR, c;
  for (XOR = 0, i = 0; i < mce.length(); i++) {
    c = (unsigned char)mce.charAt(i);
    if (c == '*') break;
    if ((c != '$') && (c != '!')) XOR ^= c;
  }
  return XOR;
}

//*********************************
//****  Deg to Rad conversion  ****
//*********************************
float deg2rad(float *angle) {
  //  float tempangle=*angle;
  *angle = *angle / 180 * 3.141516;
  return *angle;
}

//****************************************
//****  To Calculate xTask StackSize  ****
//****************************************
void printWatermark(void *pvParameters){
    while(1){
        delay(2000);
        Serial.print("TASK: ");
        Serial.print(pcTaskGetName(SerialScanTask)); // Get task name with handler
        Serial.print(", High Watermark: ");
        Serial.print(uxTaskGetStackHighWaterMark(SerialScanTask));
        Serial.println();
        Serial.print("TASK: ");
        Serial.print(pcTaskGetName(TaskValueRefresh)); // Get task name with handler
        Serial.print(", High Watermark: ");
        Serial.print(uxTaskGetStackHighWaterMark(TaskValueRefresh));
        Serial.println();
    }
}
