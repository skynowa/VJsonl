# TODO

## High Priority

- [-] Add `Go to line...`.
- [-] Add `Next Error` / `Previous Error`.
- [-] Add `Next Fatal` / `Previous Fatal`.
- [?] Add context menu for table cells:
  - Copy cell value.
  - Copy row as JSON/raw.
  - Copy selected rows.
- [-] Add export for filtered rows:
  - `.jsonl`
  - `.txt`
- [+] Add column visibility menu from the table header.
- [-] Add drag and drop file opening.
- [?] Add auto-open last file on startup.

## Filtering

- [-] Add filter mode: contains / regex / exact.
- [?] Add negative filter mode.
- Add column-specific filter:
  - [+] all columns
  - [+] `msg`
  - [+] `query`
  - `request_id`
  - `raw`
- Add quick filter for error rows only.
- Add quick filter chips:
  - [+] `log_name`
  - `service`
  - [+] `proc_name`

## Navigation

- Add bookmarks for important rows.
- Add previous/next bookmark navigation.
- Add selection history back/forward.
- [?] Add keyboard shortcuts for common navigation actions.

## Value Panel

- [+] Add `Copy value` button.
- [+] Add `Copy formatted value` button.
- [?] Add `Save value to file`.
- [?] Add `Raw / Formatted / Preview` mode selector.
- [+] Highlight all search matches, not only the first match.
- [-] Add next/previous match navigation in the value panel.
- [-] Add JSON tree view.
- [+] Add SQL syntax highlighting.
- [+] Add XML syntax highlighting.
- [+] Add HTML syntax highlighting.

## Table

- [?] Auto-detect JSON keys and add them as columns.
- [+] Allow custom column order.
- [+] Save column widths.
- [+] Save visible/hidden column state.
- Add pinned/frozen columns for `line`, `level`, and `msg`.
- [+] Add tooltip with full cell value for truncated cells.
- [+] Add more color styles:
  - [+] `Info`
  - [+] `Warn`
  - [+] `Trace`

## File Handling

- Add reload current file.
- Add file changed notification.
- Add watch mode for changed files.
- Add tail mode for append-only logs.
- Add cancel button for large file loading.
- Move large file loading to a background worker thread.
- [-] Add loading statistics:
  - bytes read
  - lines read
  - read speed

## Statistics

- [+] Add level counters:
  - [+] `Fatal`
  - [+] `Error`
  - [+] `Warn`
  - [+] `Info`
  - [+] `Debug`
  - [+] `Trace`
- Add top values panel:
  - top `log_name`
  - top `service`
  - top `proc_name`
- [+] Add memory statistics for `mem_usage_kb`:
  - [+] min
  - [+] max
  - [+] average
- [?] Add simple timeline by `ts`.
- [?] Add error timeline by `ts`.

## Performance

- Add lazy loading for very large files.
- Add virtualized/chunked model storage.
- Avoid storing parsed JSON documents for plain-text log lines.
- Add optional parsing only for visible rows.
- Add memory usage indicator for the viewer itself.

## Documentation

- Keep `Features.md` updated after each feature.
- Keep `ChangeLog.md` updated after each user-visible change.
- Add screenshots for main workflows.
- Add test data descriptions for files in `sample/`.
