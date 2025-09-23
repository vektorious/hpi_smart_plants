#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager

// ================= USER SETTINGS =================

// Device identification
// Choose a unique name and short ID for your device
// You can use an UUID generator and just use the first part (8 symbols) of the generated ID
// https://www.uuidgenerator.net/version4
const char* deviceName = "Maker Universe";
const char* deviceUUID = "22593b21";

// API endpoint
const char* apiUrl  = "https://plants.makeruniverse.de/plants/measurements";
const char* apiKey  = "vKpsikScqRUt2CdC";

// Moisture sensor calibration
// Measure voltage when sensor is in water (100%) and dry soil or air (0%)
const float minMoistureVoltage = 0.60;
const float maxMoistureVoltage = 2.45;

// Battery voltage divider ratio
// Only change this if you are using a different resistor setup
const float batteryDividerRatio = 2.0;

// Measurement interval (in seconds)
// 300 = 5 minutes, 3600 = 1 hour
// Measuring every hour is recommended for longer battery life
#define TIME_TO_SLEEP 300 

// ================= PIN CONFIG ====================
const int moisturePin     = A1;
const int batteryPin      = A0;
const int sensorPowerPin  = D10;


#define uS_TO_S_FACTOR 1000000ULL
RTC_DATA_ATTR int bootCount = 0;

/// ========== FUNCTIONS ==========

void printWakeupReason() {
  esp_sleep_wakeup_cause_t reason = esp_sleep_get_wakeup_cause();
  switch (reason) {
    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("Wakeup: Timer");
      break;
    default:
      Serial.printf("Wakeup not from timer: %d\n", reason);
      break;
  }
}

void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin();

  Serial.print("Connecting to Wi-Fi");
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wi-Fi connected: " + WiFi.localIP().toString());
    return;
  }

  Serial.println("Wi-Fi failed. Starting WiFiManager...");
  WiFiManager wm;
  String apName = "SmartPlant-Setup-" + String(deviceName);
  wm.setConfigPortalTimeout(120);  // 2 min timeout
  if (!wm.autoConnect(apName.c_str())) {
    Serial.println("WiFiManager failed or timed out. Sleeping...");
    shutdownAndSleep();
  }
}

float readBatteryVoltage() {
  analogRead(batteryPin);
  delay(50);
  uint32_t Vbatt = 0;
  for (int i = 0; i < 16; i++) {
    Vbatt += analogReadMilliVolts(batteryPin);
  }
  return batteryDividerRatio * Vbatt / 16 / 1000.0;
}

float readMoistureVoltage() {
  delay(50);
  uint32_t Vraw = 0;
  for (int i = 0; i < 16; i++) {
    Vraw += analogReadMilliVolts(moisturePin);
  }
  return Vraw / 16.0 / 1000.0;
}

float moistureVoltageToPercent(float voltage) {
  float percent = (maxMoistureVoltage - voltage) / (maxMoistureVoltage - minMoistureVoltage) * 100.0;
  return constrain(percent, 0.0, 100.0);
}

void sendData(float moistureVoltage, float moisturePercent, float batteryVoltage) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(apiUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("x-api-key", apiKey);

    String json = "{";
    json += "\"name\": \"" + String(deviceName) + "\",";
    json += "\"device_uuid\": \"" + String(deviceUUID) + "\",";
    json += "\"sensors\": {";
    json += "\"moisture_voltage\": {\"value\": " + String(moistureVoltage, 3) + ", \"unit\": \"V\"},";
    json += "\"moisture\": {\"value\": " + String(moisturePercent, 1) + ", \"unit\": \"%\"},";
    json += "\"battery_voltage\": {\"value\": " + String(batteryVoltage, 2) + ", \"unit\": \"V\"}";
    json += "}}";

    int httpResponseCode = http.POST(json);
    Serial.println("POST response code: " + String(httpResponseCode));
    Serial.println("Payload: " + json);
    http.end();
  } else {
    Serial.println("WiFi not connected, skipping data send.");
  }
}

void shutdownAndSleep() {
  digitalWrite(sensorPowerPin, LOW);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  delay(200);
  Serial.flush();
  Serial.end();
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

// ========== MAIN ==========

void setup() {
  Serial.begin(115200);
  delay(1000);

  ++bootCount;
  Serial.println("Boot #" + String(bootCount));
  printWakeupReason();

  setupWiFi();

  pinMode(sensorPowerPin, OUTPUT);
  digitalWrite(sensorPowerPin, HIGH);
  delay(200);
  float moistureVoltage = readMoistureVoltage();
  float moisturePercent = moistureVoltageToPercent(moistureVoltage);
  digitalWrite(sensorPowerPin, LOW);

  float batteryVoltage = readBatteryVoltage();

  Serial.println("Moisture voltage: " + String(moistureVoltage, 3) + " V");
  Serial.println("Moisture percent: " + String(moisturePercent, 1) + " %");
  Serial.println("Battery voltage: " + String(batteryVoltage, 2) + " V");

  sendData(moistureVoltage, moisturePercent, batteryVoltage);
  shutdownAndSleep();
}

void loop() {
  // Nothing here
}
