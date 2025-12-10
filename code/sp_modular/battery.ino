float readBatteryVoltage() {
  uint32_t v = 0;
  for (int i = 0; i < 16; i++) {
    v += analogReadMilliVolts(PIN_BATTERY);
  }
  return BATT_DIVIDER * (v / 16.0) / 1000.0;
}
