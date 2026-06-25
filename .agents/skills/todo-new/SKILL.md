---
name: todo-new
description: Add new useful feature ideas to this project's TODO.md. Use when the user asks to propose, add, expand, or record new TODO items/features for VJsonl, including requests such as "add useful features to TODO.md", "добавь новые идеи в TODO.md", or "suggest TODOs".
---

# Add New TODO Features

Follow this workflow:

1. Inspect `TODO.md` before editing to understand existing sections, wording, status markers, and duplicate ideas.
2. Inspect relevant project files when needed to keep proposed features grounded in the current Qt/C++20 JSONL viewer architecture.
3. Add only useful, actionable feature ideas that fit VJsonl's scope as a desktop JSONL/log viewer.
4. Place each new item in the most specific existing section. Create a new section only when no existing section fits.
5. Use the existing TODO style:
    - Plain new ideas: `- Add ...`
    - Uncertain ideas: `- [?] Add ...`
    - Not applicable ideas: do not add them.
    - Completed ideas: do not add them.
6. Prefer small, implementable items over vague epics. Split larger ideas into concise child bullets when that makes implementation clearer.
7. Preserve existing TODO wording, indentation, ordering, and status markers except for the new items being added.
8. Report the added items and sections changed. Do not implement features or create a git commit unless the user asks separately.

Rules:

- Do not duplicate existing TODO items, including close variants already covered by parent or child tasks.
- Do not add generic maintenance chores unless they directly improve user-visible behavior, testability, or reliability.
- Keep items phrased as features or outcomes, not implementation prescriptions, unless the implementation detail is the point of the task.
- Keep changes scoped to `TODO.md` unless the user explicitly asks to update other documentation.
- If the user provides a feature idea, preserve its intent while matching the existing TODO wording style.
