#if USE_TSL2591

#include <Adafruit_TSL2591.h>

Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);

bool readTSL(float &lux, uint16_t &ir, uint16_t &full) {
  pinMode(PIN_TSL_POWER, OUTPUT);
  digitalWrite(PIN_TSL_POWER, HIGH);
  delay(300);

  if (!tsl.begin()) {
    digitalWrite(PIN_TSL_POWER, LOW);
    return false;
  }

  tsl.setGain(TSL2591_GAIN_MED);
  tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);

  uint32_t lum = tsl.getFullLuminosity();
  ir   = lum >> 16;
  full = lum & 0xFFFF;
  lux  = tsl.calculateLux(full, ir);

  digitalWrite(PIN_TSL_POWER, LOW);
  return true;
}
#endif
