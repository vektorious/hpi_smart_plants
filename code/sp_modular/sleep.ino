void goToSleep() {
  pinMode(PIN_BME_POWER, OUTPUT);
  digitalWrite(PIN_BME_POWER, LOW);
  pinMode(PIN_TSL_POWER, OUTPUT);
  digitalWrite(PIN_TSL_POWER, LOW);

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  delay(100);

  esp_sleep_enable_timer_wakeup(settings.sleepSec * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}
