# Pump Add-On Build Instructions
The pump add-on lets your SmartPlants device automatically water your plant whenever the soil gets too dry. You will upgrade your base module with a pump, a MOSFET switch, and a flyback diode for protection.

## 1. Collect All Parts

**List of parts**
- Mini diaphragm pump (3–6 V, depending on your power source)
- IRLZ14 (or other logic-level N-MOSFET)
- 1× 1N4148 or 1N4001 diode (flyback diode)
- 1× 22 Ω resistor (gate protection resistor, optional but recommended)
- 1× 10k Ω resistor (gate pull-down resistor, optional but recommended)
- 1x PG7 cable gland (optional but recommended)
- 1x 3D-printed pump-housing
- 1x 2-core cable (0.5 - 1m)
- Extra jumper cables
- Tubes (for pump inlet/outlet)
- Water container

## 2. Understand the Pump Circuit

The ESP32 cannot power a pump directly. It uses a MOSFET that acts as an electronic switch.

- D7 → Gate (through 22 Ω, pulled-down to GND through a 10k Ω resistor)
- Pump + → Battery +
- Pump – → MOSFET Drain
- MOSFET Source → Ground
- Diode across pump wires (stripe to pump +)

⚠️ Important: The MOSFET must share ground with the ESP32!

## 3. Prepare the Pump

Solder a two-core cable to the pumps connectors. Insert the pump into the housing and pull the cable through the hole. Add the PG7 cable gland and screw it into the pump housing. Crimp a 2-pin dupont connector to the other end of the pump cable. 


## 4. Assemble the Pump Add-On Circuit

Follow the diagram:

**Battery Pack Version**
![image](../img/moisture+pump_circuit_bb.png)

**Li-ion/Lipo Version**
![image](../img/moisture+pump_circuit_lipo_bb.png)

Here are all the connections written out:

Pump
- Pump + → Battery +
- Pump – → MOSFET Drain

MOSFET IRLZ14 (Front view, legs down: Gate, Drain, Source = GDS)
- Gate → D7 via 22 Ω resistor, pulled-down to GND through a 10k Ω resistor 
- Drain → Pump –
- Source → GND

Diode
- Stripe → Pump +
- Other side → Pump –

## 5. Test the pump
Copy this script into your Arduino IDE and flash the ESP with it. The pump should turn on and off two times. 


```cpp
// Simple Pump Test for XIAO ESP32-C6
// Turns pump on/off two times for 1 second each

const int pumpPin = D7;  // MOSFET gate pin

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Pump Test Starting...");

  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);  // ensure pump is off

  for (int i = 1; i <= 2; i++) {
    Serial.printf("Test cycle %d: Pump ON\n", i);
    digitalWrite(pumpPin, HIGH);   // turn pump on
    delay(1000);

    Serial.printf("Test cycle %d: Pump OFF\n", i);
    digitalWrite(pumpPin, LOW);    // turn pump off
    delay(1000);
  }

  Serial.println("Pump Test Completed.");
}

void loop() {
  // nothing here
}

```

## 6. Upload final script

Upload the full code from code/moisture-pump and adjust the user settings. Under "Pump control" you can set the "moistureThreshold" and the "pumpDuration".

⚠️ Note: After pumping the water the moisture is measured again in the next wakeup cycle. This allows for the soil to soak up all the water. If the moisture increases only by a few percent, increase the pump duration.

