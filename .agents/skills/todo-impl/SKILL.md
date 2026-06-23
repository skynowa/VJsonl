---
name: todo-impl
description: Implement a user-specified task from this project's TODO.md and update its completion status. Use when the user names or quotes a TODO item and asks to do, implement, complete, or mark it, including requests such as "TODO.md - Add reload current file" or "сделай из TODO.md ...".
---

# Implement TODO Task

Follow this workflow:

1. Find the exact requested item in `TODO.md`. Use the user's wording and nearby section or child items to resolve its scope.
2. Inspect the relevant implementation, tests, project instructions, and current `git status` before editing.
3. Mark the requested item as active with `[*]`, preserving its wording and indentation. Keep an existing `[*]` marker unchanged.
4. If the feature already exists, verify its behavior instead of reimplementing it.
5. Implement the complete requested behavior using existing Qt/C++20 architecture and local conventions.
6. Add or update focused Qt Test coverage where the behavior can be tested meaningfully.
7. Run `./run-tests.sh` and `git diff --check` after code changes.
8. Replace `[*]` with `[+]` only after the implementation is complete and verification succeeds.
9. Report the behavior added, TODO status, and verification result. Do not create a git commit unless the user asks separately.

Rules:

- Keep unrelated TODO items and user changes untouched.
- Treat `[*]` as active/in progress and `[+]` as complete.
- Do not mark a task complete when implementation is partial, tests fail, or a blocker remains; leave it as `[*]` and report why.
- For a parent item with child tasks, mark only the items actually completed. Mark the parent only when its full stated scope is complete.
- Preserve the existing TODO wording and indentation; add only the `[+]` status marker.
- If no exact TODO item exists, report that clearly before changing `TODO.md`; still implement only when the user's requested behavior is unambiguous.
- If the request is ambiguous between multiple TODO items, inspect context first and ask one concise question only when the ambiguity cannot be resolved from the repository.
