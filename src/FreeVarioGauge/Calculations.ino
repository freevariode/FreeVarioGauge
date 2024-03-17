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
