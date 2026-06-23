# VJsonl Agent Notes

This project is a Qt 6 / C++20 JSONL log viewer.

## Build And Test

- Use `./run-tests.sh` for the normal verification loop.
- The project builds with strict compiler warnings as errors.
- Keep changes compatible with C++20 and Qt 6 Widgets.
- Do not commit `build/`, `build-codex/`, `build-tests/`, `CMakeLists.txt.user`, or `*.log`.

## Editing Rules

- Inspect the relevant files before changing code.
- Prefer small, local changes that match the existing Qt Widgets style.
- Keep UI changes consistent with existing `MainWindow` layout patterns.
- Keep UI controls aligned with the table/panel they affect.
- Preserve existing config behavior through `QSettings`.
- Store media assets such as icons and images in `resources`, not as drawing code.
- Do not revert unrelated user changes.
- When adding tests, use Qt Test under `tests/`.

## Current Architecture

- Main UI: `src/MainWindow.*`
- Model: `src/JsonlModel.*`
- Filtering: `src/LogFilterProxyModel.*`
- Themes: `src/ThemeManager.*`
- Utility helpers: `src/Utils/`, using the existing per-file namespaces.
- Unit tests: `tests/`

## Architecture Rules

- Use `ThemeManager` for theme changes.
- Use `LogFilterProxyModel` for row filtering.
- Use `JsonlModel` for model/display-role behavior.
- Use `QSettings` for theme, window, panel, table, and other persisted UI state.
