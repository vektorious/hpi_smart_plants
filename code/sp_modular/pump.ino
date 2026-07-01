int runPump(float moisturePct) {
  pinMode(PIN_PUMP, OUTPUT);
  digitalWrite(PIN_PUMP, LOW);

  if (moisturePct < settings.moistureThreshold) {
    Serial.println("Soil dry (" + String(moisturePct, 1) + "%) — activating pump for " + String(settings.pumpDurationSec) + "s");
    digitalWrite(PIN_PUMP, HIGH);
    delay(settings.pumpDurationSec * 1000);
    digitalWrite(PIN_PUMP, LOW);
    Serial.println("Pump deactivated.");
    return settings.pumpDurationSec;
  }
  return 0;
}
