void sendData(const SensorPacket &d) {
  if (WiFi.status() != WL_CONNECTED) return;

  // Check if there is ANY valid sensor data besides moisture/battery
  bool hasAnyData = false;

  String json = "{";
  json += "\"name\":\"" + String(DEVICE_NAME) + "\",";
  json += "\"device_uuid\":\"" + String(DEVICE_UUID) + "\",";
  json += "\"sensors\":{";

  // Always included and always valid
  json += "\"moisture_pct\":{\"value\":" + String(d.moistPct, 1) + "},";
  json += "\"battery_voltage\":{\"value\":" + String(d.battV, 2) + "},";

#if USE_BME280
  if (isValidFloat(d.temp)) {
    hasAnyData = true;
    json += "\"temperature\":{\"value\":" + String(d.temp,2) + "},";
  }
  if (isValidFloat(d.hum)) {
    hasAnyData = true;
    json += "\"humidity\":{\"value\":" + String(d.hum,1) + "},";
  }
  if (isValidFloat(d.press)) {
    hasAnyData = true;
    json += "\"pressure\":{\"value\":" + String(d.press,1) + "},";
  }
#endif

#if USE_TSL2591
  if (isValidFloat(d.lux)) {
    hasAnyData = true;
    json += "\"lux\":{\"value\":" + String(d.lux,2) + "},";
  }
  if (d.ir > 0 || d.full > 0) {
    hasAnyData = true;
    json += "\"ir\":{\"value\":" + String(d.ir) + "},";
    json += "\"full\":{\"value\":" + String(d.full) + "},";
  }
#endif

#if USE_PUMP
  if (d.pumpSeconds >= 0) {
    json += "\"pump_duration\":{\"value\":" + String(d.pumpSeconds) + "},";
    hasAnyData = true;
  }
#endif

  // Remove trailing comma
  if (json.endsWith(",")) json.remove(json.length() - 1);

  json += "}}";

  // If NOTHING valid was measured, skip sending
  if (!hasAnyData) {
    Serial.println("No valid sensor data (NaN). Skipping POST.");
    return;
  }

  HTTPClient http;
  http.begin(API_URL);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("x-api-key", API_KEY);

  int code = http.POST(json);
  Serial.println("POST response code: " + String(code));
  Serial.println("Payload: " + json);

  http.end();
}
