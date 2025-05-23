//*************************
//****  Calculate arc  ****
//*************************
// x,y == coords of centre of arc
// start_angle = 0 - 359
// seg_count = number of 3 degree segments to draw (120 => 360 degree arc)
// rx = x axis radius
// yx = y axis radius
// w  = width (thickness) of arc in pixels
// color = 16 bit color value
// Note if rx and ry are the same an arc of a circle is drawn

double sf;                                      //Speedfaktor zur Berechnung des STF-Tons
float B, B_alt;
float MiddleRadius = ((OuterRadius - InnerRadius) / 2) + InnerRadius; //Middle of Sliding Circle radius

//***********************
//****  Refresh Arc *****
//***********************
void ArcRefresh() {
  while (showBootscreen) {
    vTaskDelay(1000);
  }
  if ( xSemaphoreTake( xTFTSemaphore, ( TickType_t ) 5 ) == pdTRUE )
  {
    float angle = (var * 22) + 180;
    DrawArc(angle, var, stf, valueTasAsFloat);
    xSemaphoreGive(xTFTSemaphore);
  }
  vTaskDelay(10);
}

void fillArc(int x, int y, int start_angle, int seg_count, int rx, int ry, int w, unsigned int color)
{

  byte seg = 3; // Segments are 3 degrees wide = 120 segments for 360 degrees
  byte inc = 3; // Draw segments every 3 degrees, increase to 6 for segmented ring

  //***********************************************************
  // Calculate first pair of coordinates for segment start ****
  //***********************************************************
  float sx = cos((start_angle - 90) * DEG2RAD);
  float sy = sin((start_angle - 90) * DEG2RAD);

  uint16_t x0 = sx * (rx - w) + x;
  uint16_t x1 = sx * rx + x;
  uint16_t y0;
  uint16_t y1;

  if (B > 0) {
    y0 = sy * (ry - w) + y;
    y1 = sy * ry + y;
  }
  else {
    y0 = 320 - (sy * (ry - w) + y);
    y1 = 320 - (sy * ry + y);
  }

  //***********************************************
  //****  Draw color blocks every inc degrees  ****
  //***********************************************
  for (int i = start_angle; i < (start_angle + seg * seg_count); i += inc) {

    //*********************************************************
    //****  Calculate pair of coordinates for segment end  ****
    //*********************************************************
    uint16_t y2;
    uint16_t y3;
    float sx2 = cos((i + seg - 90) * DEG2RAD);
    float sy2 = sin((i + seg - 90) * DEG2RAD);
    uint16_t x2 = sx2 * (rx - w) + x;
    uint16_t x3 = sx2 * rx + x;
    if (B > 0) {
      y2 = sy2 * (ry - w) + y;
      y3 = sy2 * ry + y;
    }
    else {
      y2 = -sy2 * (ry - w) + y;
      y3 = -sy2 * ry + y;
    }

    //********************
    //****  Draw Arc  ****
    //********************
    if ((B >= 0)) {
      if (x0 < 150) {
        drawColoredArc.createSprite(x2 - x1 + 20, y0 - y3 + 10);
        drawColoredArc.setTextColor(TFT_WHITE, TFT_DARKGREY);
        drawColoredArc.fillSprite(TFT_BLACK);
        drawColoredArc.fillTriangle(x0 - x1, y0 - y3, 0, y1 - y3, x2 - x1, y2 - y3, color);
        drawColoredArc.fillTriangle(0, y1 - y3, x2 - x1, y2 - y3, x3 - x1, 0, color);
        drawColoredArc.pushToSprite(&background, x1, y3, TFT_BLACK);
        drawColoredArc.deleteSprite();
      }
      if (x0 > 150) {
        drawColoredArc.createSprite(x3 - x0 + 20, y2 - y1 + 10);
        drawColoredArc.setTextColor(TFT_WHITE, TFT_DARKGREY);
        drawColoredArc.fillSprite(TFT_BLACK);
        drawColoredArc.fillTriangle(0, y0 - y1, x1 - x0, 0, x2 - x0, y2 - y1, color);
        drawColoredArc.fillTriangle(x1 - x0, 0, x2 - x0, y2 - y1, x3 - x0, y3 - y1, color);
        drawColoredArc.pushToSprite(&background, x0, y1, TFT_BLACK);
        drawColoredArc.deleteSprite();
      }
    }
    else if (B < 0) {
      if (x0 < 150) {
        drawColoredArc.createSprite(x2 - x1 + 20, y3 - y0 + 10);
        drawColoredArc.setTextColor(TFT_WHITE, TFT_DARKGREY);
        drawColoredArc.fillSprite(TFT_BLACK);
        drawColoredArc.fillTriangle(x0 - x1, 0, 0, y1 - y0, x2 - x1, y2 - y0, color);
        drawColoredArc.fillTriangle(0, y1 - y0, x2 - x1, y2 - y0, x3 - x1, y3 - y0, color);
        drawColoredArc.pushToSprite(&background, x1, y0, TFT_BLACK);
        drawColoredArc.deleteSprite();
      }
      if (x0 > 150) {
        drawColoredArc.createSprite(x3 - x0 + 20, y1 - y2 + 20);
        drawColoredArc.setTextColor(TFT_WHITE, TFT_DARKGREY);
        drawColoredArc.fillSprite(TFT_BLACK);
        drawColoredArc.fillTriangle(0, y0 - y2, x1 - x0, y1 - y2, x2 - x0, 0, color);
        drawColoredArc.fillTriangle(x1 - x0, y1 - y2, x2 - x0, 0, x3 - x0, y3 - y2, color);
        drawColoredArc.pushToSprite(&background, x0, y2, TFT_BLACK);
        drawColoredArc.deleteSprite();
      }
    }
    //**************************************************************
    //****  Copy segment end to sgement start for next segment  ****
    //**************************************************************
    x0 = x2;
    y0 = y2;
    x1 = x3;
    y1 = y3;
  }
}

