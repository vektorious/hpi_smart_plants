# Smart Plants — Web Flasher

A browser-based installer for the Smart Plants firmware, built on
[ESP Web Tools](https://esphome.github.io/esp-web-tools/). Students open a web page,
click **Connect**, and flash the board — no Arduino IDE or drivers needed.

## Files

| File | Purpose |
|------|---------|
| `index.html` | The flasher page with the install button |
| `manifest.json` | Tells ESP Web Tools which chip + binary to flash |
| `firmware/smart_plants.bin` | The merged firmware image (generated — see below) |

## Requirements (for the student flashing)

- **Chrome, Edge, or Opera** on a desktop (Web Serial is not in Firefox/Safari or on iOS).
- The page must be served over **HTTPS** (or `http://localhost` for testing). Web Serial
  refuses to run on plain `http://`.
- A **USB-C data cable** (not charge-only).

## Building / updating the firmware image

The firmware is a single merged image flashed at offset `0x0`. The ESP32 Arduino core
produces this automatically — no manual `esptool merge_bin` step is needed.

### Option A — Arduino IDE

1. Open `code/sp_modular/` in Arduino IDE.
2. Select board **Waveshare ESP32-C6-Zero**, and under
   **Tools → Partition Scheme** choose **Huge APP (3MB No OTA/1MB SPIFFS)**.
   (The default partition is too small — the build won't fit.)
3. **Sketch → Export Compiled Binary.**
4. In the sketch's `build/…/` output folder, find `sp_modular.ino.merged.bin`.
5. Copy it to `web-flasher/firmware/smart_plants.bin`.

### Option B — arduino-cli

```bash
cd code
arduino-cli compile \
  --fqbn esp32:esp32:waveshare_esp32_c6_zero:PartitionScheme=huge_app \
  --output-dir /tmp/sp-build \
  sp_modular

cp /tmp/sp-build/sp_modular.ino.merged.bin \
   ../web-flasher/firmware/smart_plants.bin
```

After copying, bump `"version"` in `manifest.json` so returning users get the update.

> The merged image is a full 4 MB flash image starting at `0x0` (bootloader + partition
> table + app), which is exactly what the manifest expects: a single part at offset 0.

## Hosting

**Live at:** <https://vektorious.github.io/hpi_smart_plants/>
(served over HTTPS, which Web Serial requires).

GitHub Pages can only publish from a branch root or `/docs`, not an arbitrary subfolder, so the
site is published from a dedicated **`gh-pages`** branch that mirrors this folder at its root:

```
gh-pages/
├── index.html          # copy of web-flasher/index.html
├── manifest.json       # copy of web-flasher/manifest.json
├── .nojekyll
└── firmware/
    └── smart_plants.bin   # the merged image (tracked here, not on main)
```

The 4 MB binary lives only on `gh-pages` to keep `main` history clean.

**To publish a new firmware build:**

```bash
git worktree add /tmp/ghpages gh-pages           # check out the branch
cp web-flasher/index.html web-flasher/manifest.json /tmp/ghpages/
cp <build>/sp_modular.ino.merged.bin /tmp/ghpages/firmware/smart_plants.bin
# bump "version" in /tmp/ghpages/manifest.json
git -C /tmp/ghpages commit -am "Update flasher firmware" && git -C /tmp/ghpages push
git worktree remove /tmp/ghpages
```

## Local testing

```bash
cd web-flasher
python3 -m http.server 8000
# open http://localhost:8000  (localhost is treated as secure by Web Serial)
```
