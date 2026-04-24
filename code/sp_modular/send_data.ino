void sendData(const SensorPacket &d) {
  if (WiFi.status() != WL_CONNECTED) return;

  String json = "{";
  json += "\"name\":\"" + String(DEVICE_NAME) + "\",";
  json += "\"device_uuid\":\"" + String(DEVICE_UUID) + "\",";
  json += "\"sensors\":{";

  // Always included and always valid
  json += "\"moisture_pct\":{\"value\":" + String(d.moistPct, 1) + "},";
  json += "\"moisture_voltage\":{\"value\":" + String(d.moistV, 3) + "},";
  json += "\"battery_voltage\":{\"value\":" + String(d.battV, 2) + "},";

#if USE_BME280
  if (isValidFloat(d.temp)) {
    json += "\"temperature\":{\"value\":" + String(d.temp,2) + "},";
  }
  if (isValidFloat(d.hum)) {
    json += "\"humidity\":{\"value\":" + String(d.hum,1) + "},";
  }
  if (isValidFloat(d.press)) {
    json += "\"pressure\":{\"value\":" + String(d.press,1) + "},";
  }
#endif

#if USE_TSL2591
  if (isValidFloat(d.lux)) {
    json += "\"lux\":{\"value\":" + String(d.lux,2) + "},";
    json += "\"ir\":{\"value\":" + String(d.ir) + "},";
    json += "\"full\":{\"value\":" + String(d.full) + "},";
  }
#endif

#if USE_PUMP
  if (d.pumpSeconds >= 0) {
    json += "\"pump_duration\":{\"value\":" + String(d.pumpSeconds) + "},";
  }
#endif

  // Remove trailing comma
  if (json.endsWith(",")) json.remove(json.length() - 1);

  json += "}}";

  HTTPClient http;
  http.begin(API_URL);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("x-api-key", API_KEY);

  int code = http.POST(json);
  Serial.println("POST response code: " + String(code));
  Serial.println("Payload: " + json);

  http.end();
}
