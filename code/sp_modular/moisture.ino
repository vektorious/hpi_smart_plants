float readMoistureVoltage() {
  pinMode(PIN_SENSOR_POWER, OUTPUT);
  digitalWrite(PIN_SENSOR_POWER, HIGH);
  delay(150);

  uint32_t v = 0;
  for (int i = 0; i < 16; i++) {
    v += analogReadMilliVolts(PIN_MOISTURE);
  }

  digitalWrite(PIN_SENSOR_POWER, LOW);
  return (v / 16.0) / 1000.0;
}

float moistureToPercent(float voltage) {
  float pct = (MAX_MOIST_V - voltage) / (MAX_MOIST_V - MIN_MOIST_V) * 100;
  return constrain(pct, 0, 100);
}
