# Build Instructions — Waveshare ESP32-C6 Zero + PCB

This guide covers the **main workshop build**: the Waveshare ESP32-C6 Zero on the
`smart_plants_breakout_ws-board_rev2` PCB powered by 3 AA batteries. All sensors (BME280, TSL2591) and the
pump circuit are integrated on the PCB — no breadboard or loose resistors needed.

---

> **Known issue — PCB rev2 silkscreen:** The moisture sensor power connector is labelled
> **"D1-powered"** on the board, but the correct GPIO is **21**. This is a labelling error
> on the PCB and does not affect functionality — just ignore the label and plug the connector
> in as shown in the instructions.

---

## 1. Collect All Parts

Get all parts together before you start. The PCB replaces the breadboard, jumper wires,
and discrete resistors from the old build.

**Electronics**
- Waveshare ESP32-C6 Zero microcontroller
- Smart Plants Breakout PCB (ws-board rev2) + all components
- Capacitive soil moisture sensor + 3-core cable
- BME280 sensor module (temperature, humidity, pressure) + pre-crimped 4-core cable
- TSL2591 sensor module (light) + pre-crimped 4-core cable
- Mini diaphragm pump + 2-core cable
- Battery pack (3× AA)
- USB-C connector with cable
- On/Off Switch

**Mechanical**
- 3D-printed battery housing and PCB mount
- 3D-printed moisture sensor housing + lid
- 3D-printed pump housing
- 3D-printed BME280 housing (Stevenson Screen)
- 3D-printed TSL2591 housing + half ping pong ball as diffusor
- 3D-printed cable plugs (2x)
- Laser-cut enclosure
- 12× wood screws
- Velcro tape
- PG7 cable gland (for pump cable)
- Tubes (pump inlet/outlet)

**Tools**
- Soldering iron + solder
- Screwdriver
- Hot-glue gun (or other glue)

---

## 2. Prepare Parts

Several parts need preparation before assembly. These steps can be done in any order —
if a tool you need is in use, move on to another step.

### 2.1 Prepare the PCB

Solder all components on the PCB (suggested order below). Component orientation is indicated by the silkscreen markings. All components but the resistors are orientation sensitive!

| Reference | Component | Qty | Value | Notes |
|-----------|-----------|-----|-------|-------|
| R1, R2 | Resistor | 2 | 220 kΩ | Battery voltage divider |
| R3 | Resistor | 1 | 10 kΩ | MOSFET gate pull-down |
| R4 | Resistor | 1 | 22 Ω | MOSFET gate protection |
| R5, R6 | Resistor | 2 | 4.7 kΩ | I2C pull-up |
| D1 | Diode | 1 | 1N4001 | Flyback diode — stripe (cathode) faces pump + |
| D2 | Diode | 1 | 1N5817 | Schottky diode — stripe (cathode) per silkscreen |
| C2 | Capacitor | 1 | 220 µF | Electrolytic — long leg to + hole |
| C3 | Capacitor | 1 | 22 µF | Electrolytic — long leg to + hole |
| Q1 | MOSFET | 1 | IRLZ14 | Flat side matches silkscreen outline |
| J3, J5, J6, J11 | I2C JST connector | 4 | 4-pin JST XH | |
| J9 | Moisture sensor connector | 1 | 3-pin JST XH | |
| J7, SW1 | Battery / switch connector | 2 | 2-pin JST XH | |
| J10 | Pump connector | 1 | 2-pin JST XH | |
| J4 | USB-C power connector | 1 | 2-pin JST PH | Smaller pitch (2.0 mm) than the XH connectors |
| J1, J2 | Microcontroller socket | 2 | 9-pin pin socket | Solder to PCB; insert the Waveshare board after |

Resistors are not orientation-sensitive. All other components are — match the silkscreen carefully.

Suggested soldering order:
1. Resistors, Diodes
2. JST connectors. Tip: place all connectors, put a paper on it and try to flip the PCB holding all connectors in place with the paper. Solder the connector pins on the back
3. Microcontroller socket
4. Capacitors, Mosfet
5. optional: pin header to expose microcontroller pins

> **First time soldering?** Ask a workshop helper for a quick demo. A cold solder joint here will cause intermittent problems that are hard to debug later.

<img src="../img/wv_pcb_soldering.jpg" height="400">

### 2.2 Crimp the Moisture Sensor Cable

The moisture sensor uses a custom cable with a JST connector on either side

1. Cut 3-core cable to preferred length
2. Crimp **3-pin JST PH** connectors on both ends.
3. Wire order: **GND (black) — VCC (red) — SIGNAL (yellow)** . This must match the sensor
   connector and the PCB header labels.

