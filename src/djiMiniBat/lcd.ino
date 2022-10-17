// LCD Connections
// VCC -> 5V
// GND -> GND
// DIN -> D11
// CLK -> D13
// CS  -> D10
// DC  -> D7
// RST -> D8
// BL  -> D9

#define LCD_ROT 3

int16_t LCDwidth;
int16_t LCDheight;
uint8_t textSize;
String text;
size_t textLength;

void initLCD() {
  tft.initR(INITR_GREENTAB_WS);
  tft.setRotation(LCD_ROT);
  LCDwidth = tft.width();
  LCDheight = tft.height();
  tft.fillScreen(ST77XX_BLACK);

#if LCD_ROTmod2==0
#else
  textSize = 4;
  text = "DJI";
  textLength = text.length();
  tft.setCursor((LCDwidth / 2) - ((textLength * textSize * 5 + textLength) / 2), (LCDheight / 2) - (textSize * 8 / 2) - 20);
  tft.setTextSize(textSize);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(text);

  textSize = 2;
  text = "Battery";
  textLength = text.length();
  tft.setCursor((LCDwidth / 2) - ((textLength * textSize * 5 + textLength) / 2), (LCDheight / 2));
  tft.setTextSize(textSize);
  tft.setTextColor(ST77XX_CYAN);
  tft.print(text);
  textSize = 2;
  text = "Friend";
  textLength = text.length();
  tft.setCursor((LCDwidth / 2) - ((textLength * textSize * 5 + textLength) / 2), (LCDheight / 2) + (textSize * 8) + 2);
  tft.setTextSize(textSize);
  tft.setTextColor(ST77XX_CYAN);
  tft.print(text);
}
#endif
}

void clearLCD() {
  tft.fillScreen(ST77XX_BLACK);
}

void updateLCDnoBat() {
  clearLCD();
  delay(200);

#if LCD_ROTmod2==0
#else
  textSize = 2;
  text = "Please";
  textLength = text.length();
  tft.setCursor((LCDwidth / 2) - ((textLength * textSize * 5 + textLength) / 2), (LCDheight / 2) - (textSize * 8) / 2 - (textSize * 8));
  tft.setTextSize(textSize);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print(text);
  textSize = 2;
  text = "connect";
  textLength = text.length();
  tft.setCursor((LCDwidth / 2) - ((textLength * textSize * 5 + textLength) / 2), (LCDheight / 2) - ((textSize * 8) / 2));
  tft.setTextSize(textSize);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print(text);
  textSize = 2;
  text = "battery";
  textLength = text.length();
  tft.setCursor((LCDwidth / 2) - ((textLength * textSize * 5 + textLength) / 2), (LCDheight / 2) - (textSize * 8) / 2 + (textSize * 8));
  tft.setTextSize(textSize);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print(text);
#endif
}

void updateLCD() {
#if LCD_ROTmod2==0
#else
  textSize = 1;
  tft.setTextSize(textSize);

  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);

  tft.print("Serial: ");
  tft.println(djiserial);

  tft.print("Make date: " );
  tft.println(formatted_date);

  tft.print("Cycles: ");
  tft.println(cycles);
  tft.println("");

  tft.print("Charge: ");
  if (relCharge <= 30) {
    tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
  }
  else if (relCharge <= 70) {
    tft.setTextColor(ST77XX_ORANGE, ST77XX_BLACK);
  }
  else {
    tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
  }
  tft.print(relCharge);
  tft.println("%  ");
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);

  tft.print("Temperature: ");
  tft.print(tempC);
  int16_t currX = tft.getCursorX();
  int16_t currY = tft.getCursorY();
  tft.drawChar(currX, currY, 0xF7, ST77XX_WHITE, ST77XX_BLACK, textSize);
  tft.setCursor(currX += textSize * 6, currY);
  tft.println("C    ");
  tft.println("");

  tft.print("Voltage: ");
  if (volt <= 8.0) {
    tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
  }
  else if (relCharge <= 8.4) {
    tft.setTextColor(ST77XX_ORANGE, ST77XX_BLACK);
  }
  else {
    tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
  }
  tft.print(volt, 3);
  tft.println("V   ");
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);

  tft.print("Cell 1: ");
  if (cell1 == cell2) {
    tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
  }
  else if (cell1 > cell2) {
    tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
  }
  else {
    tft.setTextColor(ST77XX_ORANGE, ST77XX_BLACK);
  }
  tft.print(cell1, 3);
  tft.println("V   ");
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);

  tft.print("Cell 2: ");
  if (cell2 == cell1) {
    tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
  }
  else if (cell2 > cell1) {
    tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
  }
  else {
    tft.setTextColor(ST77XX_ORANGE, ST77XX_BLACK);
  }
  tft.print(cell2, 3);
  tft.println("V   ");
  tft.println("");
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);

  tft.print("Current: ");
  tft.print(curr);
  tft.println("mA    ");

  if (curr > 0) {
    tft.print("Time to full: ");
    tft.print(fullChargeTime);
    tft.println("mins    ");
  }
  else {
    tft.println("                       ");
  }
#endif
}

void lcdDebugPrint(char* buf, uint8_t bufLen) {
  clearLCD();
  tft.setCursor(0, 0);
  tft.write(buf, bufLen);
  delay(1000);
}
