// Builds the measurement payload and delivers it, retrying transient failures.
//
// The reading only exists in RAM, and deep sleep destroys it — so a POST that
// fails is a measurement lost forever. Retrying here is the whole defence.
// The server assigns the timestamp on arrival, so a retry lands a few seconds
// later than the reading was taken; at an hourly cadence that is irrelevant.

// Attempts and pacing. The budget bounds the worst case: a fully unreachable
// server must not keep the radio up for minutes on a battery-powered device.
static const uint8_t  SEND_ATTEMPTS   = 3;
static const uint32_t SEND_BACKOFF_MS = 2000;    // doubles after each failure
static const uint32_t SEND_BUDGET_MS  = 45000;   // hard ceiling for all attempts

// HTTPClient defaults to 5 s, which is short for a TLS handshake over a weak
// link — a slow-but-working connection would be given up on as if it were dead.
static const uint32_t HTTP_CONNECT_MS = 8000;
static const uint32_t HTTP_TIMEOUT_MS = 10000;

static String buildPayload(const SensorPacket &d) {
  String json = "{";
  json += "\"name\":\"" + String(settings.deviceName) + "\",";
  json += "\"device_id\":\"" + String(settings.deviceId) + "\",";
  // Optional top-level grouping — omit the key entirely when unset so ungrouped
  // devices still produce a valid payload.
  if (strlen(settings.project) > 0) {
    json += "\"project\":\"" + String(settings.project) + "\",";
  }
  json += "\"sensors\":{";

  // Always included and always valid. Each sensor carries an intrinsic unit;
  // ir/full are raw counts and have no unit.
  json += "\"moisture_pct\":{\"value\":" + String(d.moistPct, 1) + ",\"unit\":\"%\"},";
  json += "\"moisture_voltage\":{\"value\":" + String(d.moistV, 3) + ",\"unit\":\"V\"},";
  json += "\"battery_voltage\":{\"value\":" + String(d.battV, 2) + ",\"unit\":\"V\"},";
  json += "\"wifi_rssi\":{\"value\":" + String(d.wifiRssi) + ",\"unit\":\"dBm\"},";

  if (settings.useBme) {
    if (isValidFloat(d.temp)) {
      json += "\"temperature\":{\"value\":" + String(d.temp,2) + ",\"unit\":\"C\"},";
    }
    if (isValidFloat(d.hum)) {
      json += "\"humidity\":{\"value\":" + String(d.hum,1) + ",\"unit\":\"%\"},";
    }
    if (isValidFloat(d.press)) {
      json += "\"pressure\":{\"value\":" + String(d.press,1) + ",\"unit\":\"hPa\"},";
    }
  }

  if (settings.useTsl) {
    if (isValidFloat(d.lux)) {
      json += "\"lux\":{\"value\":" + String(d.lux,2) + ",\"unit\":\"lx\"},";
    }
    json += "\"ir\":{\"value\":" + String(d.ir) + "},";
    json += "\"full\":{\"value\":" + String(d.full) + "},";
  }

  if (settings.usePump && d.pumpSeconds >= 0) {
    json += "\"pump_duration\":{\"value\":" + String(d.pumpSeconds) + ",\"unit\":\"s\"},";
  }

  // Remove trailing comma
  if (json.endsWith(",")) json.remove(json.length() - 1);

  json += "}}";
  return json;
}

// One delivery attempt. Returns the HTTP status, a negative HTTPClient error
// code, or -1 if there is no link to send over.
static int postPayload(const String &json) {
  if (WiFi.status() != WL_CONNECTED) return -1;

  HTTPClient http;
  http.begin(settings.apiUrl);
  http.setConnectTimeout(HTTP_CONNECT_MS);
  http.setTimeout(HTTP_TIMEOUT_MS);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("x-api-key", settings.apiKey);

  int code = http.POST(json);
  http.end();
  return code;
}

static bool isAccepted(int code) {
  // 201 = the device claimed its ID (first write), 200 = stored on an existing
  // device. Anything else means the measurement did not land.
  return code == 200 || code == 201;
}

// Negative codes are transport failures (DNS, TLS, timeout, dropped link) and
// are worth another go. So are 429 and 5xx — the server is asking us to come
// back. Any other 4xx is our own malformed or unauthorised request: retrying it
// produces the same rejection and only wastes battery.
static bool shouldRetry(int code) {
  if (code < 0)    return true;
  if (code == 429) return true;
  return code >= 500;
}

// Returns the HTTP response code of the last attempt, or -1 if WiFi never came
// up. Callers may ignore it, but it is the only signal that a reading was lost.
int sendData(const SensorPacket &d) {
  String json = buildPayload(d);
  Serial.println("Payload: " + json);

  uint32_t startedAt = millis();
  int code = -1;

  for (uint8_t attempt = 1; attempt <= SEND_ATTEMPTS; attempt++) {
    ensureWiFi();
    code = postPayload(json);
    Serial.println("POST attempt " + String(attempt) + "/" + String(SEND_ATTEMPTS) +
                   " -> " + String(code));

    if (isAccepted(code)) return code;

    if (!shouldRetry(code)) {
      Serial.println("Response is not retriable — giving up on this reading");
      return code;
    }
    if (attempt == SEND_ATTEMPTS) break;

    uint32_t wait = SEND_BACKOFF_MS << (attempt - 1);   // 2 s, then 4 s
    if (millis() - startedAt + wait > SEND_BUDGET_MS) {
      Serial.println("Send budget exhausted — giving up on this reading");
      break;
    }
    Serial.println("Retrying in " + String(wait) + " ms");
    delay(wait);
  }

  return code;
}
