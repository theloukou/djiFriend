#define DEV_ADDR 11

// Standard and common non-standard Smart Battery commands
#define BATTERY_MODE             0x03
#define TEMPERATURE              0x08
#define VOLTAGE                  0x09
#define CURRENT                  0x0A
#define RELATIVE_SOC             0x0D
#define ABSOLUTE_SOC             0x0E
#define REMAINING_CAPACITY       0x0F
#define FULL_CHARGE_CAPACITY     0x10
#define TIME_TO_FULL             0x13
#define CHARGING_CURRENT         0x14
#define CHARGING_VOLTAGE         0x15
#define BATTERY_STATUS           0x16
#define CYCLE_COUNT              0x17
#define DESIGN_CAPACITY          0x18
#define DESIGN_VOLTAGE           0x19
#define SPEC_INFO                0x1A
#define MFG_DATE                 0x1B
#define SERIAL_NUM               0x1C
#define MFG_NAME                 0x20   // String
#define DEV_NAME                 0x21   // String
#define CELL_CHEM                0x22   // String
#define MFG_DATA                 0x23   // String
#define CELL4_VOLTAGE            0x3C
#define CELL3_VOLTAGE            0x3D
#define CELL2_VOLTAGE            0x3E
#define CELL1_VOLTAGE            0x3F
#define STATE_OF_HEALTH          0x4F
#define DJI_SERIAL               0xD8  // String

bool batPresent() {
  Wire.beginTransmission(DEV_ADDR);
  if (Wire.endTransmission() == 0) {
    return true;
  }
  else {
    return false;
  }
}

void startCharging() {
  digitalWrite(CHG_EN, HIGH);
  charging = true;
  delay(500);
}

void stopCharging() {
  digitalWrite(CHG_EN, LOW);
  charging = false;
  delay(500);
}

uint8_t read_byte() {
  while (1) {
    if (Wire.available()) {
      return Wire.read();
    }
  }
}

int fetchWord(byte func) {
  Wire.beginTransmission(DEV_ADDR);
  Wire.write(func);
  Wire.endTransmission(false);
  delay(1);// FIX wire bug
  Wire.requestFrom(DEV_ADDR, 2, true);

  uint8_t b1 = read_byte();
  uint8_t b2 = read_byte();
  Wire.endTransmission();
  return (int)b1 | ((( int)b2) << 8);
}

uint8_t i2c_smbus_read_block ( uint8_t command, uint8_t* blockBuffer, uint8_t blockBufferLen ) {
  uint8_t x, num_bytes;
  Wire.beginTransmission(DEV_ADDR);
  Wire.write(command);
  Wire.endTransmission(false);
  delay(1);
  Wire.requestFrom(DEV_ADDR, blockBufferLen, true);

  num_bytes = read_byte();
  num_bytes = constrain(num_bytes, 0, blockBufferLen - 2);
  for (x = 0; x < num_bytes - 1; x++) { // -1 because x=num_bytes-1 if x<y; last byte needs to be "nack"'d, x<y-1
    blockBuffer[x] = read_byte();
  }
  blockBuffer[x++] = read_byte(); // this will nack the last byte and store it in x's num_bytes-1 address.
  blockBuffer[x] = 0; // and null it at last_byte+1
  Wire.endTransmission();
  return num_bytes;
}

void getStaticBatData() {
  uint8_t length_read = 0;
  length_read = i2c_smbus_read_block(DJI_SERIAL, i2cBuffer, BUFFER_LEN);
  djiserial = i2cBuffer;

  int mdate = fetchWord(MFG_DATE);
  int mday = B00011111 & mdate;
  int mmonth = mdate >> 5 & B00001111;
  int myear = 1980 + (mdate >> 9 & B01111111);
  formatted_date += mday;
  formatted_date += "/";
  formatted_date += mmonth;
  formatted_date += "/";
  formatted_date += myear;
  //    int str_len = formatted_date.length() + 1;
  //  char mfg_date[str_len];
  //  formatted_date.toCharArray(mfg_date, str_len);

  fullChargeCap = fetchWord(FULL_CHARGE_CAPACITY);
}

void getDynamicBatData() {
  remainCap = fetchWord(REMAINING_CAPACITY);
  cycles = fetchWord(CYCLE_COUNT);
  relCharge = fetchWord(RELATIVE_SOC);
  absCharge = fetchWord(ABSOLUTE_SOC);
  volt = (double)fetchWord(VOLTAGE) / 1000;
  cell1 = (double)fetchWord(CELL1_VOLTAGE) / 1000;
  cell2 = (double)fetchWord(CELL2_VOLTAGE) / 1000;
  tempK = fetchWord(TEMPERATURE);
  tempC = tempK / 10.0 - 273.15;
  curr = fetchWord(CURRENT);
  fullChargeTime = fetchWord(TIME_TO_FULL);
}

void printBatData() {
  if (batActive) {
    Serial.print("Serial Number: ");
    Serial.println(djiserial);
    Serial.print("Manufacture Date (D.M.Y): " );
    Serial.println(formatted_date);
    Serial.print("Full Charge Capacity: " );
    Serial.println(fullChargeCap);
    Serial.print("Remaining Capacity: " );
    Serial.println(remainCap);
    Serial.print("Cycle Count: " );
    Serial.println(cycles);

    Serial.print("Relative Charge(%): ");
    Serial.println(relCharge);
    Serial.print("Absolute Charge(%): ");
    Serial.println(absCharge);
    Serial.print("Temp: ");
    Serial.println(tempC);

    Serial.print("Voltage: ");
    Serial.println(volt, 3);
    Serial.print("Cell 1 Voltage: ");
    Serial.println(cell1, 3);
    Serial.print("Cell 2 Voltage: ");
    Serial.println(cell2, 3);

    Serial.print("Current (mA): " );
    Serial.println(curr);
    Serial.print("Minutes remaining for full charge: ");
    Serial.println(fullChargeTime);
    Serial.println("");
  }
  else {
    Serial.println("Please connect battery");
  }
}
