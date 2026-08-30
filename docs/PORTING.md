# Porting Notes: Go → C

This document describes the key design decisions and mapping between the
Go `hebcal-go` source and the C `hebcal-c` port.

## Module Mapping

| Go source | C file(s) | Notes |
|-----------|----------|-------|
| `hebcal/greg` | `greg.c/h` | Floor-division helper replaces Go `quotient`. Julian/Gregorian 1752 transition preserved. |
| `hebcal/hdate` | `hdate.c/h`, `tables_data.h` | `init()` tables → precomputed `static const` (PROGMEM on AVR). Cache omitted (O(1) computation). |
| `hebcal/hdate/anniversary` | `anniversary.c/h` | Direct port. |
| `hebcal/gematriya` | `gematriya.c` | Hebrew letters as UTF-8 escape sequences. |
| `hebcal-go/molad` | `molad.c/h` | Direct port. |
| `hebcal/locales` | `locale.c/h`, `locale_data.h` | Reduced to en + he (with/without nikud). 586 Hebrew entries extracted via `tools/gen_locales.py`. Binary search lookup. |
| `hebcal-go/event` | `event.c/h` | Event struct + flags + category mapping. |
| `hebcal-go/sedra` | `sedra.c/h` | 54 parshiyot + 14 sedra arrays + lookup logic. |
| `hebcal-go/omer` | `omer.c/h` | Full Hebrew "Today is..." text ported. Ordinal formatter replaces `go-humanize`. |
| `hebcal/noaa-go` | `noaa.c/h` | NOAACalculator only (not Meeus/VSOP87). 365-entry solar radius table in PROGMEM. |
| `hebcal-go/zmanim` | `zmanim.c/h` | Times as UTC `double` hours (NaN = no event). No timezone DB needed. |
| `hebcal-go/hebcal` | `hebcal.c/h`, `holidays.c/h` | Holiday definitions + calendar orchestrator. |

## Key Design Decisions

### 1. `int64_t` R.D. everywhere
Matches Go exactly, no overflow for any realistic year. Correct (if slower) on AVR.

### 2. Precomputed tables (replacing Go `init()`)
`tools/gen_tables.c` generates `tables_data.h` at build time. The tables
(daysInMonthTab, monthOffsetTab, monthByDayOfYear) are `static const`
with `PROGMEM` on AVR, keeping ~2.5KB of lookup data in flash instead of RAM.

### 3. Timezone abstraction
Go's `time.Location` → a simple `int utc_offset` (minutes from UTC).
Zmanim computes in UTC, then the caller adds the offset. ESP32 uses SNTP +
`setenv("TZ",...)`; desktop CLI uses libc. No tz database needed on MCU.

### 4. `double` math for zmanim
Matches Go's precision. ESP32 (soft-double) is fine; AVR is slow but works
for occasional zmanim. Documented with a compile switch option for `float`
speed if needed in future.

### 5. No heap in core path
Fixed buffers for string rendering. Holiday/event lists use caller-provided
arrays. `malloc` only on ESP32 where available, behind a compile switch.

### 6. PROGMEM / `arduino_compat.h`
`PROGMEM` and `pgm_read_*` are no-ops on desktop/ESP32, real macros on AVR.
The same source compiles for all three targets unchanged.

## What Was NOT Ported

- **Meeus/VSOP87 calculator** — the high-accuracy solar position algorithm.
  NOAACalculator is sufficient for all zmanim (±1 minute). Can be added later.
- **Daf Yomi / Mishna Yomi / Nach Yomi** — daily learning schedules live in
  a separate `hebcal/learning` module not in `go.mod`. Out of scope.
- **Ashkenazi/French/Spanish/Russian locales** — only English + Hebrew included.
  The locale system supports adding more locales by extending `locale_data.h`.
- **JSON marshal/unmarshal** — desktop-only, not needed on MCU.
- **URL generation** — for hebcal.com web links, not needed on MCU.
- **City database** — `zmanim/cities.go` has 500+ cities. Not included; user
  provides lat/lon/elevation directly.

## Test Strategy

Each Go `*_test.go` becomes a `tests/test_*.c` with identical inputs and expected
values. A tiny `hebtest.h` provides assert macros. `make test` builds and runs
all tests. The Go suite (`go test ./...` in `hebcal-go/`) stays untouched as
the reference.

Current test count: **1091 checks** across 13 test files.
