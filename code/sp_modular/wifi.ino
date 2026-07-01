// Quick station connect using saved credentials.
// Returns true if connected. If it returns false (no/invalid credentials),
// the caller opens the commissioning portal (see portal.ino).
bool setupWiFi() {
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
    return true;
  }

  Serial.println("WiFi not connected with saved credentials");
  return false;
}
