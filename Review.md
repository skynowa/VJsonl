# Qt Code Review Report

**Scope**: files: `/home/skynowa/Projects/VJsonl/src`, `/home/skynowa/Projects/VJsonl/tests`
**Files reviewed**: 49
**Original issues found**: 41 (27 from lint, 14 from deep analysis)
**Resolution status**: Fixed in this working tree.

## Resolution Summary

- Deterministic Qt review linter findings are fixed; rerunning `qt_review_lint.py` over the whole project now exits cleanly.
- Model loading now avoids progress callbacks inside a model reset, validates model indexes, treats non-object JSON rows as invalid, records typed timestamps once, and checks file-device read errors where supported.
- CSV loading now checks read errors and enforces file, row, and field-size limits before full materialization.
- Proxy sorting is case-insensitive for text fallback sorting and timestamp filtering uses the typed timestamp role instead of reparsing display strings.
- Session menu rebuilds are deferred out of action-trigger signal delivery.
- QObject-derived classes explicitly disable copy/move.
- Formatting work is no longer performed unconditionally on every selection change; formatted copies compute lazily when needed.
- Level icon HTML and SQL formatter regexes are cached.
- Table-session settings now include schema validation, and settings writes are synced and checked through `syncSettings()`.
- Focused tests were added or tightened for non-object JSON rows, case-insensitive sorting, table-session schema validation, JSON parse validation in CSV tests, and the release-safe proxy test helper.

**Verification**:

- `./run-tests.sh` passed: 5/5 tests.
- `python3 /home/skynowa/Projects/Github/AISkills/agent-skills/skills/qt-cpp-review/references/lint-scripts/qt_review_lint.py <all source/test C++ files>` passed with no output.

---

## Lint Findings

### [L-001] `QChar` object type

- **File**: `src/CodeFormatter.cpp:190`, `:191`, `:197`, `:220`, `:244`; `src/CsvReader.cpp:92`
- **Rule**: DEP-13
- **Finding**: `QChar` used as an object type.
- **Mitigation**: Prefer `char16_t` where a character object is needed; `QChar::` namespace usage is fine.

### [L-002] Deprecated `qMin`/`qMax`

- **File**: `src/JsonlModel.cpp:303`, `:346`, `:347`; `src/MainWindow.cpp:328`, `:1162`, `:1163`
- **Rule**: DEP-7
- **Finding**: `qMin`/`qMax` used.
- **Mitigation**: Use `(std::min)()`, `(std::max)()`, or `std::clamp()`.

### [L-003] `QMap` usage flagged for review

- **File**: `src/JsonlModel.cpp:397`; `src/JsonlModel.h:59`, `:101`; `src/LogFilterProxyModel.h:62`; `src/MainWindow.cpp:1565`; `src/TableSessionManager.cpp:22`, `:35`, `:40`; `src/Utils/TableHeader.h:21`
- **Rule**: PAT-7
- **Finding**: `QMap` usage should be verified for copy/order needs.
- **Mitigation**: Keep only where sorted keys or Qt API compatibility matter; otherwise consider cheaper storage.

### [L-004] Unchecked `open()`

- **File**: `src/LogLevelStyle.cpp:120`
- **Rule**: ERR-1
- **Finding**: `QBuffer::open()` result is ignored before saving PNG data.
- **Mitigation**: Check `open()` and `pixmap.save()` before returning data-url HTML.

### [L-005] `currentDateTime()`

- **File**: `src/MainWindow.cpp:160`, `:166`, `:1220`
- **Rule**: DEP-11
- **Finding**: `QDateTime::currentDateTime()` used.
- **Mitigation**: Prefer `currentDateTimeUtc()` where local time is not required.

### [L-006] Unvalidated JSON parse in test

- **File**: `tests/TestCsv.cpp:93`
- **Rule**: ERR-2
- **Finding**: `QJsonDocument::fromJson()` result is used without validation.
- **Mitigation**: Check parse error / expected object before reading fields.

---

## Deep Analysis Findings

### [D-001] Reentrant event processing during model reset

- **File**: `src/JsonlModel.cpp:266`
- **Category**: Model Contracts
- **Confidence**: 84/100
- **Finding**: `loadDevice()` calls the progress callback between `beginResetModel()` and `endResetModel()`, and the production callback processes events.
- **Trace**: Reset opens at `JsonlModel.cpp:266`; records mutate through the load loop; callback runs at `:280`/`:302`; `MainWindow.cpp:727` calls `QApplication::processEvents()`.
- **Mitigation**: Build data in locals while reporting progress, then reset/swap once; or avoid event pumping during reset.

### [D-002] Rebuilding sessions menu can delete the emitting action

