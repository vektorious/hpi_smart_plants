# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Smart Plants Starter Kit — an educational IoT project using the SEEED Studio XIAO ESP32-C6 to monitor soil moisture and optionally control automatic watering. The device wakes from deep sleep, reads sensors, posts data via HTTP to `https://plants.makeruniverse.de/plants/measurements`, then sleeps again to conserve battery.

## Build & Upload

There is no Makefile or build script. This is an Arduino IDE project.

**Required setup:**
1. Arduino IDE from arduino.cc
2. Board package: **XIAO_ESP32C6** (via Board Manager)
3. Libraries (via Library Manager):
   - WiFiManager by tzapu
   - Adafruit BME280 (if `USE_BME280 1`)
   - Adafruit TSL2591 (if `USE_TSL2591 1`)

**Upload:** Select board **XIAO_ESP32C6**, select the correct COM port, click Upload.

**Recovery from deep sleep lock** (device won't accept uploads):
1. Unplug, hold BOOT button, plug back in, release after 2 seconds, retry upload.

## Code Architecture

The main firmware lives in `code/sp_modular/`. It follows a modular pattern where feature flags in `config.h` conditionally compile sensor support:

```c
#define USE_BME280   1   // Temperature/humidity/pressure sensor
#define USE_TSL2591  1   // Light sensor
#define USE_PUMP     1   // Automatic watering pump
```

**Execution flow** (`sp_modular.ino`):
1. Wake from deep sleep
2. Power on sensors via GPIO gates
3. Connect to WiFi (WiFiManager opens config AP if no saved credentials)
4. Read moisture (16-sample ADC average on pin A1, powered via GPIO 21)
5. Read battery voltage (ADC on A0 through 2× voltage divider)
6. Optionally read BME280 (I2C 0x76, powered via GPIO 2)
7. Optionally read TSL2591 (I2C, powered via GPIO 3)
8. Optionally activate pump if moisture below threshold (MOSFET on GPIO 22)
9. POST JSON payload to API
10. Enter deep sleep for `TIME_TO_SLEEP_SEC` (default 3600s)

**Key files:**

| File | Responsibility |
|------|---------------|
| `config.h` | All pins, calibration values, feature flags, device credentials |
| `sp_modular.ino` | Entry point; wires together all modules |
| `send_data.ino` | Builds JSON and POSTs to API; conditionally includes fields based on feature flags |
| `moisture.ino` | ADC read + voltage-to-percentage conversion using `MIN_MOIST_V`/`MAX_MOIST_V` |
| `battery.ino` | ADC read multiplied by `BATT_DIVIDER` |
| `wifi.ino` | WiFiManager connection with AP fallback |
| `sleep.ino` | Disables radio, configures timer wakeup, enters deep sleep |
| `bme280.ino` / `tsl2591.ino` / `pump.ino` | Sensor/actuator modules, only compiled when feature flag is set |
| `utils.ino` | `isValidFloat()` — filters NaN before sending to API |

All sensor readings are collected into a `SensorPacket` struct (defined in `config.h`) and passed to `send_data.ino`.

**Simpler monolithic variants** exist in `code/moisture/`, `code/moisture-pump/`, etc. — these are standalone sketches for less complex builds and are not the primary codebase.

## Configuration

All tunable values are in `code/sp_modular/config.h`:

- **Device identity:** `DEVICE_NAME`, `DEVICE_UUID`, `API_KEY` — must match the dashboard
- **Sleep interval:** `TIME_TO_SLEEP_SEC` (default 3600)
- **Moisture calibration:** `MIN_MOIST_V` (wet, ~0.60V) / `MAX_MOIST_V` (dry, ~2.45V) — adjust per sensor
- **Pump:** `MOISTURE_THRESHOLD` (%) and `PUMP_DURATION_SEC`
- **Pin assignments:** all GPIO pins defined here

## Hardware

- **MCU:** XIAO ESP32-C6 (RISC-V, WiFi, <10µA deep sleep)
- **Moisture sensor:** Capacitive (preferred over resistive to avoid corrosion), analog output
- **Optional sensors:** BME280 (temp/humidity/pressure), TSL2591 (lux/IR) — both I2C, individually power-gated
- **Power:** 3× AA or LiPo; GPIO-gated sensor power to minimize idle current
- **Pump:** Diaphragm pump driven via MOSFET (IRLZ14) with separate supply

PCB design files (KiCAD) are in `hardware/pcb/`. Enclosures are in `hardware/3d-print/` (STL/3MF) and `hardware/laser_files/` (SVG for laser cutting). Circuit diagrams (Fritzing) are in `src/`.
