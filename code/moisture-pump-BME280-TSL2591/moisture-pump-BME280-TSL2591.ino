#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_TSL2591.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiManager.h>

// ================= USER SETTINGS =================
const char* deviceName = "INSERT NAME";
const char* deviceUUID = "INSERT UUID";
const char* apiUrl  = "INSERT API URL";
const char* apiKey  = "INSERT REAL KEY HERE";

// Moisture calibration
const float minMoistureVoltage = 0.60;
const float maxMoistureVoltage = 2.45;
const float batteryDividerRatio = 2.0;

// Pump control
const int pumpPin = D7;                 // MOSFET gate for pump
const float moistureThreshold = 30.0;   // % threshold below which pump activates
const int pumpDuration = 5;             // seconds pump runs when soil is too dry

// Sleep config
#define TIME_TO_SLEEP 3600
#define uS_TO_S_FACTOR 1000000ULL
RTC_DATA_ATTR int bootCount = 0;

// ========== PIN CONFIG ==========
const int moisturePin     = A1;
const int batteryPin      = A0;
const int sensorPowerPin  = D10;  // Moisture sensor power
const int bmePowerPin     = D6;   // BME280 + TSL2591 power
const int SDA_PIN         = D4;
const int SCL_PIN         = D5;

// ========== OBJECTS ==========
Adafruit_BME280 bme;
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);

// ========== FUNCTIONS ==========
void printWakeupReason() {
  esp_sleep_wakeup_cause_t reason = esp_sleep_get_wakeup_cause();
  if (reason == ESP_SLEEP_WAKEUP_TIMER) Serial.println("Wakeup: Timer");
  else Serial.printf("Wakeup not from timer: %d\n", reason);
}

void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin();
  int retries = 0;
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500); Serial.print(".");
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
  wm.setConfigPortalTimeout(120);
  if (!wm.autoConnect(apName.c_str())) {
    Serial.println("WiFiManager failed or timed out. Sleeping...");
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    esp_deep_sleep_start();
  }
}

float readBatteryVoltage() {
  analogRead(batteryPin);
  delay(50);
  uint32_t Vbatt = 0;
  for (int i = 0; i < 16; i++) Vbatt += analogReadMilliVolts(batteryPin);
  return batteryDividerRatio * Vbatt / 16 / 1000.0;
}

float readMoistureVoltage() {
  delay(50);
  uint32_t Vraw = 0;
  for (int i = 0; i < 16; i++) Vraw += analogReadMilliVolts(moisturePin);
  return Vraw / 16.0 / 1000.0;
}

float moistureVoltageToPercent(float voltage) {
  float percent = (maxMoistureVoltage - voltage) / (maxMoistureVoltage - minMoistureVoltage) * 100.0;
  return constrain(percent, 0.0, 100.0);
}

bool readBME280(float &temperature, float &humidity, float &pressure) {
  pinMode(bmePowerPin, OUTPUT);
  digitalWrite(bmePowerPin, HIGH);
  delay(150);

  Wire.begin(SDA_PIN, SCL_PIN);
  bool status = bme.begin(0x76);
  if (!status) {
    Serial.println("BME280 not found!");
    temperature = humidity = pressure = NAN;
    return false;
  }

  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  return true;
}

bool readTSL2591(float &lux, uint16_t &ir, uint16_t &full) {
  bool status = tsl.begin();
  if (!status) {
    Serial.println("TSL2591 not found!");
    lux = NAN;
    ir = full = 0;
    return false;
  }

  tsl.setGain(TSL2591_GAIN_MED);        // Medium gain
  tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);

  uint32_t lum = tsl.getFullLuminosity();
  ir = lum >> 16;
  full = lum & 0xFFFF;
  lux = tsl.calculateLux(full, ir);

  Serial.println("Light sensor: Full=" + String(full) + " IR=" + String(ir) + " Lux=" + String(lux, 2));
  return true;
}

int controlPump(float moisturePercent) {
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);

  if (moisturePercent < moistureThreshold) {
    Serial.println("Soil dry (" + String(moisturePercent, 1) + "%) — activating pump for " + String(pumpDuration) + " s");
    digitalWrite(pumpPin, HIGH);
    delay(pumpDuration * 1000);
    digitalWrite(pumpPin, LOW);
    Serial.println("Pump deactivated.");
    return pumpDuration;
  } else {
    Serial.println("Soil moisture sufficient (" + String(moisturePercent, 1) + "%) — pump off.");
    return 0;
  }
}

void sendData(float moistureVoltage, float moisturePercent, float batteryVoltage,
              float temperature, float humidity, float pressure,
              float lux, uint16_t ir, uint16_t full, int pumpDurationTriggered) {
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
    json += "\"battery_voltage\": {\"value\": " + String(batteryVoltage, 2) + ", \"unit\": \"V\"},";
    json += "\"temperature\": {\"value\": " + String(temperature, 2) + ", \"unit\": \"°C\"},";
    json += "\"humidity\": {\"value\": " + String(humidity, 1) + ", \"unit\": \"%RH\"},";
    json += "\"pressure\": {\"value\": " + String(pressure, 1) + ", \"unit\": \"hPa\"},";
    json += "\"light_lux\": {\"value\": " + String(lux, 2) + ", \"unit\": \"lux\"},";
    json += "\"light_ir\": {\"value\": " + String(ir) + ", \"unit\": \"raw\"},";
    json += "\"light_full\": {\"value\": " + String(full) + ", \"unit\": \"raw\"},";
    json += "\"pump_duration\": {\"value\": " + String(pumpDurationTriggered) + ", \"unit\": \"s\"}";
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
  digitalWrite(bmePowerPin, LOW);
  digitalWrite(pumpPin, LOW);
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

  float temperature = NAN, humidity = NAN, pressure = NAN;
  readBME280(temperature, humidity, pressure);

  float lux = NAN;
  uint16_t ir = 0, full = 0;
  readTSL2591(lux, ir, full);

  Serial.println("Moisture: " + String(moisturePercent, 1) + "%, Light: " + String(lux, 2) + " lux");

  int pumpDurationTriggered = controlPump(moisturePercent);

  sendData(moistureVoltage, moisturePercent, batteryVoltage,
           temperature, humidity, pressure, lux, ir, full, pumpDurationTriggered);

  shutdownAndSleep();
}

void loop() {}
