#if USE_TSL2591

#include <Adafruit_TSL2591.h>

Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);

static void scanI2C() {
  Serial.println("I2C scan:");
  int found = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("  0x");
      Serial.println(addr, HEX);
      found++;
    }
  }
  if (found == 0) Serial.println("  no devices found");
}

bool readTSL(float &lux, uint16_t &ir, uint16_t &full) {
  pinMode(PIN_TSL_POWER, OUTPUT);
  digitalWrite(PIN_TSL_POWER, HIGH);
  delay(300);

  // Re-init Wire with explicit pins: Adafruit_I2CDevice::begin() calls Wire.begin()
  // internally without parameters, which on ESP32 may reset to default pins.
  Wire.begin(PIN_SDA, PIN_SCL);
  Serial.print("Wire SDA=");
  Serial.print(PIN_SDA);
  Serial.print(" SCL=");
  Serial.println(PIN_SCL);

  scanI2C();

  if (!tsl.begin()) {
    Serial.println("TSL2591: begin() failed — not found at 0x29");
    digitalWrite(PIN_TSL_POWER, LOW);
    return false;
  }

  tsl.setGain(TSL2591_GAIN_MED);
  tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);

  uint32_t lum = tsl.getFullLuminosity();
  ir   = lum >> 16;
  full = lum & 0xFFFF;
  // calculateLux divides by full internally; avoid NaN when both channels read 0 (e.g. at night)
  lux  = (full == 0 && ir == 0) ? 0.0f : tsl.calculateLux(full, ir);

  digitalWrite(PIN_TSL_POWER, LOW);
  return true;
}
#endif
