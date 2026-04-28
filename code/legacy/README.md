# Legacy Sketches

These are the original monolithic Arduino sketches from the breadboard era of this project.

They are **no longer maintained or supported**. The current firmware is `code/sp_modular/`.

## Why they are kept

Preserved here for reference in case you want to understand the project's history or run a
stripped-down build without the full modular setup. The full breadboard-era repository state
is also available at git tag `v0-breadboard`.

## What's here

| Folder | What it does |
|--------|-------------|
| `moisture/` | Moisture sensor only — no pump, no I2C sensors |
| `moisture-pump/` | Moisture sensor + pump control |
| `moisture-pump-BME280/` | + BME280 (temperature, humidity, pressure) |
| `moisture-pump-BME280-TSL2591/` | + TSL2591 light sensor (full feature set) |

## Important differences from sp_modular

- These sketches use **different GPIO pin assignments** than the current PCB design.
- They are **monolithic** — all code is in one `.ino` file per sketch.
- They target **breadboard wiring**, not the PCB.
- No `SensorPacket` struct — sensor values are passed as individual function parameters.
