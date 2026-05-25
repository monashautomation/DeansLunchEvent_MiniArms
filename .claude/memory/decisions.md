<!--
  FILE: .claude/memory/decisions.md
  PURPOSE: Log of non-obvious design decisions. Prevents re-litigating choices.
-->

# Decision log

| Decision | Reason | Date |
|---|---|---|
| Host-compiled Unity tests in `tests/` with stub headers for ESP-NOW/WiFi/Arduino | Arduino code can't unit-test on hardware automatically; stubs let logic run on Mac with `make test` | 2026-05-25 |
| Gripper button cooldown reduced from 3000ms → 500ms | 3s was too long between open/close — user UX feedback | 2026-05-25 |
| Monash Automation palette applied to LVGL UI (`#020817` bg, `#016ccc` blue, `#59e9cb` teal) | Team branding requested; monotone dark theme suits handheld demo context | 2026-05-25 |
| `ui_GripperStateLabel` global added to `ui_Screen1.c` to update CLOSED/OPEN text on toggle | State text must update from existing `ui_event_Button1` callback | 2026-05-25 |
