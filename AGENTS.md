# VJsonl Agent Notes

This project is a Qt 6 / C++20 JSONL log viewer.

## Build And Test

- Use `./run-tests.sh` for the normal verification loop.
- The project builds with strict compiler warnings as errors.
- Keep changes compatible with C++20 and Qt 6 Widgets.
- Do not commit `build/`, `build-codex/`, `build-tests/`, `CMakeLists.txt.user`, or `*.log`.

## Editing Rules

- Prefer small, local changes that match the existing Qt Widgets style.
- Keep UI controls aligned with the table/panel they affect.
- Preserve existing config behavior through `QSettings`.
- Do not revert unrelated user changes.
- When adding tests, use Qt Test under `tests/`.

## Current Architecture

- Main UI: `src/MainWindow.*`
- Model: `src/JsonlModel.*`
- Filtering: `src/LogFilterProxyModel.*`
- Themes: `src/ThemeManager.*`
- Utility helpers: `src/Utils/`
- Unit tests: `tests/`

