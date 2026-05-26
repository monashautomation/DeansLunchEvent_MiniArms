<!--
  FILE: .claude/rules/coding.md
  PURPOSE: Behavioural guardrails for every coding task.
           Biases toward caution over speed — use judgment on trivial tasks.
           Loaded automatically from rules/ each session.
  DO NOT DELETE — removing this degrades output quality and traceability.
-->

# Coding Behaviour

## 1. Think Before Coding
- State assumptions explicitly. If uncertain, ask.
- If multiple interpretations exist, present them — don't pick silently.
- If a simpler approach exists, say so. Push back when warranted.
- If something is unclear, stop. Name what's confusing. Ask.

## 2. Simplicity First
Minimum code that solves the problem. Nothing speculative.
- No features beyond what was asked.
- No abstractions for single-use code.
- No "flexibility" or "configurability" that wasn't requested.
- No error handling for impossible scenarios.
- If you write 200 lines and it could be 50, rewrite it.

Heuristic: "Would a senior engineer say this is overcomplicated?" If yes, simplify.

## 3. Surgical Changes
Touch only what you must. Clean up only your own mess.

When editing existing code:
- Don't improve adjacent code, comments, or formatting.
- Don't refactor things that aren't broken.
- Match existing style, even if you'd do it differently.
- If you notice unrelated dead code, mention it — don't delete it.

When your changes create orphans:
- Remove imports/variables/functions that YOUR changes made unused.
- Don't remove pre-existing dead code unless asked.

Test: every changed line must trace directly to the user's request.

## 4. Goal-Driven Execution
Transform tasks into verifiable goals before starting:
- "Add validation" → "Write tests for invalid inputs, then make them pass."
- "Fix the bug" → "Write a test that reproduces it, then make it pass."
- "Refactor X" → "Ensure tests pass before and after."

For multi-step tasks, state a plan first:
```
1. [Step] → verify: [check]
2. [Step] → verify: [check]
```

Weak criteria ("make it work") require constant clarification.
Strong criteria let you loop independently.

## Health check
These rules are working if:
- Diffs contain fewer unnecessary changes.
- Fewer rewrites from overcomplication.
- Clarifying questions come before implementation, not after mistakes.