- **File**: `src/MainWindow.cpp:1420`
- **Category**: Ownership & Lifecycle
- **Confidence**: 86/100
- **Finding**: A menu action's `triggered` lambda calls `switchTableSession()`, which calls `rebuildTableSessionsMenu()`, which clears the menu and can delete the currently emitting action.
- **Trace**: Actions are created at `:1382`, connected at `:1386`, menu clears at `:1378`, rebuild is called at `:1420`.
- **Mitigation**: Defer rebuild with a queued call, or avoid clearing menu-owned actions inside their own signal path.

### [D-003] QObject-derived classes do not explicitly disable copy/move

- **File**: `src/MainWindow.h:38`
- **Category**: Ownership & Lifecycle
- **Confidence**: 82/100
- **Finding**: `MainWindow`, `JsonlModel`, `LogFilterProxyModel`, `ActiveCellDelegate`, and `JsonSyntaxHighlighter` rely on base non-copyability rather than declaring intent.
- **Trace**: Headers show Qt polymorphic inheritance and no `Q_DISABLE_COPY_MOVE`.
- **Mitigation**: Add `Q_DISABLE_COPY_MOVE(ClassName)` to each class body.

### [D-004] Case-sensitive fallback table sort

- **File**: `src/LogFilterProxyModel.cpp:279`
- **Category**: API & C++ Correctness
- **Confidence**: 86/100
- **Finding**: Text columns fall back to default `QSortFilterProxyModel::lessThan()`, producing case-sensitive user-visible sorting.
- **Trace**: Only `line` and `ts` are special-cased before base fallback.
- **Mitigation**: Set sort case sensitivity or compare display strings case-insensitively in fallback.

### [D-005] Missing trailing comma in `JsonSyntaxHighlighter::Mode`

- **File**: `src/JsonSyntaxHighlighter.h:25`
- **Category**: API & C++ Correctness
- **Confidence**: 95/100
- **Finding**: Last enumerator lacks trailing comma.
- **Trace**: `Backtrace` is the final enumerator.
- **Mitigation**: Add the trailing comma.

### [D-006] Missing trailing comma in `ThemeManager::Theme`

- **File**: `src/ThemeManager.h:24`
- **Category**: API & C++ Correctness
- **Confidence**: 95/100
- **Finding**: Last enumerator lacks trailing comma.
- **Trace**: `Dark` is the final enumerator.
- **Mitigation**: Add the trailing comma.

### [D-007] JSONL loader marks non-object JSON as valid

- **File**: `src/JsonlModel.cpp:325`
- **Category**: Error Handling
- **Confidence**: 84/100
- **Finding**: Arrays parse as valid rows, but `JsonlRecord::value()` only exposes object fields, so valid rows can display empty data.
- **Trace**: `[` is accepted at `:317`; validity checks only parse errors at `:325`; `JsonlRecord.h:25` returns empty for non-objects.
- **Mitigation**: Require JSON objects for JSONL rows, or define/display array semantics explicitly.

### [D-008] JSONL read failures can still return success

- **File**: `src/JsonlModel.cpp:287`
- **Category**: Error Handling
- **Confidence**: 82/100
- **Finding**: `loadDevice()` reads lines and returns `true` without checking mid-stream device errors.
- **Trace**: `readLine()` at `:287`; unconditional success at `:365`.
- **Mitigation**: Propagate `outError` into `loadDevice()` and check device/file error state after reading.

### [D-009] CSV read errors are not checked after `readAll()`

- **File**: `src/CsvReader.cpp:32`
- **Category**: Error Handling
- **Confidence**: 83/100
- **Finding**: A partial or failed read may be accepted or reported as malformed CSV.
- **Trace**: `open()` is checked at `:24`; `readAll()` is passed directly to `parse()` at `:32`.
- **Mitigation**: Check file error state after read and return I/O errors distinctly.

### [D-010] CSV import fully materializes multiple copies

- **File**: `src/MainWindow.cpp:742`
- **Category**: Error Handling / Performance & Quality
- **Confidence**: 86/100
- **Finding**: CSV import reads full bytes, full UTF-16 text, parsed records, full JSONL bytes, then loads all records again.
- **Trace**: `CsvReader::readFile()`, `ConverterCsvToJsonl::convert()`, and `_model->loadJsonlData()` are chained at `:742-744`.
- **Mitigation**: Add size/row limits or stream CSV conversion directly into model records.

### [D-011] `data()` does expensive work before role rejection

- **File**: `src/JsonlModel.cpp:64`
- **Category**: Performance & Quality
- **Confidence**: 88/100
- **Finding**: Level/error lookups and comparisons happen for roles that later return `{}`.
- **Trace**: Record and column work starts at `:64`; unsupported roles return at `:135`.
- **Mitigation**: Branch on role first and compute only the data needed for that role.

### [D-012] File open rebuilds filters with repeated scans

- **File**: `src/MainWindow.cpp:762`
- **Category**: Performance & Quality
- **Confidence**: 90/100
- **Finding**: Six `updateColumnFilterItems()` calls scan rows and invalidate filtering repeatedly.
- **Trace**: Calls at `:762-767`; each path reaches `applyFilters()`.
- **Mitigation**: Build all distinct filter values in one pass and apply filters once.

