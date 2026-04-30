#pragma once

// ============ FEATURE FLAGS ============
// Set to 1 to enable a module, 0 to disable it.
#define USE_BME280    1   // Temperature, humidity, pressure (I2C)
#define USE_TSL2591   1   // Light sensor — lux + IR (I2C)
#define USE_PUMP      1   // Automatic watering pump via MOSFET

// ============ DEVICE INFO ============
// *** Change these for your device before uploading ***
// Device name and UUID are shown on the dashboard.
// API key is provided at the workshop.
#define DEVICE_NAME  "aisc_plant_bot"
#define DEVICE_UUID  "00000003"
#define API_URL      "https://plants.makeruniverse.de/plants/measurements"
#define API_KEY      "vKpsikScqRUt2CdC"

// ============ SLEEP SETTINGS ============
#define TIME_TO_SLEEP_SEC 3600      // Wake interval in seconds (default: 1 hour)
#define uS_TO_S_FACTOR 1000000ULL

// ============ PIN CONFIG ============
// --- Waveshare ESP32-C6 Zero + smart_plants_breakout_ws-board_rev2 (main build) ---
#define PIN_MOISTURE     1   // A1  — capacitive moisture sensor (ADC)
#define PIN_BATTERY      0   // A0  — battery voltage via 2× divider (ADC)
#define PIN_SENSOR_POWER 21  // GPIO 21 — moisture sensor power gate (PCB rev2 silkscreen incorrectly labels this "D1-powered")
#define PIN_BME_POWER    2   // GPIO 2  — BME280 power gate
#define PIN_TSL_POWER    3   // GPIO 3  — TSL2591 power gate
#define PIN_PUMP         22  // GPIO 22 — pump MOSFET gate
#define PIN_SDA          4   // GPIO 4  — I2C data
#define PIN_SCL          5   // GPIO 5  — I2C clock

// --- SEEED Studio XIAO ESP32-C6 + smart_plants_breakout_rev1 ---
// To build for XIAO: comment out the Waveshare block above and uncomment this one.
// #define PIN_MOISTURE     A1
// #define PIN_BATTERY      A0
// #define PIN_SENSOR_POWER D1
// #define PIN_BME_POWER    D0
// #define PIN_TSL_POWER    D2
// #define PIN_PUMP         D10
// #define PIN_SDA          SDA
// #define PIN_SCL          SCL

// ============ CALIBRATION ============
// Adjust MIN/MAX per sensor batch — measure voltage in wet soil and dry air.
#define MIN_MOIST_V   0.60f  // Voltage when soil is saturated (wet)
#define MAX_MOIST_V   2.45f  // Voltage in open air (dry)
#define BATT_DIVIDER  2.0f   // Voltage divider ratio on the battery ADC pin

// Pump settings
#define MOISTURE_THRESHOLD  30.0f  // Pump triggers below this moisture percentage
#define PUMP_DURATION_SEC   2      // How long to run the pump per cycle (seconds)

// Shared data struct
struct SensorPacket {
  float moistV;
  float moistPct;
  float battV;
  float temp;
  float hum;
  float press;
  float lux;
  uint16_t ir;
  uint16_t full;
  int pumpSeconds;
  int wifiRssi;
};
