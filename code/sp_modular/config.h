#pragma once

// ============ FEATURE FLAGS ============
#define USE_BME280    1
#define USE_TSL2591   1
#define USE_PUMP      1

// ============ DEVICE INFO ============
extern const char* DEVICE_NAME;
extern const char* DEVICE_UUID;
extern const char* API_URL;
extern const char* API_KEY;

// ============ SLEEP SETTINGS ============
#define TIME_TO_SLEEP_SEC 3600
#define uS_TO_S_FACTOR 1000000ULL

// ============ PIN CONFIG ============
#define PIN_MOISTURE     1
#define PIN_BATTERY      0
#define PIN_SENSOR_POWER 21
#define PIN_BME_POWER    2
#define PIN_TSL_POWER    3
#define PIN_PUMP         22
#define PIN_SDA          4
#define PIN_SCL          5

// ============ CALIBRATION ============
#define MIN_MOIST_V   0.60f
#define MAX_MOIST_V   2.45f
#define BATT_DIVIDER  2.0f

// Pump settings
#define MOISTURE_THRESHOLD  30.0f
#define PUMP_DURATION_SEC   2

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
};