<img src="../img/sensor_connectors_JST.png" height="120">

### 2.3 Assemble the Moisture Sensor

1. Discard the short cable that ships with the sensor — use the cable you just crimped.
2. Connect the sensor to the cable.
3. Insert the sensor into the 3D-printed housing and screw on the lid.
4. Push the cable jacket into the housing hole to seal it against dirt and moisture.

<img src="../img/sensor_connection.png" height="200"> <img src="../img/sensor_housing.png" height="200"> <img src="../img/sensor_final.png" height="200">

### 2.4 Prepare the Pump

1. Solder a two-core cable to the pump terminals.
2. Insert the pump into the 3D-printed pump housing.
3. Thread the cable through the housing hole, fit the PG7 cable gland, and screw it tight.
4. Crimp a **2-pin JST PH** connector on the free cable end.

<img src="../img/2-pin_JST_connector.jpg" height="200"> <img src="../img/pump_setup.jpg" height="200">

### 2.5 Prepare the BME280 Sensor

1. Solder the pre-crimped cable to the sensor.
2. Wire order: **VCC (black) — GND (red) — SCL (yellow) — SDA (white)**
3. Insert the sensor into the 3D-printed mini Stevenson screen.
4. Thread the cable through the threaded tube and screw it into the Stevenson screen.

> **Note:** The black/red wire colours are swapped relative to convention — black is VCC and red is GND. This is a quirk of the pre-crimped cables. Double-check against the labels above before soldering.

<img src="../img/bme_insertion.jpg" height="200"> <img src="../img/bme_assembled.jpg" height="200">
### 2.6 Prepare the TSL2591 Sensor

1. Solder the pre-crimped cable to the sensor.
2. Wire order: **VCC (black) — GND (red) — SCL (yellow) — SDA (white)**
3. Thread the cables through the housing bottom and hot-glue the sensor onto it. The sensor should face upwards.
4. Cut a ping-pong ball in half and glue it onto the sensor housing as a diffuser. It should fit tightly into the groove.

> **Note:** The black/red wire colours are swapped relative to convention — black is VCC and red is GND. This is a quirk of the pre-crimped cables. Double-check against the labels above before soldering.

The ping-pong ball half acts as a diffuser: it scatters incoming light so the sensor measures ambient light from a wide angle rather than a narrow beam from one direction. This makes readings less sensitive to the exact orientation of the housing and reduces the effect of direct sunlight hitting the sensor from a single spot. The same principle is used in professional lux meters and pyranometers.

<!-- TODO: add TSL2591 assembly images -->

### 2.7 Prepare the Switch

1. Solder two wires to the switch. 
2. Add a 2-pin JST connector. 
3. Wire order: does not matter in this case but to stay consistent: VCC (black), GND (red)

### 2.8 Prepare the battery pack

1. Add a 2-pin JST connector to the battery cables
2. Wire order: VCC (black), GND (red)

### 2.9 Assemble the Laser-Cut Enclosure

Assemble the housing panels before anything goes inside — it is much harder to add panels
after components are mounted.

1. Slot the side panels together. Add the small corner pieces **while joining the sides**,
   not after — they won't fit in later.
2. If the sensor cable hole is not pre-cut, drill one with an **8 mm bit**. For a tighter
   fit use 5 mm, but you may need to thread the cable before crimping.

---

## 3. Assembly

### 3.1 Mount Sensors to the Lid

1. BME280: slide the distance piece onto the threaded tube, insert it through the hole in the housing lid, and fix it with the locking piece on the other side.
2. Hot-glue the TSL2591 housing to the lid.

> Keep as much distance between the two sensors as possible — the Stevenson screen can otherwise cast a shadow on the TSL2591.

<img src="../img/bme+tsl_lid.jpg" height="300">

### 3.2 Prepare the Housing

1. Insert the switch into the housing. Make sure it is switched off (flipped to the O-marked side).
2. Insert the USB-C connector into the housing.
3. Feed the pump and moisture sensor cables into the housing.
4. Optional: secure both cables using the 3D-printed cable plugs.

### 3.3 Add the PCB Mount

1. Insert the battery pack into the battery/PCB mount with the switch facing outward.
2. Screw the PCB to the PCB mount.
3. Insert the Waveshare microcontroller into the PCB socket.
4. Attach a small strip of velcro (~2 cm) to the bottom of the PCB mount and a matching strip inside the housing. Keeping it short makes it easier to remove later.
5. You can now fix the battery/PCB mount inside the housing — but wait until after programming (Section 4).

<img src="../img/pcb_mount+housing.jpg" height="300">

### 3.4 Connect Components to the PCB

