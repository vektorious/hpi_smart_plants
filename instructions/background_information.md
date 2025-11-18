# Background Information

## Sensor Choice
### Soil Moisture Sensors
#### Overview

| Sensor Type      | Measurement Principle                    | Advantages                  | Limitations                                    |
|------------------|------------------------------------------|-----------------------------|------------------------------------------------|
| Capacitive       | Measures dielectric constant of the soil | Durable, accurate           | Sensitive to nearby objects and interference   |
| Resistive        | Measures electrical resistance           | Cheap, easy to use          | Corrodes over time (potentially toxic)         |

In addition to resistive and capacitive sensors, there are more advanced methods used in agriculture and research, such as tensiometers, Time Domain Reflectometry or Frequency Domain Reflectometry TDR/FDR sensors, and thermal or capillary-based systems. These offer higher accuracy or different measurement principles (like soil water tension) but are often more complex or expensive.

#### DIY Sensors

Tension Sensors (e.g. Tensiometer, Capillary Sensor)
- Measure how tightly water is held in the soil.
- Simulate what plants "feel" when extracting water.
- Use porous ceramic tips and water-filled tubes to show suction or pressure changes.
- analog output but requires pressure sensor to read electronically -> fun but complex to integrate.

Resistive Sensors (e.g. Nail Electrodes, Gypsum Block)
- Measure electrical resistance between two conductors.
- Moist soil conducts better, dry soil worse (but soil type/quality has an impact too!).
- Simple to build with nails, wires, or embedded in gypsum for more stability.
- analog output (voltage) which is very easy to read with microcontrollers -> simple and easy to integrate.

Tensiometers provide more biologically meaningful data, but are harder to read electronically. Resistive sensors are easier to connect and automate, making them ideal for basic watering systems.

#### Workshop Sensor Choice

For this workshop I recommend using the an off-the-shelf capacitive sensor. They are are more stable and durable than resistive ones. They don’t corrode as quickly, provide smoother readings, and are widely available as well as affordable. This makes them ideal for beginner projects and long-term use in automated watering systems. There are standard sensors that are sold by various shops (see image below). However, they are often faulty. Check this video before you buy some: [Capacitive Soil Moisture Sensors don't work correctly + Fix for v2.0 v1.2 Arduino ESP32 Raspberry Pi](https://www.youtube.com/watch?app=desktop&v=IGP38bz-K48)

![image](img/moisture_sensor.png)

### Pump Systems

| Pump Type         | Working Principle                             | Advantages                        | Suitable For                     |
|-------------------|-----------------------------------------------|-----------------------------------|----------------------------------|
| Submersible Pump  | Impeller submerged in water reservoir         | High flow rate, robust            | Raised beds, greenhouses         |
| Diaphragm Pump    | Oscillating membrane creates pressure/vacuum  | Stable pressure, self-priming     | Distributed watering systems     |
| Peristaltic Pump  | Fluid pushed through flexible tube by rollers | Precise dosage, no backflow       | Potted plants, indoor setups     |

For this workshop, we will use a diaphragm pump. They are reliable, self-priming, and can handle small amounts of water with consistent pressure. They work well in compact systems and are easy to control with a relay or MOSFET. Their sealed design also makes them less prone to leaking or clogging — perfect for small-scale automated irrigation projects.

### Controlling the Pump

Microcontrollers like ESP32 or Arduinos cannot power pumps directly. You need an electronic switch that can handle higher current. There are two common ways to do this: Using a relay or a MOSFET.

| Feature                  | Relay Module                            | MOSFET Module                               |
|--------------------------|-----------------------------------------|---------------------------------------------|
| Switching type           | Mechanical (electromagnetic switch)     | Electronic (transistor-based)               |
| Sound                    | Audible “click” when switching          | Silent                                      |
| Speed                    | Slow switching (ms range)               | Fast switching (μs range), suitable for PWM |
| Load types               | Good for AC and DC                      | Typically used for DC only                  |
| Efficiency               | Some power loss (mechanical contact)    | High efficiency, low heat generation        |
| Use case example         | Simple on/off pump control              | Precise or PWM pump control (e.g. speed)    |
| Complexity               | Easy to wire and understand             | Requires correct polarity and pin setup     |


For this workshop, we will use a MOSFET (IRLZ14). This logic-level MOSFET can be triggered directly from a microcontroller (e.g. Arduino or micro:bit) and is capable of switching up to 60 V DC and 10 A — more than enough for most small pumps or valves. It's silent, efficient, and ideal for low-voltage DC systems.
While the pumps used in this setup typically draw far less current (around 500 mA), using a higher-rated MOSFET gives us more flexibility. It ensures that the same circuit can also be used safely with larger pumps or other components in future projects.

(Wiring example check [here](https://learn.adafruit.com/rgb-led-strips/usage)) 

NOTE: Microcontroller AND MOSFET/Pump Power Source MUST HAVE THE SAME GROUND!!



## Similar Projects

### Self-Watering Flower Pot "Flaura"
This 3D-printable self-watering flower pot project integrates a soil moisture sensor, a microcontroller (ESP8266), and a small water pump to automate plant care. It also includes a mobile app interface using Blynk, allowing users to monitor soil moisture and trigger manual watering remotely.

https://www.thingiverse.com/thing:4921885


### Watering System without microcontroller
This project builds a simple automatic plant watering system without using a microcontroller. It uses soil probes to detect moisture, a BC547 transistor to trigger a relay, and a small water pump. When the soil is dry, the pump automatically turns on; when the soil is wet, it turns off. It's a low-cost solution with no programming required.

https://circuitdigest.com/electronic-circuits/automatic-plant-watering-system-without-arduino

### Elecrow Smart Plant

A compact IoT plant monitor built around an ESP32‑S2 microcontroller, featuring a built‑in capacitive soil moisture probe, ambient light and air temperature/humidity sensors, and an optional 2.9″ e‑paper display for live data. It streams data over Wi‑Fi (compatible with Home Assistant via ESPHome) and is ideal for visualizing plant health, though it does not include a pump or watering system

https://www.elecrow.com/smart-plant.html
