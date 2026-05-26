<!--
  FILE: .claude/rules/maintain.md
  PURPOSE: Instructs the agent to maintain all agent config files throughout
           every session. Loaded automatically from rules/ on session start.
  DO NOT DELETE — removing this breaks session continuity.
-->

# Maintenance Rules

## Session start (always, before any task)

1. Read kanban.md. If OPEN or IN-PROGRESS items exist, present them:
   ```
   Open issues:
   - [ID] [SEVERITY] <description>
   Fix now / defer / dismiss?
   ```
2. Fix confirmed items highest-severity first. Append Fix record. Update status.
3. If no kanban items, check improvements.md — ask if user wants to action P1 items.

## During work (continuous)

**kanban.md**
- Add OPEN item immediately when you find a bug, security gap, or quality issue.
- Update to IN-PROGRESS when starting a fix.
- Update to FIXED and append Fix record when done.
- Update to DEFERRED or DISMISSED on user instruction.
- Never delete rows — only change status.

**improvements.md**
- Add a row for meaningful out-of-scope improvements found during work.
- Remove a row only when fully implemented and verified.

**AGENTS.md** (if present)
- Update commands, stack, structure, or conventions when the project changes.
- Keep it accurate — it is read by every agent, not just Claude.

**memory/architecture.md**
- Update when files, services, or data flows are added, removed, or renamed.

**memory/decisions.md**
- Append a row for every non-obvious design decision made this session.

**CLAUDE.md**
- Update if project context changes. Keep ≤ 80 lines.

**settings.json**
- Add to allow list when a new project command is introduced.
- Add to deny list when a shell-injection risk is found (log to kanban first).

## Session end (always, after last task)

1. All in-progress kanban items must be FIXED or DEFERRED.
2. Decisions made this session are logged in decisions.md.
3. architecture.md reflects current state.
4. AGENTS.md reflects any changed commands or conventions.
5. Write files silently. Do not summarise unless asked.

## Hard rules

- Never auto-fix [RISK: HIGH] kanban items without user confirmation.
- kanban Fix records are append-only. Never edit or delete past records.
- audit.log is append-only. Never truncate.
- CLAUDE.md must stay ≤ 80 lines.
- AGENTS.md is the source of truth for cross-agent conventions; CLAUDE.md defers to it.
