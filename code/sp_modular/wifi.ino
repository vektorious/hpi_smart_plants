void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin();

  Serial.print("Connecting");
  for (int i = 0; i < 20; i++) {
    if (WiFi.status() == WL_CONNECTED) break;
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi OK: " + WiFi.localIP().toString());
    return;
  }

  WiFiManager wm;
  wm.setConfigPortalTimeout(120);
  wm.autoConnect((String(DEVICE_NAME) + "-Setup").c_str());
}
