#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

bool readBME280(float &t, float &h, float &p) {
  pinMode(PIN_BME_POWER, OUTPUT);
  digitalWrite(PIN_BME_POWER, HIGH);
  delay(150);

  if (!bme.begin(0x76)) {
    digitalWrite(PIN_BME_POWER, LOW);
    return false;
  }

  t = bme.readTemperature();
  h = bme.readHumidity();
  p = bme.readPressure() / 100.0F;
  return true;
}
