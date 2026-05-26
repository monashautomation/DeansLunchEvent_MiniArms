<!--
  FILE: CLAUDE.md
  PURPOSE: Claude Code primary memory. Loaded at every session start.
           Delegates to AGENTS.md for shared context; .claude/ for deep memory.
  Target: ≤ 80 lines.
-->

# Dean's Lunch Event - Mini Arms

> See AGENTS.md for full project context, commands, and conventions.

## Claude-specific notes

- Build: Arduino IDE only (no CLI build commands)
- Hardware: ESP32-S3 with Waveshare LCD-touch + PCA9685 servo driver
- Communication: ESP-NOW (MAC-address-based)
- Existing code uses C-style comments — match that style

## Memory index
@AGENTS.md
@.claude/memory/architecture.md
@.claude/memory/decisions.md
@.claude/rules/maintain.md
@.claude/rules/coding.md
