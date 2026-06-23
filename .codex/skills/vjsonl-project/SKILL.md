---
name: vjsonl-project
description: Use when making code, UI, test, or build changes in the VJsonl Qt/C++20 project.
---

# Work On VJsonl

VJsonl is a Qt 6 Widgets JSONL/log viewer written in C++20.

Default workflow:

1. Inspect the relevant files before changing code.
2. Keep UI changes consistent with existing `MainWindow` layout patterns.
3. Keep helper code in `src/Utils/` using the existing per-file namespaces.
4. Add or update Qt Test coverage under `tests/` when behavior changes.
5. Run `./run-tests.sh` before finishing code changes.

Important project conventions:

- Compiler warnings are treated as errors.
- Theme persistence uses `QSettings`.
- Window/panel/table state persistence also uses `QSettings`.
- Use `ThemeManager` for theme changes.
- Use `LogFilterProxyModel` for row filtering.
- Use `JsonlModel` for model/display-role behavior.

Avoid:

- Reverting unrelated changes.
- Introducing new UI frameworks.
- Bypassing existing model/proxy responsibilities.
- Adding broad refactors while implementing a narrow feature.
