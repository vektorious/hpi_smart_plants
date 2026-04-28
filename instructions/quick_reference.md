# Smart Plants — Quick Build Reference

## 1. Solder the PCB

Orientation-sensitive components must match the silkscreen. Resistors are not orientation-sensitive.

| Ref | Component | Value | Notes |
|-----|-----------|-------|-------|
| R1, R2 | Resistor | 220 kΩ | Battery voltage divider |
| R3 | Resistor | 10 kΩ | MOSFET gate pull-down |
| R4 | Resistor | 22 Ω | MOSFET gate protection |
| R5, R6 | Resistor | 4.7 kΩ | I2C pull-up |
| D1 | Diode | 1N4001 | Flyback — stripe (cathode) toward pump + |
| D2 | Diode | 1N5817 | Schottky — stripe per silkscreen |
| C2 | Capacitor | 220 µF | Electrolytic — long leg to + hole |
| C3 | Capacitor | 22 µF | Electrolytic — long leg to + hole |
| Q1 | MOSFET | IRLZ14 | Flat side matches silkscreen outline |
| J3, J5, J6, J11 | JST XH 4-pin | — | I2C connectors |
| J9 | JST XH 3-pin | — | Moisture sensor |
| J7, SW1 | JST XH 2-pin | — | Battery, switch |
| J10 | JST XH 2-pin | — | Pump |
| J4 | JST PH 2-pin | — | USB-C power (2.0 mm pitch — smaller than XH) |
| J1, J2 | 9-pin socket | — | Microcontroller — insert Waveshare board after soldering |

Suggested order: resistors + diodes → JST connectors → pin sockets → capacitors + MOSFET.

> **PCB bug:** The moisture connector (J9) is labelled "D1-powered" — the correct GPIO is 21. Ignore the label.

---

## 2. Prepare Sensors and Actuators

**Moisture sensor cable** — cut 3-core cable to length, crimp 3-pin JST XH on both ends.
Wire order: GND (black) — VCC (red) — SIGNAL (yellow). Discard the short cable that ships with the sensor.
Insert sensor into housing, screw on lid.

**Pump** — solder 2-core cable to pump terminals. Insert pump into housing, fit PG7 cable gland.
Crimp 2-pin JST XH on free end.

**BME280** — solder pre-crimped cable. Wire order: VCC (black) — GND (red) — SCL (yellow) — SDA (white).
⚠ Black = VCC, red = GND (reversed from convention). Insert into Stevenson screen housing, thread cable through tube.

**TSL2591** — solder pre-crimped cable (wires away from sensor face). Same wire order as BME280.
Hot-glue sensor into housing bottom, sensor facing up. Glue half a ping-pong ball into the groove as diffuser.

**Switch** — solder 2-core cable, crimp 2-pin JST XH. Wire order doesn't matter.

**Battery pack** — crimp 2-pin JST XH. Wire order: VCC (black) — GND (red).

---

## 3. Assemble the Enclosure

1. Slot the laser-cut side panels together; add corner pieces while joining — they won't fit in later.
2. Drill an 8 mm hole for sensor cables if not pre-cut.
3. Mount BME280 through the lid hole (distance piece + locking piece). Hot-glue TSL2591 housing to lid.
   Keep the two sensors as far apart as possible.
4. Insert switch and USB-C connector into the housing. Feed pump and moisture sensor cables in.
5. Insert battery pack into the PCB mount. Screw PCB to mount. Insert Waveshare board into socket.
6. Stick velcro to the PCB mount bottom and matching strip inside housing. Fix mount in place.
7. Connect all components to the labelled PCB connectors. BME280 → "I2C D2-powered", TSL2591 → "I2C D3-powered".

---

## 4. Program

1. Install **Arduino IDE**. Add board URL in Preferences:
   `https://dl.espressif.com/dl/package_esp32_index.json`
2. Boards Manager → install **ESP32 by Espressif Systems**. Select **Waveshare ESP32-C6-Zero**.
3. Library Manager → install: **WiFiManager** (tzapu), **Adafruit BME280**, **Adafruit TSL2591**, **Adafruit Unified Sensor**.
   Click "Install All" when prompted for dependencies.
4. Open `code/sp_modular/sp_modular.ino`. Edit `config.h`:
   ```cpp
   #define DEVICE_NAME  "my-plant"        // your plant name
   #define DEVICE_UUID  "00000000"        // 8-character ID from dashboard
   #define API_KEY      "vKpsikScqRUt2CdC"
   ```
5. Select port → Upload. If upload fails: unplug, hold BOOT, plug in, release after 2 s, retry.

---

## 5. Connect to Wi-Fi

On first boot the device opens a hotspot named **`<DEVICE_NAME>-Setup`**. Connect to it (no password),
enter your Wi-Fi credentials in the page that opens. The device saves them and connects automatically from then on.

---

## 6. Calibrate Moisture Sensor

Upload the test sketch (or open Serial Monitor after the main sketch):
- **Dry (0%):** hold sensor in open air → note voltage → set as `MAX_MOIST_V` in `config.h`
- **Wet (100%):** submerge prongs in water → note voltage → set as `MIN_MOIST_V` in `config.h`

Default values: `MIN_MOIST_V 0.60`, `MAX_MOIST_V 2.45`. Re-upload after changing.

---

## 7. Final Test

Confirm a reading appears on [plants.makeruniverse.de](https://plants.makeruniverse.de) while on USB.
Disconnect USB, flip power switch on — device should wake, send a reading, and sleep.
Screw the lid on. Done.

> Switching off and on triggers an immediate reading — no need to wait an hour.
