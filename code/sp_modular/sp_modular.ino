#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiManager.h>
#include "config.h"

// RTC boot counter (definition, with attribute)
RTC_DATA_ATTR uint32_t bootCount = 0;

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

  loadSettings();

  if (settings.useBme || settings.useTsl) {
    Wire.begin(PIN_SDA, PIN_SCL);
  }

  setupWiFi();

  SensorPacket d;
  d.temp = d.hum = d.press = NAN;
  d.lux = NAN; d.ir = d.full = 0;
  d.pumpSeconds = 0;
  d.wifiRssi = WiFi.RSSI();

  // Moisture always on
  d.moistV   = readMoistureVoltage();
  d.moistPct = moistureToPercent(d.moistV);

  // Battery always on
  d.battV    = readBatteryVoltage();

  if (settings.useBme) {
    if (!readBME280(d.temp, d.hum, d.press)) {
      Serial.println("BME280 init failed");
    }
  }

  if (settings.useTsl) {
    if (!readTSL(d.lux, d.ir, d.full)) {
      Serial.println("TSL2591 init failed");
    }
  }

  if (settings.usePump) {
    d.pumpSeconds = runPump(d.moistPct);
  }

  sendData(d);
  goToSleep();
}

void loop() {}
