<!--
  FILE: .claude/kanban.md
  PURPOSE: Session-persistent issue tracker. At session start, agent reads
           this file, surfaces open items, and asks: fix / defer / dismiss?
           Agent updates status after each action.
  Quality bar: DO-178B-aligned — every fix traceable to symptom, root cause,
               and verification step.
  DO NOT auto-fix [RISK: HIGH] items without explicit user confirmation.
-->

# Backlog

| ID | Category | Severity | Item | Status |
|---|---|---|---|---|
| <!-- Agent populates from scan findings --> | | | | |

# Fix record

<!--
  Append for every resolved item — never edit or delete past records.

  ### FIX-<ID>
  **File:** <path>
  **Symptom:** <what was wrong>
  **Root cause:** <why it existed>
  **Change:** <what was done>
  **Verification:** <test command or manual step>
  **Date:** <ISO 8601>
-->
