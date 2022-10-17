#define NO_BAT

#define BUT1 2
#define BUT2 3
#define CHG_EN 4
#define VBUS_DET 5
#define LCD_BL 9

#include <Wire.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ST7735.h"

Adafruit_ST7735 tft = Adafruit_ST7735(10, 7, 8);

#define BUFFER_LEN 32
uint8_t i2cBuffer[BUFFER_LEN];

bool batActive = false;
bool batActiveLast = false;
bool vbus = false;
bool charging = false;
volatile bool but1Pressed = false;

char *djiserial;
String formatted_date;
int16_t curr;
uint16_t fullChargeCap, tempK, remainCap, fullChargeTime;
uint8_t cycles, relCharge, absCharge;
double volt, cell1, cell2, tempC;

void setup() {
  Serial.begin(115200);

  pinMode(BUT1, INPUT_PULLUP);
  pinMode(BUT2, INPUT_PULLUP);
  pinMode(CHG_EN, OUTPUT);
  pinMode(VBUS_DET, INPUT);
  pinMode(LCD_BL, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(BUT1), but1ISR, FALLING);
  digitalWrite(CHG_EN, LOW);
  digitalWrite(LCD_BL, HIGH);

  initLCD();

  Wire.begin();
  Wire.setClock(100000);

  delay(1500);
  clearLCD();
  delay(500);
}

void loop() {
  batActive = batPresent();
  vbus = digitalRead(VBUS_DET);

  if (!vbus || !batActive) {
    stopCharging();
  }
  else if (vbus && batActive) {
    if (but1Pressed) {
      but1Pressed = false;
      if (!charging) {
        startCharging();
      }
      else if (charging) {
        stopCharging();
      }
    }
  }

  if (batActive) {
    if (batActive != batActiveLast) {
      getStaticBatData();
      clearLCD();
      delay(100);
    }
    getDynamicBatData();
    updateLCD();
  }
  else {
    updateLCDnoBat();
  }
  printBatData();
  batActiveLast = batActive;

  delay(1000);
}

void but1ISR() {
  but1Pressed = true;
}
