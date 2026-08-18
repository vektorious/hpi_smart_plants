// Quick station connect using saved credentials.
// Returns true if connected. If it returns false (no/invalid credentials),
// the caller opens the commissioning portal (see portal.ino) — but only on a
// fresh power-on, never on a scheduled wake.

// A single association attempt gets this long. 10 s was too tight: a busy
// 2.4 GHz AP plus a DHCP lease can easily exceed it, and every timeout costs a
// whole measurement cycle.
static const uint32_t WIFI_ATTEMPT_MS = 15000;
static const uint8_t  WIFI_ATTEMPTS   = 2;

bool setupWiFi() {
  WiFi.mode(WIFI_STA);
  // Modem sleep saves power but adds latency and drops to the AP mid-exchange.
  // We are only awake for a few seconds per hour, so the trade is worth it.
  WiFi.setSleep(false);

  for (uint8_t attempt = 1; attempt <= WIFI_ATTEMPTS; attempt++) {
    if (attempt > 1) {
      // A stuck association does not recover on its own — tear it down first.
      WiFi.disconnect(true);
      delay(200);
    }
    WiFi.begin();

    Serial.print("Connecting (attempt " + String(attempt) + "/" + String(WIFI_ATTEMPTS) + ")");
    uint32_t started = millis();
    while (millis() - started < WIFI_ATTEMPT_MS) {
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.println("WiFi OK: " + WiFi.localIP().toString() +
                       " in " + String(millis() - started) + " ms" +
                       ", RSSI " + String(WiFi.RSSI()) + " dBm");
        return true;
      }
      delay(250);
      Serial.print(".");
    }
    Serial.println();
    Serial.println("Attempt " + String(attempt) + " timed out after " +
                   String(WIFI_ATTEMPT_MS) + " ms");
  }

  Serial.println("WiFi not connected with saved credentials");
  return false;
}

// Re-establish the link if it dropped. Called before each send attempt: a POST
// failure is often the association going away rather than the server, and
// retrying the HTTP request over a dead link just burns the retry budget.
bool ensureWiFi() {
  if (WiFi.status() == WL_CONNECTED) return true;
  Serial.println("WiFi link lost — reconnecting");
  return setupWiFi();
}
