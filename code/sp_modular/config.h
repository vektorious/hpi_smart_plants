#pragma once

#include <Arduino.h>

// =====================================================================
//  Smart Plants — configuration
//
//  As of the runtime-config rework, the values that a user may want to
//  change per device (feature toggles, device identity, sleep interval,
//  calibration, pump behaviour) are stored in NVS/Preferences and edited
//  from the WiFiManager setup portal — NOT recompiled.
//
//  The DEFAULT_* macros below are only the factory defaults, applied on
//  first boot (or after a settings reset). Pin assignments stay
//  compile-time because they depend on the physical board.
// =====================================================================

// ============ FEATURE DEFAULTS ============
// Initial state of each module. All sensor code is always compiled in;
// these just set the runtime default in settings.useBme / useTsl / usePump.
#define DEFAULT_USE_BME280   true   // Temperature, humidity, pressure (I2C)
#define DEFAULT_USE_TSL2591  true   // Light sensor — lux + IR (I2C)
#define DEFAULT_USE_PUMP     true   // Automatic watering pump via MOSFET

// ============ DEVICE INFO DEFAULTS ============
// Editable in the setup portal. Device name and ID are shown on the
// dashboard. API key is provided at the workshop.
#define DEFAULT_DEVICE_NAME  "SmartPlant"
// Prefix for the generated device_id (the chip-derived suffix is appended).
// Keep to [A-Za-z0-9_-] — the API rejects anything else.
#define DEFAULT_DEVICE_ID_PREFIX "smartplant"
#define DEFAULT_PROJECT      ""   // Groups devices on the dashboard; empty = ungrouped. Set per workshop in the portal (e.g. "workshop-2026").
#define DEFAULT_API_URL      "https://diy-sensor.org/sensor/measurement"
#define DEFAULT_API_KEY      "public-test-key"

// ============ SLEEP DEFAULTS ============
#define DEFAULT_TIME_TO_SLEEP_SEC 3600      // Wake interval in seconds (default: 1 hour)
#define uS_TO_S_FACTOR 1000000ULL

// ============ CALIBRATION DEFAULTS ============
// Adjust MIN/MAX per sensor batch — measure voltage in wet soil and dry air.
#define DEFAULT_MIN_MOIST_V   0.60f  // Voltage when soil is saturated (wet)
#define DEFAULT_MAX_MOIST_V   2.45f  // Voltage in open air (dry)
#define DEFAULT_BATT_DIVIDER  2.0f   // Voltage divider ratio on the battery ADC pin

// ============ PUMP DEFAULTS ============
#define DEFAULT_MOISTURE_THRESHOLD  50.0f  // Pump triggers below this moisture percentage
#define DEFAULT_PUMP_DURATION_SEC   2      // How long to run the pump per cycle (seconds)

// ============ PIN CONFIG (compile-time — board dependent) ============
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

// ============ RUNTIME SETTINGS ============
// Persisted in NVS/Preferences; edited from the setup portal.
// Defined in settings.ino; loaded once at boot via loadSettings().
struct Settings {
  char     deviceName[32];
  char     deviceId[32];        // public device_id on the dashboard; not user-editable
  char     project[48];         // dashboard grouping; empty = ungrouped
  char     apiKey[48];
  char     apiUrl[128];
  uint32_t sleepSec;
  float    minMoistV;
  float    maxMoistV;
  float    battDivider;
  float    moistureThreshold;   // %
  uint32_t pumpDurationSec;
  bool     useBme;
  bool     useTsl;
  bool     usePump;
};

extern Settings settings;

void loadSettings();                 // populate `settings` from NVS (or defaults on first boot)
void saveSettings();                 // persist `settings` to NVS
void resetSettingsToDefaults();      // overwrite `settings` with DEFAULT_* values (RAM only; call saveSettings() to persist)

// ============ SHARED DATA STRUCT ============
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

// Functions taking SensorPacket by reference need explicit prototypes: the
// Arduino auto-prototype generator skips reference parameters.
SensorPacket readAllSensors();              // sp_modular.ino
int          sendData(const SensorPacket &d);  // send_data.ino — HTTP code, -1 if offline
