# PCB Designs

Two active PCB designs are maintained, one per supported controller board. Both route sensors
to the same GPIO numbers, so no firmware changes are needed between variants.

## Active designs

### Waveshare ESP32-C6 Zero — `smart_plants_breakout_ws-board_rev2/` (main build)

The primary board used in student workshops. Designed for the
[Waveshare ESP32-C6 Zero](https://www.waveshare.com/wiki/ESP32-C6-Zero).

- **Gerber files:** `sp_ws_breakout_rev2/` folder and `sp_ws_breakout_rev2.zip`
- **KiCAD project:** `smart_plants_breakout_ws-board_rev2.kicad_pro`

### SEEED Studio XIAO ESP32-C6 — `smart_plants_breakout_rev1/` (alternative build)

Alternative build based on the [XIAO ESP32-C6](https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/).

- **Gerber files:** `smart_plants_breakout_rev1_gerber/` folder and `smart_plants_breakout_rev1_gerber.zip`
- **KiCAD project:** `smart_plants_breakout_rev1.kicad_pro`
- **BOM:** `smart_plants_breakout_rev1.csv`

## Ordering PCBs

1. Download the Gerber `.zip` file for your chosen variant.
2. Upload it to a PCB fab of your choice (e.g., JLCPCB, PCBWay, Aisler).
3. Default settings (1.6 mm FR4, 2 layers, HASL finish) work fine.

## Opening in KiCAD

Open the `.kicad_pro` file in KiCAD 8 or later. External footprint libraries are in
`ext_files/` — KiCAD will prompt you to resolve them on first open; point it to that folder.

## Legacy designs

Older PCB revisions are in `legacy/`. They are archived for reference only.
