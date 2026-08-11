// Returns the HTTP response code, or -1 if WiFi is not connected.
int sendData(const SensorPacket &d) {
  if (WiFi.status() != WL_CONNECTED) return -1;

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

  HTTPClient http;
  http.begin(settings.apiUrl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("x-api-key", settings.apiKey);

  int code = http.POST(json);
  Serial.println("POST response code: " + String(code));
  Serial.println("Payload: " + json);

  http.end();
  return code;
}
