# hebcal-c

A perpetual Jewish Calendar library in C, ported from the [hebcal-go](https://github.com/hebcal/hebcal-go) Go library. Designed for **Arduino** and **ESP32**, while also building as a desktop CLI and test suite.

## Features

- **Hebrew ↔ Gregorian date conversion** (using Rata Die day numbers)
- **Holiday lookup** — all Jewish holidays for any year (Israel & Diaspora schedules)
- **Halachic times (Zmanim)** — sunrise, sunset, chatzot, alot hashachar, tzeit, fast start/end
- **Parashat HaShavua** — weekly Torah readings with doubled/triple parsha handling
- **Sefirat HaOmer** — counting of the Omer with full Hebrew and English rendering
- **Molad** — new moon calculation
- **Yahrzeit & Birthday** — Hebrew anniversary calculations
- **Gematriya** — Hebrew numerals (numbers as Hebrew letters)
- **English + Hebrew** locale support (with and without nikud)

## Quick Start

### Desktop (build & test)

```bash
make test     # build and run 1091-test C test suite
make cli      # build the hebcal-c command-line tool
```

### Command-line tool

```bash
# Hebrew → Gregorian
./hebcal-c 2g 15 Cheshvan 5769
# → 2008-11-13 (Thursday)

# Gregorian → Hebrew
./hebcal-c 2h 2008-11-13
# → 15 Cheshvan 5769

# Holidays for a Gregorian year
./hebcal-c -y 2026

# Holidays with Omer and Torah readings, Israel schedule
./hebcal-c -y 2026 -o -s -i

# Hebrew output
./hebcal-c -y 2026 --he

# Halachic times for a location (lat,lon,UTC_offset_minutes)
./hebcal-c -z 40.097,-74.222,-300 2026

# Yahrzeit dates
./hebcal-c --yahrzeit 2017-2013
```

### Arduino

Install the `Hebcal` library in your Arduino IDE or PlatformIO, then:

```cpp
#include <Arduino.h>
#include "hebcal.h"

void setup() {
    Serial.begin(9600);

    // Convert today's Gregorian date to Hebrew
    heb_hdate_t hd = heb_hdate_from_gregorian(2026, HEB_AUGUST, 30);
    char buf[64];
    heb_hdate_to_string(&hd, buf, sizeof(buf));
    Serial.println(buf);

    // Check for holidays on this date
    heb_event_t events[10];
    int n = heb_holidays_for_date(&hd, 0, events, 10);
    for (int i = 0; i < n; i++) {
        Serial.println(events[i].desc);
    }
}
```

See the `examples/` folder for complete sketches:
- `HebrewDateDemo` — basic date conversion (AVR + ESP32)
- `HolidaysOfYear` — list all holidays for a year
- `ZmanimESP32` — halachic times with WiFi/NTP (ESP32 only)

## Architecture

```
hebcal-c/
  src/           # C library source (shared by CLI, tests, and Arduino)
  cli/           # Command-line tool
  tests/         # 1:1 mirrored C test suite (1091 checks)
  examples/      # Arduino sketches
  tools/         # Build-time table generators
  docs/          # Porting notes
```

The same `src/` code compiles for:
- **Desktop** (gcc/clang) — via Makefile
- **ESP32** (xtensa-gcc) — via Arduino IDE or PlatformIO
- **AVR** (avr-gcc) — via Arduino IDE (tables in PROGMEM, `int64_t` R.D.)

## Target Notes

| Target | int64 R.D. | PROGMEM | float math | RAM |
|--------|-----------|---------|-------------|-----|
| ESP32  | native    | no-op   | native double | 520KB |
| AVR    | emulated  | yes     | soft double (slow) | 2-8KB |
| Desktop| native    | no-op   | native double | — |

On AVR, the year-type lookup tables and Hebrew locale strings live in flash (PROGMEM), not RAM. The `int64_t` R.D. math is correct but slower than on ESP32/desktop.

## License

- **GPL-2.0-or-later** for the calendar/holiday/sedra/omer/event code (derived from hebcal-go)
- **LGPL-2.1** for the NOAA solar astronomy code (`noaa.c`, derived from noaa-go/KosherJava)

See `LICENSE` for details.
