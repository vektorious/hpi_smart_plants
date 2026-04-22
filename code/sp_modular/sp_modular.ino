#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiManager.h>
#include "config.h"

// RTC boot counter (definition, with attribute)
RTC_DATA_ATTR int bootCount = 0;

// Define the externs from config.h
const char* DEVICE_NAME = "Test_ws_pcbv2";
const char* DEVICE_UUID = "22593b33";
const char* API_URL     = "https://plants.makeruniverse.de/plants/measurements";
const char* API_KEY     = "vKpsikScqRUt2CdC";

void setup() {
  Serial.begin(115200);
  delay(800);

  ++bootCount;
  Serial.println("Boot #" + String(bootCount));

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
  readBME280(d.temp, d.hum, d.press);
#endif

#if USE_TSL2591
  readTSL(d.lux, d.ir, d.full);
#endif

#if USE_PUMP
  d.pumpSeconds = runPump(d.moistPct);
#endif

  sendData(d);
  goToSleep();
}

void loop() {}
