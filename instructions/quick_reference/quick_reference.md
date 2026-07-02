# Smart Plants — Quick Build Reference

## 1. Solder the PCB

Orientation-sensitive components must match the silkscreen. Resistors are not orientation-sensitive.

| Ref             | Component    | Value  | Notes                                                      |
| --------------- | ------------ | ------ | ---------------------------------------------------------- |
| R1, R2          | Resistor     | 220 kΩ | Battery voltage divider                                    |
| R3              | Resistor     | 10 kΩ  | MOSFET gate pull-down                                      |
| R4              | Resistor     | 22 Ω   | MOSFET gate protection                                     |
| R5, R6          | Resistor     | 4.7 kΩ | optional, I2C pull-up                                      |
| D1              | Diode        | 1N4001 | orientation sensitive, Flyback — stripe per silkscreen     |
| D2              | Diode        | 1N5817 | orientation sensitive, Schottky — stripe per silkscreen    |
| J3, J5, J6, J11 | JST XH 4-pin | —      | I2C connectors, you only need the first two from the top.  |
| J9              | JST XH 3-pin | —      | Moisture sensor                                            |
| J7, SW1         | JST XH 2-pin | —      | Battery, switch                                            |
| J10             | JST XH 2-pin | —      | Pump                                                       |
| J4              | JST PH 2-pin | —      | USB-C power (2.0 mm pitch — smaller than XH)               |
| J1, J2          | 9-pin socket | —      | Microcontroller — insert Waveshare board after soldering   |
| C2              | Capacitor    | 220 µF | orientation sensitive, Electrolytic — long leg to + hole   |
| C3              | Capacitor    | 22 µF  | orientation sensitive, Electrolytic — long leg to + hole   |
| Q1              | MOSFET       | IRLZ14 | Flat side matches silkscreen outline                       |

Suggested soldering order: resistors + diodes → JST connectors → pin sockets → capacitors + MOSFET.

> **legacy PCB bug:** On old PCB versions the moisture connector (J9) is labelled "D1-powered" — the correct GPIO is 21. Ignore the label.

---

## 2. Prepare Sensors and Actuators

**Moisture sensor cable** — cut 3-core cable to length, crimp 3-pin JST XH on both ends.
Wire order: SIGNAL (yellow) — GND (black) — VCC (red). Discard the short cable that ships with the sensor.
Insert sensor into housing, screw on lid.

**Pump** — solder 2-core cable to pump terminals. Insert pump into housing, fit PG7 cable gland.
Crimp 2-pin JST XH on free end.

**BME280** — solder pre-crimped cable. Wire order: VCC (black) — GND (red) — SCL (yellow) — SDA (white).
⚠ Black = VCC, red = GND (reversed from convention). Insert into Stevenson screen housing, thread cable through tube.

**TSL2591** — solder pre-crimped cable (wires away from sensor face). Same wire order as BME280.
Hot-glue sensor into housing bottom, sensor facing up. Glue half a ping-pong ball into the groove as diffuser.

**Switch** — solder 2-core cable, crimp 2-pin JST XH. Wire order doesn't matter.

**Battery pack** — crimp 2-pin JST XH. Wire order: VCC (red) — GND (black).

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

## 4. Flash the Firmware (from the browser)

Use **Chrome, Edge, or Opera** and a USB-C **data** cable.

1. Put the board in flashing mode: **unplug → hold BOOT → plug in → release after ~2 s**
   (the firmware sleeps quickly and drops the USB port, so start it in the bootloader).
2. Open **[vektorious.github.io/hpi_smart_plants](https://vektorious.github.io/hpi_smart_plants/)**.
3. **Connect** → pick the `USB JTAG/serial debug unit` port → **Install Smart Plants Firmware**.

> Building from source instead? See [`firmware_from_source.md`](../firmware_from_source.md).

---

## 5. Configure (setup portal)

On first boot the device opens a Wi-Fi hotspot named **`SmartPlant-XXXXXXXX-Setup`** (unique per board).

1. Connect to it (no password); the setup page opens automatically (or browse to `192.168.4.1`).
2. Set your **Wi-Fi**, and optionally the device name, which sensors/pump are enabled, and
   calibration. The API key is pre-filled. **Save.**

> Reopen the setup page later to change anything: **double-press the reset button**.

---

## 6. Check & Calibrate

Open **Live Sensor Readings** in the portal:

- Values refresh every 10 s; the Wi-Fi badge shows the connection.
- **Send to API** → green **200 OK** confirms Wi-Fi + API key + server.
- A sensor showing **"not detected"** → check that sensor's cable / wire order.

**Calibrate the moisture sensor** from the same page — read the *moisture voltage*:
- **In open air** (dry, 0 %) → enter as *Moisture dry voltage*.
- **Prongs in water** (wet, 100 %) → enter as *Moisture wet voltage*.

Save in the portal — no reflash. Defaults: wet 0.60 V, dry 2.45 V.

---

## 7. Finish & Deploy

Click **Finish setup** → the device takes a reading and starts its hourly cycle. Confirm it
appears on [plants.makeruniverse.de](https://plants.makeruniverse.de). Disconnect USB, flip the
power switch on, screw the lid on. Done.

> A single reset triggers an immediate reading; a double-press reopens the setup page.