void DrawArc(float inangle, float liftValue, double speedToFly, float trueAirSpeed) {
  if (SourceIsXCSoar == true) {
    stf_mode_state = digitalRead(STF_MODE);
  }
  else   if (SourceIsLarus == true) {
    stf_mode_state = 0;
  }

  unsigned int color;
  String st;

  //**********************************
  //****  Vario Mode Colored Arc  ****
  //**********************************
  if (stf_mode_state == 0) {
    if (liftValue <= -6) B = -6;
    if ((liftValue > -6) && (liftValue < 6)) B = liftValue;
    if (liftValue >= 6) B = 6;
  }

  //********************************
  //****  STF Mode colored Arc  ****
  //********************************
  if (stf_mode_state == 1) {
    sf = (trueAirSpeed - speedToFly) / 10;
    if (sf <= -6) B = -6;
    if ((sf > -6) && (sf < 6)) B = sf;
    if (sf >= 6) B = 6;
  }

  //**********************************
  //****  Calculate Arc Segments  ****
  //**********************************
  segmentCount = abs(B) * 7.4;
  deg2rad(&inangle);

  if (B >= 0) {
    startAngle = 270;
    segmentDraw = segmentCount;
    color = TFT_GREEN;
  }
  else if (B < 0) {
    startAngle = 270;
    segmentDraw = segmentCount;
    color = TFT_RED;
  }

  //***************************************
  //****  Start Funktion to Draw Arc  ****
  //***************************************
  fillArc(160, 160, startAngle, segmentDraw, 160, 160, 30, color);
  B_alt = B;

  //**************************************
  //****  Draw divisions and numbers  ****
  //**************************************
  for (int i = 70; i <= 300; i += 22) {
    float divangle = i;
    deg2rad(&divangle);
    int x0 = OuterRadius * cos(divangle) + xCenter;
    int y0 = OuterRadius * sin(divangle) + yCenter;
    int x1 = (MiddleRadius + 10) * cos(divangle) + xCenter;
    int y1 = (MiddleRadius + 10) * sin(divangle) + yCenter;
    drawOuterLine.createSprite(30, 30);
    drawOuterLine.setTextColor(TFT_WHITE, TFT_DARKGREY);
    drawOuterLine.fillSprite(TFT_BLACK);
    drawOuterLine.drawLine(0 + 10, 0 + 10, x1 - x0 + 10, y1 - y0 + 10, TFT_WHITE);
    drawOuterLine.pushToSprite(&background, x0 - 10, y0 - 10, TFT_BLACK);
    drawOuterLine.deleteSprite();

  }
  if (loadMenuArc) {
    number.loadFont("micross20");
    loadMenuArc = false;
  }

  number.createSprite(20, 25);
  number.setTextColor(TFT_WHITE, TFT_DARKGREY);
  number.fillSprite(TFT_BLACK);
  number.setTextSize(2);
  number.setTextDatum(TR_DATUM);
  number.drawString("0", 15, 2);
  number.pushToSprite(&background, 7, 150, TFT_BLACK);
  number.deleteSprite();

  number.createSprite(20, 25);
  number.setTextColor(TFT_WHITE, TFT_DARKGREY);
  number.fillSprite(TFT_BLACK);
  number.setTextSize(2);
  number.setTextDatum(TR_DATUM);
  number.drawString("1", 15, 2);
  number.pushToSprite(&background, 17, 95, TFT_BLACK);
  number.deleteSprite();

  number.createSprite(20, 25);
  number.setTextColor(TFT_WHITE, TFT_DARKGREY);
  number.fillSprite(TFT_BLACK);
  number.setTextSize(2);
  number.setTextDatum(TR_DATUM);
  number.drawString("2", 15, 2);
  number.pushToSprite(&background, 48, 50, TFT_BLACK);
  number.deleteSprite();

  number.createSprite(20, 18);
  number.setTextColor(TFT_WHITE, TFT_DARKGREY);
  number.fillSprite(TFT_BLACK);
  number.setTextSize(2);
  number.setTextDatum(TR_DATUM);
  number.drawString("3", 15, 2);
  number.pushToSprite(&background, 89, 20, TFT_BLACK);
  number.deleteSprite();

  number.createSprite(20, 25);
  number.setTextColor(TFT_WHITE, TFT_DARKGREY);
  number.fillSprite(TFT_BLACK);
  number.setTextSize(2);
  number.setTextDatum(TR_DATUM);
  number.drawString("4", 15, 2);
  number.pushToSprite(&background, 143, 7, TFT_BLACK);
  number.deleteSprite();

  number.createSprite(20, 25);
  number.setTextColor(TFT_WHITE, TFT_DARKGREY);
  number.fillSprite(TFT_BLACK);
  number.setTextSize(2);
  number.setTextDatum(TR_DATUM);
  number.drawString("5", 15, 2);
  number.pushToSprite(&background, 201, 16, TFT_BLACK);
  number.deleteSprite();

  number.createSprite(20, 25);
  number.setTextColor(TFT_WHITE, TFT_DARKGREY);
  number.fillSprite(TFT_BLACK);
  number.setTextSize(2);
  number.setTextDatum(TR_DATUM);
  number.drawString("1", 15, 2);
  number.pushToSprite(&background, 17, 200, TFT_BLACK);
  number.deleteSprite();

  number.createSprite(20, 25);
  number.setTextColor(TFT_WHITE, TFT_DARKGREY);
  number.fillSprite(TFT_BLACK);
  number.setTextSize(2);
  number.setTextDatum(TR_DATUM);
  number.drawString("2", 15, 2);
  number.pushToSprite(&background, 48, 246, TFT_BLACK);
  number.deleteSprite();

  number.createSprite(20, 25);
  number.setTextColor(TFT_WHITE, TFT_DARKGREY);
  number.fillSprite(TFT_BLACK);
  number.setTextSize(2);
  number.setTextDatum(TR_DATUM);
  number.drawString("3", 15, 2);
  number.pushToSprite(&background, 89, 278, TFT_BLACK);
  number.deleteSprite();

  number.createSprite(20, 25);
  number.setTextColor(TFT_WHITE, TFT_DARKGREY);
  number.fillSprite(TFT_BLACK);
  number.setTextSize(2);
  number.setTextDatum(TR_DATUM);
  number.drawString("4", 15, 2);
  number.pushToSprite(&background, 143, 292, TFT_BLACK);
  number.deleteSprite();

  number.createSprite(20, 25);
  number.setTextColor(TFT_WHITE, TFT_DARKGREY);
  number.fillSprite(TFT_BLACK);
  number.setTextSize(2);
  number.setTextDatum(TR_DATUM);
  number.drawString("5", 15, 2);
  number.pushToSprite(&background, 201, 285, TFT_BLACK);
  number.deleteSprite();
}
