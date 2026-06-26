# VJson

Minimal Qt 6 Widgets JSONL viewer.

## Features

- Open `.jsonl`, `.log`, `.txt`
- Parse JSON object per line
- Show table columns:
  - line
  - valid
  - timestamp
  - level
  - message
  - service
  - request_id
  - raw
- Text filter across all columns
- Pretty JSON preview for selected row
- Invalid lines are highlighted

## Build in Qt Creator

1. Open `CMakeLists.txt`
2. Select Qt 6 kit
3. Configure
4. Build / Run

Tested target: Qt 6.x Widgets, C++20.

## Build from terminal

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/VJson
```

## Notes

This MVP loads the full file into memory. It is OK for normal log files and hundreds of thousands of lines. For multi-GB logs, use chunked/lazy loading.

## Similar apps

- [json-log-viewer](https://github.com/gistia/json-log-viewer)
- [jq](https://jqlang.org)
