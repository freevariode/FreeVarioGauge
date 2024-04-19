void loop() {
  if (showBootscreen) {
    UpdateMode();
  }
  else {
    EncoderReader();
    Menu ();
  }
}
