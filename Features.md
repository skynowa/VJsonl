# Features

## File Handling

- Open `.jsonl`, `.log`, `.txt`, or any file type.
- Recent files menu with persistent history.
- Open the current file location in the OS file manager.
- Large-file loading progress bar for files over 50 MB.
- Human-readable file size in the status bar.

## Table View

- Displays log data in a sortable table.
- Numeric sorting for the `line` column.
- Text filter across all columns.
- Log level filter for `Fatal`, `Error`, `Warn`, `Warning`, `Debug`, `Info`, and `Trace`.
- Current line tracking in the status bar.
- Plain text log lines are supported alongside JSONL.

## Columns

- Built-in columns include:
  - `line`
  - `valid`
  - `ts`
  - `level`
  - `error`
  - `log_name`
  - `mem_usage_kb`
  - `msg`
  - `query`
  - `proc_name`
  - `service`
  - `request_id`
  - `backtrace`
  - `raw`
- `mem_usage_kb` is displayed in human-readable format.

## Visual Highlighting

- `Fatal` rows use a bright purple background with white text.
- `Error` rows use a red background with white text.
- `Debug` rows use a light green background.
- Log level icons are shown in the `level` column.
- Log level icons are also shown in the level filter dropdown.

## Value Panel

- Lower panel shows the current selected cell value.
- Optional line wrapping.
- Search inside the current value.
- Formatting toggle for supported content.

## Formatting

- SQL formatting.
- JSON formatting.
- XML formatting.
- Formatting also works when SQL, JSON, or XML is embedded inside larger text.

## HTML Preview

- Detects HTML-like values.
- Optional rendered HTML preview in the lower panel.
- Search works in the active lower-panel mode.

## Menus

- `File -> Open`
- `File -> Recent files...`
- `File -> Open original file location`
- `File -> Exit`
- `Help -> About`

## Architecture

- Modular implementation for easier replacement of features:
  - `LogFilterProxyModel` handles filtering and sorting.
  - `CodeFormatter` handles SQL/JSON/XML formatting.
  - `FileUtils` handles human-readable file sizes.
  - `HtmlUtils` handles HTML detection.
  - `LogLevelStyle` handles log level icons.