The PCB has labelled connectors for every component. Match the connector to its label on
the silkscreen. Connect the BME280 to "I2C D2-powered" and the TSL2591 to "I2C D3-powered".

TODO: add image here

---

## 4. Program the Microcontroller

### 4.1 Install Arduino IDE and Board Package

If you haven't set up Arduino IDE yet:

1. Download and install [Arduino IDE](https://www.arduino.cc/en/software).
2. Open **File > Preferences** and add this URL to *Additional Boards Manager URLs*:
   `https://dl.espressif.com/dl/package_esp32_index.json`
3. Go to **Tools > Board > Boards Manager**, search for `esp32`, and install the
   **ESP32 by Espressif Systems** package.
4. Select board: **Tools > Board > ESP32 Arduino > Waveshare ESP32-C6-Zero**
   (or search for "ESP32-C6-Zero" in the board list).

### 4.2 Install Required Libraries

In **Tools > Manage Libraries**, search for and install:

| Library | Author |
|---------|--------|
| WiFiManager | tzapu |
| Adafruit BME280 Library | Adafruit |
| Adafruit TSL2591 Library | Adafruit |
| Adafruit Unified Sensor | Adafruit |

> When installing the Adafruit BME280 or TSL2591 libraries, the Library Manager will ask
> whether to install missing dependencies. Click **Install All** — this installs
> Adafruit Unified Sensor automatically.

<img src="../img/install_wm.png" height="400">

### 4.3 Upload the Code

1. Open `code/sp_modular/sp_modular.ino` in Arduino IDE.
2. Open `code/sp_modular/config.h` and fill in your device credentials near the top:

```cpp
#define DEVICE_NAME  "my-plant"   // choose a unique name
#define DEVICE_UUID  "00000000"   // 8-character ID from the dashboard
#define API_KEY      "vKpsikScqRUt2CdC"  // provided at the workshop
```

3. If you are not using all sensors, set unused flags to `0` in the same file:

```cpp
#define USE_BME280    1   // set to 0 if no BME280 connected
#define USE_TSL2591   1   // set to 0 if no TSL2591 connected
#define USE_PUMP      1   // set to 0 if no pump connected
```

4. Connect the Waveshare board to your computer via USB-C.
5. Select the correct port: **Tools > Port**.
6. Click **Upload**.

> **Upload fails?** The board may be stuck in deep sleep from a previous upload.
> 1. Unplug the board.
> 2. Hold the **BOOT** button.
> 3. Plug back in while holding BOOT.
> 4. Release after 2 seconds, then retry Upload.

### 4.4 Connect to Wi-Fi

The firmware uses WiFiManager. On first boot (or when no saved network is found):

1. The device starts a temporary Wi-Fi access point named **`<DEVICE_NAME>-Setup`**
   (e.g. `my-plant-Setup` if you used the default name).
2. Connect to it from your phone or laptop — no password required.
3. A configuration page opens automatically. Enter your Wi-Fi credentials.
4. The device connects, saves the credentials, and starts measuring.

From the next boot onward it connects automatically. Your device will appear on the
dashboard at [plants.makeruniverse.de](https://plants.makeruniverse.de).

> **Tip:** For battery-powered operation, keep `TIME_TO_SLEEP_SEC` at 3600 (1 hour).
> This gives a good balance between data frequency and battery life.

---

## 5. Troubleshooting

Use the sketches below to verify individual components if something does not work as
expected. Upload each sketch separately, then open the **Serial Monitor** at **115200 baud**.

### 5.1 Test the Moisture Sensor

```cpp
const int moisturePin   = 1;   // GPIO 1 (A1) on Waveshare
const int moisturePower = 21;  // GPIO 21 — sensor power gate

const float minMoistureVoltage = 0.60;
const float maxMoistureVoltage = 2.45;

float readMoistureVoltage() {
  uint32_t sum = 0;
  for (int i = 0; i < 16; i++) sum += analogReadMilliVolts(moisturePin);
  return sum / 16.0 / 1000.0;
}

float moistureVoltageToPercent(float v) {
  return constrain((maxMoistureVoltage - v) /
    (maxMoistureVoltage - minMoistureVoltage) * 100.0, 0.0, 100.0);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  pinMode(moisturePower, OUTPUT);
  digitalWrite(moisturePower, HIGH);
  delay(200);

  float v = readMoistureVoltage();
  Serial.printf("Voltage: %.3f V  |  Moisture: %.1f %%\n", v, moistureVoltageToPercent(v));

  digitalWrite(moisturePower, LOW);
  delay(5000);
}
```

You should see a reading every 5 seconds. The voltage decreases as the soil gets wetter.
If you see a constant value near 0 V or 3.3 V, check that the sensor cable is fully seated.

### 5.2 Calibrate the Moisture Sensor

The voltage-to-percentage conversion uses two reference points that vary slightly between
sensors. Run the test sketch from 5.1 while doing the measurements below.

**Dry measurement (= 0 % moisture):**
1. Hold the sensor in open air.
2. Note the voltage — this is your `MAX_MOIST_V`.

**Wet measurement (= 100 % moisture):**
1. Insert only the metal prongs into a glass of water or saturated soil.
2. Note the voltage — this is your `MIN_MOIST_V`.

Update `code/sp_modular/config.h` with your measured values:

```cpp
#define MIN_MOIST_V   0.60f  // replace with your wet voltage
#define MAX_MOIST_V   2.45f  // replace with your dry voltage
```

Then re-upload the main sketch.

### 5.3 Test the Pump

```cpp
const int PIN_PUMP = 22;  // GPIO 22 on Waveshare

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(PIN_PUMP, OUTPUT);

  Serial.println("Pump ON for 2 seconds...");
  digitalWrite(PIN_PUMP, HIGH);
  delay(2000);
  digitalWrite(PIN_PUMP, LOW);
  Serial.println("Pump OFF.");
}

void loop() {}
```

You should hear the pump run and see water flow through the tube. If nothing happens,
check that the pump connector is fully seated and that the power supply can deliver
enough current (USB power alone may be insufficient — use batteries).

### 5.4 Test the I2C Sensors (BME280 + TSL2591)

This sketch scans the I2C bus and reads both sensors. Expected I2C addresses are
`0x76` (BME280) and `0x29` (TSL2591).

```cpp
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_TSL2591.h>

const int PIN_BME_POWER = 2;
const int PIN_TSL_POWER = 3;
const int PIN_SDA       = 4;
const int PIN_SCL       = 5;

Adafruit_BME280    bme;
Adafruit_TSL2591   tsl(2591);

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(PIN_BME_POWER, OUTPUT);
  pinMode(PIN_TSL_POWER, OUTPUT);
  digitalWrite(PIN_BME_POWER, HIGH);
  digitalWrite(PIN_TSL_POWER, HIGH);
  delay(300);

  Wire.begin(PIN_SDA, PIN_SCL);

  // I2C bus scan
  Serial.println("Scanning I2C bus...");
  int found = 0;
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  Device at 0x%02X\n", addr);
      found++;
    }
  }
  if (found == 0) Serial.println("  No devices found — check wiring and power gates.");
  Serial.println();

  // BME280
  if (bme.begin(0x76, &Wire)) {
    Serial.printf("BME280  — Temp: %.1f °C  Humidity: %.1f %%  Pressure: %.0f hPa\n",
      bme.readTemperature(), bme.readHumidity(), bme.readPressure() / 100.0f);
  } else {
    Serial.println("BME280  — not found. Check connector and wire order.");
  }

  // TSL2591 (Adafruit_I2CDevice requires Wire to be re-initialised)
  Wire.begin(PIN_SDA, PIN_SCL);
  if (tsl.begin()) {
    tsl.setGain(TSL2591_GAIN_MED);
    tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);
    uint32_t lum  = tsl.getFullLuminosity();
    uint16_t ir   = lum >> 16;
    uint16_t full = lum & 0xFFFF;
    Serial.printf("TSL2591 — Lux: %.1f  IR: %d  Full: %d\n",
      tsl.calculateLux(full, ir), ir, full);
  } else {
    Serial.println("TSL2591 — not found. Check connector and wire order.");
  }
}

void loop() {}
```

If a sensor is not found but appears on the I2C scan, the most likely cause is a
wrong wire order on the connector — double-check the colour order from section 2.5/2.6.

---

## 6. Final Test and Deployment

1. With the USB cable still connected, confirm data appears on the dashboard at
   [plants.makeruniverse.de](https://plants.makeruniverse.de).
2. **Disconnect USB** and flip the power switch on.
3. The device should wake, connect to Wi-Fi, send a reading, and go back to sleep.
4. Confirm a new reading appears on the dashboard.

Once confirmed:

5. Fix the PCB mount inside the housing using the velcro strips.
6. Place the moisture sensor cable through the housing hole.
7. Screw on the lid.

> Switching the device off and on again triggers an immediate measurement — useful for
> testing without waiting an hour.

---

## What's Next?

Your device now measures soil moisture, temperature, humidity, light, and battery voltage,
and waters automatically when the soil gets too dry. All readings appear on the dashboard.

For background on how the sensors work and why certain design choices were made, see
[`background_information.md`](background_information.md).
