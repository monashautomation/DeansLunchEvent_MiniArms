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
ESP32-S3 robot arm demo system with LVGL touchscreen controllers communicating via ESP-NOW to receiver boards.

## Stack
- **MCU:** ESP32-S3 (dual-core Xtensa LX7)
- **UI Framework:** LVGL v8 with custom ESP-Panel driver
- **Communication:** ESP-NOW (peer-to-peer, no WiFi infrastructure)
- **Language:** C/C++ (Arduino framework)

## Commands
```bash
# Flash via Arduino IDE (no CLI build available)
# Select ESP32-S3 board, upload sketch from any *.ino in subdirectories
# Verify via Serial Monitor at 115200 baud

# Build C++ UI components (if using ESP-IDF toolchain)
cd final && cmake -B build && cmake --build build
```

## Structure
```
master/espnow_two_buttons/  — Two-button ESP-NOW transmitter (buttons → commands)
final/                      — Full LVGL touchscreen UI with ESP-NOW integration
receiving_test/             — ESP-NOW receiver test sketch (LED toggle)
get_mac_addr/               — MAC address discovery utility
README.md                   — Setup instructions and troubleshooting
```

## Code style
- Arduino-style C++ with function-level comments
- `#define` constants for configuration values (pins, channels, delays)
- `volatile` for ISR-shared state variables
- `esp_now_send_status_t` for callback status
- ASCII art blocks for section headers (matching existing style)
- No compliance or security requirements

## Testing
- Flash to hardware and verify via Serial Monitor
- Test button inputs and ESP-NOW message delivery
- Verify receiver LED toggle on valid commands

## Security
- No authentication on ESP-NOW messages (local demo use)
- No external network exposure

## Conventions
- Clarify ambiguities before implementing.
- Prefer the simplest solution that works.
- Touch only what the task requires.
- Match existing code style (comment blocks, defines, volatile usage).

## Agent Session Routine

### Session Start (before any task)
1. Read `.claude/kanban.md`. If OPEN or IN-PROGRESS items exist, present them:
   ```
   Open issues:
   - [ID] [SEVERITY] <description>
   Fix now / defer / dismiss?
   ```
2. Fix confirmed items highest-severity first. Append Fix record. Update status.
3. If no kanban items, check `.claude/improvements.md` — ask if user wants to action P1 items.

### During Work (continuous)
- **kanban.md:** Add OPEN item immediately when you find a bug, security gap, or quality issue. Update to IN-PROGRESS when starting a fix. Update to FIXED and append Fix record when done. Never delete rows — only change status.
- **improvements.md:** Add a row for meaningful out-of-scope improvements found during work. Remove a row only when fully implemented and verified.
- **memory/architecture.md:** Update when files, services, or data flows are added, removed, or renamed.
- **memory/decisions.md:** Append a row for every non-obvious design decision made this session.
- **AGENTS.md:** Update commands, stack, structure, or conventions when the project changes. Keep it accurate — it is read by every agent, not just Claude.

### Session End (after last task)
1. All in-progress kanban items must be FIXED or DEFERRED.
2. Decisions made this session are logged in decisions.md.
3. architecture.md reflects current state.
4. Write files silently. Do not summarise unless asked.

### Hard Rules
- Never auto-fix [RISK: HIGH] kanban items without user confirmation.
- kanban Fix records are append-only. Never edit or delete past records.
- AGENTS.md is the source of truth for cross-agent conventions.
- CLAUDE.md defers to AGENTS.md for shared context.
