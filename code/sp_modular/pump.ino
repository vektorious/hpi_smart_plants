#if USE_PUMP

int runPump(float moisturePct) {
  pinMode(PIN_PUMP, OUTPUT);
  digitalWrite(PIN_PUMP, LOW);

  if (moisturePct < MOISTURE_THRESHOLD) {
    Serial.println("Soil dry (" + String(moisturePct, 1) + "%) — activating pump for " + String(PUMP_DURATION_SEC) + "s");
    digitalWrite(PIN_PUMP, HIGH);
    delay(PUMP_DURATION_SEC * 1000);
    digitalWrite(PIN_PUMP, LOW);
    Serial.println("Pump deactivated.");
    return PUMP_DURATION_SEC;
  }
  return 0;
}

#endif