### [D-013] Timestamp filtering reparses per row

- **File**: `src/LogFilterProxyModel.cpp:371`
- **Category**: Performance & Quality
- **Confidence**: 86/100
- **Finding**: Each row lookup rediscovers the `ts` column and reparses timestamp text.
- **Trace**: `timestampMatches()` calls `columnByName()` at `:371` and `parseTimestamp()` at `:378`.
- **Mitigation**: Cache timestamp column and parsed timestamp values or expose a typed timestamp role.

### [D-014] Cell selection eagerly formats both value and raw row

- **File**: `src/MainWindow.cpp:876`
- **Category**: Performance & Quality
- **Confidence**: 84/100
- **Finding**: Current-cell changes eagerly run fragment formatting on both cell and raw row.
- **Trace**: Formatting occurs at `:876` and `:882`.
- **Mitigation**: Format lazily when display/copy actually needs formatted text, and reuse existing strings for detection.

---

## Investigation Targets

### [I-001] Bounds validation before model storage access

- **File**: `src/JsonlModel.cpp:64`
- **Category**: Model Contracts
- **Confidence**: 72/100
- **Finding**: `data()` checks `index.isValid()` but not model/row/column bounds.
- **Unverified because**: Normal Qt views usually provide valid in-range indexes.
- **How to verify**: Add `QAbstractItemModelTester` and manual stale/foreign index tests.

### [I-002] `formatFragments` nullable-looking out parameter

- **File**: `src/CodeFormatter.cpp:392`
- **Category**: API & C++ Correctness
- **Confidence**: 74/100
- **Finding**: Public function writes `*changed` without null guard.
- **Unverified because**: Current in-tree callers pass a valid pointer.
- **How to verify**: Decide whether the API contract is required reference-like or optional pointer-like.

### [I-003] Settings writes ignore persistence errors

- **File**: `src/MainWindow.cpp:659`
- **Category**: Error Handling
- **Confidence**: 72/100
- **Finding**: `QSettings::setValue()` paths do not `sync()`/check `status()`.
- **Unverified because**: Losing UI settings may be acceptable for this app.
- **How to verify**: Decide which persisted settings must report disk/permission errors.

### [I-004] Table-session settings lack schema version

- **File**: `src/TableSessionManager.cpp:67`
- **Category**: Error Handling
- **Confidence**: 69/100
- **Finding**: Stored sessions are loaded without version/migration validation.
- **Unverified because**: Current schema may be intentionally simple.
- **How to verify**: Define whether old/incompatible settings need migration guarantees.

### [I-005] Test helper uses `Q_ASSERT` as guard

- **File**: `tests/TestLogFilterProxyModel.cpp:61`
- **Category**: Ownership & Lifecycle
- **Confidence**: 68/100
- **Finding**: Release builds lose the invalid-column guard in `valueAt()`.
- **Unverified because**: It is test-only helper code.
- **How to verify**: Replace with a test assertion if release-mode test behavior matters.

### [I-006] SQL formatter recompiles regexes

- **File**: `src/CodeFormatter.cpp:127`
- **Category**: Performance & Quality
- **Confidence**: 72/100
- **Finding**: Regexes are rebuilt per formatting call.
- **Unverified because**: Actual impact depends on payload size and interaction frequency.
- **How to verify**: Profile selection/formatting on large SQL-heavy logs.

### [I-007] Status rendering regenerates icon data URLs

- **File**: `src/MainWindow.cpp:1585`
- **Category**: Performance & Quality
- **Confidence**: 74/100
- **Finding**: Status updates rebuild base64 PNG icon HTML even when counts have not changed.
- **Unverified because**: Cost may be small at current level counts.
- **How to verify**: Profile status updates during rapid selection/filtering.

### [I-008] `MainWindow` responsibilities are becoming coupled

- **File**: `src/MainWindow.cpp:75`
- **Category**: Performance & Quality
- **Confidence**: 68/100
- **Finding**: File loading, filtering, sessions, preview formatting, search, and status rendering interact in performance-sensitive paths.
- **Unverified because**: This is architectural judgment rather than a single defect.
- **How to verify**: Start with the confirmed filter/preview performance issues and see whether helper extraction reduces coupling.

---

## Summary

| Category | Lint | Deep | Investigate | Total |
|----------|------|------|-------------|-------|
| Deprecated/API hygiene | 15 | 3 | 1 | 19 |
| Model Contracts | 0 | 1 | 1 | 2 |
| Ownership & Lifecycle | 0 | 2 | 1 | 3 |
| Thread Safety | 0 | 0 | 0 | 0 |
| Error Handling & Validation | 2 | 4 | 2 | 8 |
| Performance & Quality | 10 | 4 | 3 | 17 |
| **Total** | **27** | **14** | **8** | **49** |

Findings below confidence 60 are suppressed entirely.
