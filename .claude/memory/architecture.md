<!--
  FILE: .claude/memory/architecture.md
  PURPOSE: Structural map of the codebase. Updated on significant refactors.
-->

## Architecture

### System overview
- Multiple ESP32 boards communicate via ESP-NOW protocol
- Each Waveshare ESP32-S3-Touch-LCD-7 screen is paired with a receiving ESP32
- Receiving ESP32 drives a PCA9685 servo driver (GPIO 21 → SDA, GPIO 22 → SCL)
- Servo power shared: PCA9685 V+ and GND common with ESP32 ground

### Key modules
- **final/final.ino** — Main screen firmware with LVGL UI (full feature set)
- **receiving_test/receiving_test.ino** — Receiver firmware (LED toggle test)
- **get_mac_addr/get_mac_addr.ino** — MAC address discovery utility
- **MASTERCONTROLLER/master/master.ino** — Alternative controller build
- **master/espnow_two_buttons/espnow_two_buttons.ino** — Example button controller

### Data flow
1. User presses button on Waveshare LCD touchscreen
2. Screen firmware sends ESP-NOW message to receiver's MAC address
3. Receiver ESP32 decodes message and drives PCA9685 servo controller
4. Servo moves corresponding mini robot arm

### File organization
- `final/` — Production UI firmware (LVGL generated + custom C files for images/UI)
- `MASTERCONTROLLER/` — Alternative build with same UI components
- `receiving_test/` — Standalone receiver test
- `get_mac_addr/` — MAC address reader utility
