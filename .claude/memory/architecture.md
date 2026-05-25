<!--
  FILE: .claude/memory/architecture.md
  PURPOSE: Structural map of the codebase. Updated on significant refactors.
-->

# Architecture

## Overview
ESP32-S3 multi-device demo using ESP-NOW for zero-infrastructure communication. Each device is either a sender (touchscreen controller) or receiver (LED/arm controller).

## Communication Topology
- **Sender devices:** `master/espnow_two_buttons/` (2-button transmitter) and `final/` (full LVGL UI)
- **Receiver devices:** `receiving_test/` (LED toggle test)
- **Protocol:** ESP-NOW broadcast with hardcoded MAC addresses
- **Channel:** Fixed at channel 1 across all devices

## Module Breakdown
- `espnow_two_buttons.ino` — Button polling, ESP-NOW init, command transmission
- `final/final.ino` — LVGL UI initialization, ESP-NOW integration, broadcast
- `final/ui_*.c` — Generated LVGL screen code (generated scaffolding)
- `final/ui_comp*.c` — Custom UI component implementations
- `final/lvgl_v8_port.cpp` — ESP-Panel LVGL v8 port bindings
- `receiving_test.ino` — Simple ESP-NOW receiver, LED output
- `get_mac_addr.ino` — MAC address discovery utility

## Data Flow
1. User interacts with touchscreen/buttons on sender
2. Sender formats ESP-NOW message (command string)
3. Sender broadcasts via `esp_now_send()` with receiver MAC
4. Receiver parses command and acts (toggle LED / move arm)
