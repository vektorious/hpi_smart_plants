# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Smart Plants Starter Kit — an educational IoT project for student workshops. The device monitors
soil moisture, temperature, humidity, light, and battery level, then sends data via HTTP to
`https://plants.makeruniverse.de/plants/measurements`. It wakes from deep sleep, reads sensors,
POSTs the payload, then sleeps again to conserve battery.

Per-device settings (WiFi, device identity, calibration, feature toggles) are configured at
runtime through the WiFiManager setup portal — see "Configuration" — not by editing source.
Students can install the firmware from a browser via the web flasher in `web-flasher/`.

**Two supported boards** (same firmware, different GPIO pins in `config.h`):
- **Waveshare ESP32-C6 Zero** + `smart_plants_breakout_ws-board_rev2` — main workshop build
- **SEEED Studio XIAO ESP32-C6** + `smart_plants_breakout_rev1` — alternative build
  (XIAO pin labels in `config.h` use Arduino symbolic names D0/D1/A0/etc. — not yet verified
  on real hardware)

## Build & Upload

No Makefile or build script. This is an Arduino IDE project.

**Required setup:**
1. Arduino IDE from arduino.cc
2. Board packages (via Boards Manager):
   - **Waveshare build:** ESP32 by Espressif → select "Waveshare ESP32-C6-Zero"
   - **XIAO build:** Seeed Studio XIAO → select "XIAO_ESP32C6"
3. Libraries (via Library Manager):
   - WiFiManager by tzapu
   - Adafruit BME280, Adafruit TSL2591 (both always required — all sensor code now compiles
     in unconditionally; the sensors are enabled/disabled at runtime, not at compile time)
   - Adafruit Unified Sensor (dependency of both Adafruit sensor libraries — install via
     "Install All" when prompted, or manually)

**Partition scheme:** set **Tools → Partition Scheme → "Huge APP (3MB No OTA/1MB SPIFFS)"**.
The firmware no longer fits the default partition (all sensors + the portal web server are
always compiled in). Canonical arduino-cli target:
`esp32:esp32:waveshare_esp32_c6_zero:PartitionScheme=huge_app`.

**Upload:** Select the correct board and COM port, click Upload. Alternatively, flash a merged
binary from the browser via `web-flasher/` (ESP Web Tools; Chrome/Edge/Opera, HTTPS) — see
`web-flasher/README.md`.

