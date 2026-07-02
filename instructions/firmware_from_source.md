# Firmware from Source (Arduino IDE)

> **Most people should use the web flasher instead** — see
> [`build_instructions.md` → Install the Firmware](build_instructions.md#4-install--configure-the-firmware).
> It flashes the exact same firmware from the browser with no toolchain to install.

Use this guide only if you want to **build and upload the firmware yourself** — for example to
change the code, if your browser doesn't support the web flasher (only Chrome/Edge/Opera do), or
for advanced component testing with the sketches at the end.

All per-device settings (WiFi, device name, calibration, which sensors are enabled) are configured
at runtime in the setup portal after flashing — you do **not** need to edit the code for a normal
build. See [Configure via the setup portal](build_instructions.md#42-configure-via-the-setup-portal).

---

## 1. Install Arduino IDE and the ESP32 board package

1. Download and install [Arduino IDE](https://www.arduino.cc/en/software).
2. Open **File > Preferences** and add this URL to *Additional Boards Manager URLs*:
   `https://dl.espressif.com/dl/package_esp32_index.json`
3. Go to **Tools > Board > Boards Manager**, search for `esp32`, and install the
   **ESP32 by Espressif Systems** package.
4. Select board: **Tools > Board > ESP32 Arduino > Waveshare ESP32-C6-Zero**
   (or search for "ESP32-C6-Zero" in the board list).

## 2. Install the required libraries

In **Tools > Manage Libraries**, search for and install:

| Library | Author |
|---------|--------|
| WiFiManager | tzapu |
| Adafruit BME280 Library | Adafruit |
| Adafruit TSL2591 Library | Adafruit |
| Adafruit Unified Sensor | Adafruit |

> All sensor libraries are required even if you don't use every sensor — the firmware compiles
> all sensor code in and enables/disables each one at runtime. When installing the Adafruit BME280
> or TSL2591 libraries, click **Install All** if asked about dependencies.

<img src="../img/install_wm.png" height="400">

## 3. Set the partition scheme

**Tools > Partition Scheme > "Huge APP (3MB No OTA/1MB SPIFFS)".**

This is required — the firmware does not fit the default partition. If you leave it on the
default you'll get a "text section exceeds available space" / "sketch too big" error.

## 4. Upload

1. Open `code/sp_modular/sp_modular.ino` in Arduino IDE.
2. (Optional) Open `code/sp_modular/config.h` if you want to change the **factory defaults** —
   e.g. the device-name prefix or default calibration. These only apply on first boot or after a
   factory reset; a configured device keeps whatever you set in the portal. You do **not** need to
   touch this file for a normal build.
3. Connect the Waveshare board via USB-C, select **Tools > Port**, and click **Upload**.

> **Upload fails?** The board may be in deep sleep from the running firmware, which disconnects
> the USB port. Put it into flashing mode:
> 1. Unplug the board.
> 2. Hold the **BOOT** button.
> 3. Plug back in while holding BOOT.
> 4. Release after ~2 seconds, then retry Upload.

After uploading, continue with
[Configure via the setup portal](build_instructions.md#42-configure-via-the-setup-portal).

### Building a web-flasher image (optional)

The ESP32 core also emits a single merged image at `sp_modular.ino.merged.bin` (in the
Export-Compiled-Binary output). That's what the web flasher serves — see
[`../web-flasher/README.md`](../web-flasher/README.md) for details.

---

## 5. Component test sketches (advanced troubleshooting)

For a quick "is this sensor alive?" check you normally don't need these — use the **Live Sensor
Readings** page in the portal instead. These standalone sketches are for deeper debugging: upload
each separately and open the **Serial Monitor** at **115200 baud**.

### 5.1 Test the moisture sensor

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

### 5.2 Calibrate the moisture sensor

You can calibrate straight from the portal's **Live Sensor Readings** page — read the moisture
voltage in the two states below and enter them into the portal's *Moisture wet/dry voltage*
fields. To do it from this sketch instead:

**Dry (= 0 % moisture):** hold the sensor in open air — this voltage is `MAX_MOIST_V` (dry).
**Wet (= 100 % moisture):** put only the metal prongs into water or saturated soil — this
voltage is `MIN_MOIST_V` (wet).

Enter both values in the setup portal (they're saved per device), or, if building from source,
update the `DEFAULT_MIN_MOIST_V` / `DEFAULT_MAX_MOIST_V` defaults in `code/sp_modular/config.h`.

### 5.3 Test the pump

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

### 5.4 Test the I2C sensors (BME280 + TSL2591)

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
wrong wire order on the connector — double-check the colour order from section 2.5/2.6 of the
[build instructions](build_instructions.md).
