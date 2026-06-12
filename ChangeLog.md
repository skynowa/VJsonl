# ChangeLog

## 2026-06-13

- Added row highlighting by log level:
  - `Fatal`: bright purple background with white text.
  - `Error`: red background with white text.
  - `Debug`: light green background.
- Added log level icons in the `level` table column.
- Added the same log level icons to the level filter combobox.
- Added a dedicated `error` column.
- Added `mem_usage_kb` column with human-readable display.
- Added log level filter combobox with `All levels`, `Fatal`, `Error`, `Warn`, `Warning`, `Debug`, `Info`, and `Trace`.
- Fixed `line` sorting so it sorts numerically.
- Added current row tracking in the status bar.
- Simplified status bar to show current line, invalid count, and file size.
- Changed file size formatting to decimal units so it matches the OS file dialog.
- Added large-file loading progress bar in the main window status bar for files larger than 50 MB.
- Changed loading progress to use real bytes read from `QFile::readLine()`.
- Improved non-JSON log handling:
  - Plain text log lines no longer become JSON parse errors.
  - Only lines starting with `{` or `[` are parsed as JSON.
- Added lower value panel for the current cell.
- Removed the old lower raw/pretty JSON panel.
- Added `Format` checkbox for value formatting.
- Added SQL formatting.
- Added JSON formatting.
- Added XML formatting.
- Added formatting for SQL/JSON/XML fragments embedded inside text.
- Added `Wrap line` checkbox for the value panel.
- Added search field for the value panel.
- Added HTML preview support in the lower panel.
- Added `HTML preview` checkbox, enabled when the selected value looks like HTML.
- Added `File` menu.
- Added `File -> Open`.
- Added `File -> Recent files...`.
- Added persistent recent files list via `QSettings`.
- Added `Clear recent files`.
- Added `File -> Open original file location`.
- Added `File -> Exit`.
- Added `Help -> About`.
- Refactored code into replaceable modules:
  - `LogFilterProxyModel`: filtering and sorting.
  - `CodeFormatter`: SQL/JSON/XML formatting.
  - `FileUtils`: human-readable file sizes.
  - `HtmlUtils`: HTML detection.
  - `LogLevelStyle`: log level icons.
- Updated `CMakeLists.txt` for the new modules.