**Recovery from deep sleep lock** (device won't accept uploads):
1. Unplug, hold BOOT button, plug back in, release after 2 seconds, retry upload.

## Code Architecture

The main firmware is `code/sp_modular/`. All sensor code compiles in unconditionally; features
are toggled at runtime via `settings.useBme` / `useTsl` / `usePump` (persisted in NVS). The
`DEFAULT_*` macros in `config.h` are only the factory defaults applied on first boot.

**Execution flow** (`sp_modular.ino`):
1. Wake from deep sleep
2. Load settings from NVS (`loadSettings()`); detect double reset (`detectDoubleReset()`)
3. Quick-connect to WiFi with saved credentials (`setupWiFi()`)
4. If double reset OR no valid credentials → open the commissioning portal
   (`runCommissioningPortal()`): edit settings, view live sensor readings, and test the API.
   Runs in AP+STA and stays open until the user taps "Finish" or a 5-min timeout
   (defeatable via the live page's keep-awake toggle)
5. Read moisture (16-sample ADC average, powered via GPIO 21)
6. Read battery voltage (ADC through 2× voltage divider)
7. If enabled, read BME280 (I2C 0x76, powered via GPIO 2)
8. If enabled, read TSL2591 (I2C, powered via GPIO 3)
9. If enabled, activate pump when moisture below threshold (MOSFET on GPIO 22)
10. POST JSON payload to API
11. Enter deep sleep for `settings.sleepSec` (default 3600s)

**Key files:**

| File | Responsibility |
|------|---------------|
| `config.h` | `DEFAULT_*` factory values, pin assignments, `Settings` + `SensorPacket` structs |
| `settings.ino` | `Settings` struct persisted in NVS/Preferences; load/save/reset |
| `sp_modular.ino` | Entry point; boot flow + `readAllSensors()` helper |
| `send_data.ino` | Builds JSON and POSTs to API; includes fields per `settings.*` toggles |
| `moisture.ino` | ADC read + voltage-to-percentage conversion |
| `battery.ino` | ADC read multiplied by `settings.battDivider` |
| `wifi.ino` | Quick STA connect; returns whether it succeeded |
| `portal.ino` | WiFiManager commissioning portal: editable settings + live `/sensors.json` page |
| `resetdetect.ino` | NVS-backed double-reset detector (arm on manual reset, self-disarm after a ~3 s window; timer wakes skip it) |
| `sleep.ino` | Disables radio, configures timer wakeup, enters deep sleep |
| `bme280.ino` / `tsl2591.ino` / `pump.ino` | Sensor/actuator modules (always compiled; gated at runtime) |
| `utils.ino` | `isValidFloat()` — filters NaN before sending to API |

All sensor readings are collected into a `SensorPacket` struct (defined in `config.h`) and
passed to `send_data.ino`.

Legacy monolithic sketches are in `code/legacy/` — unsupported, breadboard-era only.

## Configuration

Settings are edited at **runtime in the WiFiManager setup portal**, then persisted to
NVS/Preferences (`settings.ino`). Editing `config.h` only changes the factory defaults used on
first boot / after a reset. This is what lets a single web-flashed binary be reconfigured
without recompiling.

**Opening the portal:** it opens automatically when there are no saved WiFi credentials, or on
a **double reset** (two presses of the reset button within one boot cycle). It runs for 5
minutes, and the live-sensor page has a "keep awake" toggle that defeats the timeout for
debugging.

**In the portal you can set:** WiFi credentials; device identity (name, UUID, API key/URL);
sleep interval; moisture calibration (wet/dry voltage); battery divider; pump threshold and
duration; and enable/disable each sensor and the pump. The **"Live Sensor Readings"** menu page
refreshes every 10 s so students can confirm connected sensors work, includes a **"Send to API"**
button that POSTs live readings to test the connection (green 200 = OK), and a **WiFi status
badge**. The menu also has **"Finish setup"** (proceed to normal operation) and a **factory
reset** (restore `DEFAULT_*` values + clear WiFi).

- **Device identity:** on first boot `settings.ino` generates a unique name and UUID from the
  chip's factory MAC (`ESP.getEfuseMac()`) — e.g. `SmartPlant-1a2b3c4d` — so many students'
  setup APs and dashboard entries don't collide. Stable per board; editable in the portal.
  `DEFAULT_DEVICE_NAME` is the name prefix; `DEFAULT_DEVICE_UUID` is unused.
- **Factory defaults:** the `DEFAULT_*` macros in `config.h`. The API key is not sensitive and
  can stay in the code as a default.
- **Pin assignments:** compile-time in `config.h` (board-dependent) — active block is Waveshare;
  XIAO block is commented out.
- **Settings storage:** NVS namespace `sp`; the double-reset flag uses namespace `drd`. A
  `version` key in `settings.ino` re-seeds defaults if the `Settings` layout changes.

## Hardware

- **MCU:** Waveshare ESP32-C6 Zero (primary) or XIAO ESP32-C6 (alternative) — both RISC-V,
  WiFi, deep sleep <10 µA
- **Moisture sensor:** Capacitive (preferred over resistive to avoid corrosion), analog output
- **Sensors on PCB:** BME280 (temp/humidity/pressure, I2C 0x76), TSL2591 (lux/IR, I2C 0x29)
  — both power-gated via GPIO
- **Power:** 3× AA; sensor power gated via GPIO to minimise idle current
- **Pump:** Diaphragm pump driven via IRLZ14 MOSFET on the PCB

**Known PCB bug — ws-board rev2 silkscreen:** The moisture sensor power connector is labelled
"D1-powered" on the board. The correct GPIO is 21. This is a labelling error only; wiring and
firmware are correct.

**TSL2591 quirk:** `Wire.begin()` must be called a second time (with correct SDA/SCL pins)
immediately before `tsl.begin()` due to Adafruit_I2CDevice internals resetting the bus.

PCB design files (KiCAD) are in `hardware/pcb/`. Enclosures are in `hardware/3d-print/`
(STL/OBJ) and `hardware/laser_files/` (SVG). See `hardware/pcb/README.md` for ordering info.

## Documentation

- `instructions/build_instructions.md` — main student-facing build guide (start here); firmware
  install now defaults to the web flasher, then runtime configuration via the setup portal
- `instructions/firmware_from_source.md` — build/upload from Arduino IDE (huge_app partition) and
  standalone component test sketches — the "bare code" path, split out of the main guide
- `instructions/background_information.md` — sensor theory, system architecture, design rationale
- `instructions/quick_reference/` — printable quick reference (md, html, pdf) with all build
  steps and PCB BOM; QR code at `img/qr_build_instructions.png` links to the full instructions.
  To regenerate the PDF after editing the HTML (rendered with headless Chromium — LibreOffice
  mis-indents `<h2>`/`<th>` and ignores the CSS grid):
  `cd instructions/quick_reference && chromium --headless=new --no-pdf-header-footer --print-to-pdf=quick_reference.pdf "file://$PWD/quick_reference.html"`
- `instructions/legacy/` — old breadboard-era guides, unsupported
- `web-flasher/` — browser-based firmware installer (ESP Web Tools), live at
  <https://vektorious.github.io/hpi_smart_plants/>. Published from the **`gh-pages`** branch
  (mirrors this folder at its root; the merged `.bin` lives there, not on `main`). See its
  `README.md` for how to build/update the `.bin` and republish.
- The server at `plants.makeruniverse.de` runs FastAPI → PostgreSQL; Grafana dashboard is public
  at the same domain
