#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiManager.h>
#include "config.h"

// RTC boot counter (definition, with attribute)
RTC_DATA_ATTR uint32_t bootCount = 0;

// Define the externs from config.h
const char* DEVICE_NAME = "PCB-all-sensors";
const char* DEVICE_UUID = "22593b29";
const char* API_URL     = "https://plants.makeruniverse.de/plants/measurements";
const char* API_KEY     = "vKpsikScqRUt2CdC";

static void printWakeupReason() {
  esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
  switch (cause) {
    case ESP_SLEEP_WAKEUP_TIMER:  Serial.println("Wakeup: timer");      break;
    case ESP_SLEEP_WAKEUP_EXT0:   Serial.println("Wakeup: ext0");       break;
    case ESP_SLEEP_WAKEUP_EXT1:   Serial.println("Wakeup: ext1");       break;
    default:                      Serial.println("Wakeup: power-on or reset"); break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(800);

  ++bootCount;
  Serial.println("Boot #" + String(bootCount));
  printWakeupReason();

#if USE_BME280 || USE_TSL2591
  Wire.begin(PIN_SDA, PIN_SCL);
#endif

  setupWiFi();

  SensorPacket d;
  d.temp = d.hum = d.press = NAN;
  d.lux = 0; d.ir = d.full = 0;
  d.pumpSeconds = 0;

  // Moisture always on
  d.moistV   = readMoistureVoltage();
  d.moistPct = moistureToPercent(d.moistV);

  // Battery always on
  d.battV    = readBatteryVoltage();

#if USE_BME280
  if (!readBME280(d.temp, d.hum, d.press)) {
    Serial.println("BME280 init failed");
  }
#endif

#if USE_TSL2591
  if (!readTSL(d.lux, d.ir, d.full)) {
    Serial.println("TSL2591 init failed");
  }
#endif

#if USE_PUMP
  d.pumpSeconds = runPump(d.moistPct);
#endif

  sendData(d);
  goToSleep();
}

void loop() {}
