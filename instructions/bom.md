# Bill of Materials — Smart Plants Starter Kit (ws-board rev2)

Complete parts list for one device. PCB components are the discrete parts soldered onto the
Smart Plants Breakout PCB; the Electronics, Mechanical, and Tools sections cover everything
else needed for the full build.

## PCB Components

| Reference | Component | Qty | Value | Notes |
|-----------|-----------|-----|-------|-------|
| — | Smart Plants Breakout PCB (bare board) | 1 | ws-board rev2 | Populated with the components below |
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

## Electronics

| Item | Qty | Spec | Notes |
|------|-----|------|-------|
| Microcontroller | 1 | Waveshare ESP32-C6 Zero | |
| Smart Plants Breakout PCB | 1 | ws-board rev2 | Populated with the PCB components above |
| Capacitive soil moisture sensor | 1 | | |
| 3-core cable | 1 | | Self-crimped for the moisture sensor |
| 3-pin female JST XH connector | 2 | 3-pin JST XH | For the moisture sensor cable (both ends) |
| BME280 sensor module | 1 | Temperature/humidity/pressure | |
| TSL2591 sensor module | 1 | Light | |
| Pre-crimped 4-core cable | 2 | 4-pin JST XH | For BME280 and TSL2591 sensors |
| Mini diaphragm pump | 1 | | |
| 2-core cable | 1 | | Self-crimped for the pump |
| 2-pin female JST XH connector | 1 | 2-pin JST XH | For the pump cable |
| Battery pack | 1 | 3× AA | |
| USB-C connector | 1 | | With cable |
| On/Off switch | 1 | | |
| Pre-crimped 2-core cable | 1 | 2-pin JST XH | For the on/off switch |

## Mechanical

| Item | Qty | Notes |
|------|-----|-------|
| 3D-printed battery housing and PCB mount | 1 | |
| 3D-printed moisture sensor housing + lid | 1 | |
| 3D-printed pump housing | 1 | |
| 3D-printed BME280 housing | 1 | Stevenson Screen |
| 3D-printed TSL2591 housing | 1 | |
| Ping pong ball half | 1 | Diffusor for the TSL2591 light sensor |
| 3D-printed cable plugs | 2 | |
| Laser-cut enclosure | 1 | |
| Wood screws | 12 | |
| Velcro tape | 1 | |
| PG7 cable gland | 1 | For pump cable |
| Tubes | 2 | Pump inlet/outlet |

## Tools

| Item | Qty | Notes |
|------|-----|-------|
| Soldering iron + solder | 1 | |
| Crimping tool | 1 | For JST connectors |
| Wire cutters / flush cutters | 1 | |
| Wire strippers | 1 | |
| Screwdriver | 1 | |
| Hot-glue gun | 1 | Or other glue |
| Drill with 8 mm bit | 1 | Only needed if the housing cable hole is not pre-cut |
