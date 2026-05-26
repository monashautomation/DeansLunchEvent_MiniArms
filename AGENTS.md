<!--
  FILE: AGENTS.md
  PURPOSE: Universal agent instructions for this project.
           Works across Claude Code, OpenAI Codex, Cursor, Windsurf,
           Gemini CLI, Aider, GitHub Copilot, Amp, and others.
           Ref: https://agents.md/
  MAINTENANCE: Update commands, structure, and conventions when the project changes.
               For nested packages, place a child AGENTS.md inside each subdirectory.
-->

# Dean's Lunch Event - Mini Arms Project

## Overview

Code for controlling 3D-printed mini robot arms via Waveshare ESP32-S3-Touch-LCD-7 screens. Multiple ESP32s communicate over ESP-NOW: each screen sends button commands to a receiving ESP32 that drives the corresponding arm.

## Stack

- **Language:** Arduino C/C++ (ESP32)
- **Framework:** LVGL v8 (UI on Waveshare ESP32-S3-Touch-LCD-7)
- **Hardware:** ESP32-S3, PCA9685 servo driver, 3D-printed mini robot arm
- **Build:** Arduino IDE (no CLI build commands)
- **Communication:** ESP-NOW (MAC-address-based)

## Structure

```
final/              → Main screen firmware with full UI (LVGL)
MASTERCONTROLLER/   → Alternative/parallel master controller build
receiving_test/     → Receiver firmware (initial test: LED toggle)
get_mac_addr/       → Utility sketch for reading ESP-NOW MAC addresses
master/             → Example button controller sketch
```

## Commands

```bash
# Flash firmware
# Open the .ino file in Arduino IDE, select board + port, click Upload
# (No command-line build commands — Arduino IDE only)
```

## Code style

- C-style comments in existing code (match the style you see)
- No enforced naming conventions
- No linting or formatting tools configured
- Keep code simple and functional

## Testing

- Flash to hardware and verify button → servo response
- Check Serial Monitor for MAC addresses and ESP-NOW messages
- No automated test suite

## Security

- No security or compliance requirements
- MAC addresses are used as device identifiers — do not hardcode production secrets
- No sensitive data in logs

## Conventions

- Clarify ambiguities before implementing
- Prefer the simplest solution that works
- Touch only what the task requires
- When editing existing code, match the existing style (C-style comments, Arduino-style)

## Agent session routine

At session start:
1. Read `.claude/kanban.md`. If OPEN items exist, present them and ask: fix / defer / dismiss?
2. Fix confirmed items highest-severity first.
3. If no kanban items, check `.claude/improvements.md` — ask if user wants to action P1 items.
