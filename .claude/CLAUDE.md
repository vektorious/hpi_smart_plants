# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Smart Plants Starter Kit — an educational IoT project for student workshops. The device monitors
soil moisture, temperature, humidity, light, and battery level, then sends data via HTTP to
`https://plants.makeruniverse.de/plants/measurements`. It wakes from deep sleep, reads sensors,
POSTs the payload, then sleeps again to conserve battery.

**Two supported boards** (same firmware, different GPIO pins in `config.h`):
- **Waveshare ESP32-C6 Zero** + `smart_plants_breakout_ws-board_rev2` — main workshop build
- **SEEED Studio XIAO ESP32-C6** + `smart_plants_breakout_rev1` — alternative build
  (XIAO GPIO pin numbers are not yet confirmed — see TODO in `config.h`)

## Build & Upload

No Makefile or build script. This is an Arduino IDE project.

**Required setup:**
1. Arduino IDE from arduino.cc
2. Board packages (via Boards Manager):
   - **Waveshare build:** ESP32 by Espressif → select "Waveshare ESP32-C6-Zero"
   - **XIAO build:** Seeed Studio XIAO → select "XIAO_ESP32C6"
3. Libraries (via Library Manager):
   - WiFiManager by tzapu
   - Adafruit BME280 (required when `USE_BME280 1`)
   - Adafruit TSL2591 (required when `USE_TSL2591 1`)

**Upload:** Select the correct board and COM port, click Upload.

**Recovery from deep sleep lock** (device won't accept uploads):
1. Unplug, hold BOOT button, plug back in, release after 2 seconds, retry upload.

## Code Architecture

The main firmware is `code/sp_modular/`. Feature flags in `config.h` conditionally compile
sensor support:

```c
#define USE_BME280    1   // Temperature/humidity/pressure sensor
#define USE_TSL2591   1   // Light sensor
#define USE_PUMP      1   // Automatic watering pump
```

**Execution flow** (`sp_modular.ino`):
1. Wake from deep sleep
2. Power on sensors via GPIO gates
3. Connect to WiFi (WiFiManager opens config AP if no saved credentials)
4. Read moisture (16-sample ADC average, powered via GPIO 21)
5. Read battery voltage (ADC through 2× voltage divider)
6. Optionally read BME280 (I2C 0x76, powered via GPIO 2)
7. Optionally read TSL2591 (I2C, powered via GPIO 3)
8. Optionally activate pump if moisture below threshold (MOSFET on GPIO 22)
9. POST JSON payload to API
10. Enter deep sleep for `TIME_TO_SLEEP_SEC` (default 3600s)

**Key files:**

| File | Responsibility |
|------|---------------|
| `config.h` | Feature flags, pin assignments, calibration values |
| `sp_modular.ino` | Entry point; device credentials; wires together all modules |
| `send_data.ino` | Builds JSON and POSTs to API; conditionally includes fields |
| `moisture.ino` | ADC read + voltage-to-percentage conversion |
| `battery.ino` | ADC read multiplied by `BATT_DIVIDER` |
| `wifi.ino` | WiFiManager connection with AP fallback |
| `sleep.ino` | Disables radio, configures timer wakeup, enters deep sleep |
| `bme280.ino` / `tsl2591.ino` / `pump.ino` | Sensor/actuator modules, compiled only when flag is set |
| `utils.ino` | `isValidFloat()` — filters NaN before sending to API |

All sensor readings are collected into a `SensorPacket` struct (defined in `config.h`) and
passed to `send_data.ino`.

Legacy monolithic sketches are in `code/legacy/` — unsupported, breadboard-era only.

## Configuration

- **Device identity:** defined at the top of `sp_modular.ino` — `DEVICE_NAME`, `DEVICE_UUID`,
  `API_KEY` must match the dashboard
- **Sleep interval:** `TIME_TO_SLEEP_SEC` in `config.h` (default 3600)
- **Moisture calibration:** `MIN_MOIST_V` (wet, ~0.60 V) / `MAX_MOIST_V` (dry, ~2.45 V)
- **Pump:** `MOISTURE_THRESHOLD` (%) and `PUMP_DURATION_SEC`
- **Pin assignments:** in `config.h` — active block is Waveshare; XIAO block is commented out
  with a TODO to verify GPIO numbers

## Hardware

- **MCU:** Waveshare ESP32-C6 Zero (primary) or XIAO ESP32-C6 (alternative) — both RISC-V,
  WiFi, deep sleep <10 µA
- **Moisture sensor:** Capacitive (preferred over resistive to avoid corrosion), analog output
- **Sensors on PCB:** BME280 (temp/humidity/pressure), TSL2591 (lux/IR) — I2C, power-gated
- **Power:** 3× AA or Li-ion; sensor power gated via GPIO to minimise idle current
- **Pump:** Diaphragm pump driven via MOSFET on the PCB (IRLZ14)

PCB design files (KiCAD) are in `hardware/pcb/`. Enclosures are in `hardware/3d-print/`
(STL/OBJ) and `hardware/laser_files/` (SVG). See `hardware/pcb/README.md` for ordering info.
