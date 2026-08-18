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

// Read every enabled sensor once. Does NOT run the pump — that stays in the
// normal measurement path so live-view polling can't trigger watering.
SensorPacket readAllSensors() {
  SensorPacket d;
  d.temp = d.hum = d.press = NAN;
  d.lux = NAN; d.ir = d.full = 0;
  d.pumpSeconds = 0;
  d.wifiRssi = WiFi.RSSI();

  d.moistV   = readMoistureVoltage();       // moisture always on
  d.moistPct = moistureToPercent(d.moistV);
  d.battV    = readBatteryVoltage();        // battery always on

  if (settings.useBme && !readBME280(d.temp, d.hum, d.press)) {
    Serial.println("BME280 read failed");
  }
  if (settings.useTsl && !readTSL(d.lux, d.ir, d.full)) {
    Serial.println("TSL2591 read failed");
  }
  return d;
}

void setup() {
  Serial.begin(115200);
  delay(800);

  ++bootCount;
  Serial.println("Boot #" + String(bootCount));
  printWakeupReason();

  loadSettings();
  bool doubleReset = detectDoubleReset();

  if (settings.useBme || settings.useTsl) {
    Wire.begin(PIN_SDA, PIN_SCL);
  }

  bool connected = setupWiFi();

  // Open the commissioning portal on a double reset, or when a *fresh start*
  // cannot get online — that is the case where the user is standing there with
  // the device and needs to enter credentials.
  //
  // A scheduled wake is deliberately excluded. Deep in a measurement series a
  // failed association means the network blipped, not that the device needs
  // commissioning, and opening a 5-minute AP there would cost the reading we
  // woke up to take plus a large share of the hourly power budget. Better to
  // skip the cycle and try again next hour.
  bool timerWake = (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER);
  if (doubleReset || (!connected && !timerWake)) {
    runCommissioningPortal();
  }

  SensorPacket d = readAllSensors();

  if (settings.usePump) {
    d.pumpSeconds = runPump(d.moistPct);
  }

  int code = sendData(d);
  if (code != 200 && code != 201) {
    // The reading only lives in RAM and deep sleep destroys it, so this is the
    // moment it is lost. Nothing records that yet — see the diagnostics note.
    Serial.println("Measurement NOT delivered (code " + String(code) + ") — reading lost");
  }

  goToSleep();
}

void loop() {}
