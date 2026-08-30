# Hebcal ESPHome Component

A native ESPHome component that computes the Hebrew (Jewish) calendar
**locally on the ESP32** — no HTTP API calls required.

## Features

- Hebrew date (e.g. "15 Cheshvan 5789" or Hebrew "ט״ו חֶשְׁוָן תשפ״ט")
- Holiday detection (all major/minor holidays, fasts, special Shabbatot)
- Parashat HaShavua (weekly Torah reading)
- Sefirat HaOmer (with full Hebrew text)
- Candle-lighting & Havdalah times (requires lat/lon)
- Sunrise / sunset / chatzot as numeric sensors (requires lat/lon)
- Israel or Diaspora schedule
- English or Hebrew output

## Installation

### Option 1: Local source (recommended for development)

Clone this repo alongside your ESPHome config, then reference it:

```yaml
external_components:
  - source:
      type: local
      path: ../hebcal-c/components
    components: [ hebcal ]
```

### Option 2: Git source

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/harrislloydlevy/hebcal-go
      ref: hebcal-c
    components: [ hebcal ]
```

## Configuration

```yaml
# Required: a time source so the ESP32 knows the current date
time:
  - platform: sntp
    id: sntp_time
    timezone: "America/New_York"

hebcal:
  id: my_hebcal
  time_id: sntp_time

  # Location (required for zmanim/candle-lighting)
  latitude: 40.09789
  longitude: -74.21764
  elevation: 0.0

  # Options
  israel: false
  locale: "en"          # "he" for Hebrew, "he-x-NoNikud" for Hebrew without vowel points
  candle_lighting: true
  candle_lighting_mins: 18
  havdalah_mins: 0      # 0 = use degree-based havdalah
  havdalah_deg: 8.5     # 8.5 = 3 small stars, 7.083 = 3 medium stars
  omer: true
  sedrot: true
  update_interval: 1h

  # Text sensors
  hebrew_date:
    name: "Hebrew Date"
  parasha:
    name: "Parasha"
  holiday:
    name: "Holiday"
  candles:
    name: "Candle Lighting"
  havdalah:
    name: "Havdalah"
  omer_text:
    name: "Omer"
  day_of_week:
    name: "Day of Week"

  # Numeric sensors (epoch seconds)
  sunrise:
    name: "Sunrise"
  sunset:
    name: "Sunset"
  chatzot:
    name: "Chatzot"
```

## Configuration Variables

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `time_id` | ID | **Required** | ID of your time component (SNTP, etc.) |
| `latitude` | float | — | Latitude for zmanim (north positive) |
| `longitude` | float | — | Longitude for zmanim (east positive) |
| `elevation` | float | 0 | Elevation in meters |
| `israel` | bool | false | Use Israel holiday schedule |
| `locale` | string | "en" | "en", "he", or "he-x-NoNikud" |
| `candle_lighting` | bool | false | Enable candle-lighting time computation |
| `candle_lighting_mins` | int | 18 | Minutes before sunset for candle lighting |
| `havdalah_mins` | int | 0 | Minutes after sunset (0 = use degrees) |
| `havdalah_deg` | float | 8.5 | Solar depression angle for havdalah |
| `use_elevation` | bool | false | Use elevation in sunrise/sunset |
| `omer` | bool | false | Include Omer counting |
| `sedrot` | bool | false | Include weekly Torah reading |
| `molad` | bool | false | Include molad announcements |
| `update_interval` | duration | 1h | How often to recompute |

## Using in Lambdas

You can access the component's data in display lambdas:

```yaml
display:
  - platform: ...
    lambda: |-
      it.printf(0, 0, id(hebrew_date).state.c_str());
      it.printf(0, 20, id(holiday).state.c_str());
      if (!id(candles).state.empty()) {
        it.printf(0, 40, "Candles: %s", id(candles).state.c_str());
      }
```

## Flash Usage

The C library adds approximately:
- ~60 KB flash (code + static tables + Hebrew locale strings)
- ~2 KB RAM (stack buffers during computation)

On an ESP32 with 4 MB flash, this is well under 5% of available space.
