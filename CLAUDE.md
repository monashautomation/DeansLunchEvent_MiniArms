<!--
  FILE: CLAUDE.md
  PURPOSE: Claude Code primary memory. Loaded at every session start.
           Delegates to AGENTS.md for shared context; .claude/ for deep memory.
  Target: ≤ 80 lines.
-->

# Dean's Lunch Event - Mini Arms Project

> See AGENTS.md for full project context, commands, and conventions.

## Claude-specific notes
- Arduino sketches use `setup()`/`loop()` pattern — don't refactor to modern C++.
- LVGL v8 UI code is generated; treat `ui_*.c` files as generated scaffolding.
- ESP-NOW broadcast addresses are hardcoded MACs — document changes to them.

## Memory index
@AGENTS.md
@.claude/memory/architecture.md
@.claude/memory/decisions.md
@.claude/rules/maintain.md
@.claude/rules/coding.md
