/*
 * hebcal-c ESPHome component — C++ wrapper.
 *
 * Bridges the pure-C hebcal-c library into ESPHome's PollingComponent
 * framework, exposing Hebrew date, holidays, parasha, zmanim, and omer
 * as text sensors and numeric sensors.
 *
 * License: GPL-2.0-or-later
 */
#pragma once

#include "esphome.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/time/real_time.h"

// Include the C library — arduino_compat.h makes it compile clean on ESP32
#include "hebcal_all.h"

#include <string>
#include <time.h>

namespace esphome {
namespace hebcal {

static const char *const TAG = "hebcal";

class HebcalComponent : public PollingComponent {
 public:
  void setup() override {
    ESP_LOGCONFIG(TAG, "Setting up Hebcal component");
    // Nothing else to do at boot; update() will fire on the first interval
  }

  void update() override {
    auto time = this->time_->now();
    if (!time.is_valid()) {
      ESP_LOGW(TAG, "Time not yet synced, skipping update");
      return;
    }

    int year = time.year;
    int month = time.month;  // 1-12
    int day = time.day_of_month;
    int utc_offset = time.timezone_offset() / 60;  // seconds → minutes

    ESP_LOGD(TAG, "Computing Hebrew calendar for %04d-%02d-%02d (UTC offset %d min)",
             year, month, day, utc_offset);

    // --- Hebrew date ---
    heb_hdate_t hd = heb_hdate_from_gregorian(year, (heb_month_greg_t)month, day);
    char hd_buf[64];
    heb_hdate_to_string(&hd, hd_buf, sizeof(hd_buf));

    if (this->hebrew_date_ != nullptr)
      this->hebrew_date_->publish_state(std::string(hd_buf));

    // --- Day of week ---
    if (this->dow_ != nullptr) {
      const char *dows[] = {"Sunday", "Monday", "Tuesday", "Wednesday",
                            "Thursday", "Friday", "Saturday"};
      int dow = (int)heb_hdate_weekday(&hd);
      this->dow_->publish_state(std::string(dows[dow]));
    }

    // --- Holiday lookup ---
    std::string holiday_str;
    std::string candles_str;
    std::string havdalah_str;

    static heb_event_t events[30];
    int n = heb_holidays_for_date(&hd, this->israel_, events, 30);

    for (int i = 0; i < n; i++) {
      char desc[128];
      heb_event_render(&events[i], this->locale_.c_str(), desc, sizeof(desc));

      // Collect holiday names
      if (!holiday_str.empty()) holiday_str += "; ";
      holiday_str += desc;

      // Candle lighting / havdalah detection
      if (events[i].flags & HEB_F_LIGHT_CANDLES) {
        if (this->candle_lighting_ && this->has_location_) {
          double cl = this->compute_candle_lighting(year, month, day, utc_offset);
          if (!isnan(cl)) {
            candles_str = std::string(desc) + ": " + this->format_local_time(cl, utc_offset);
          }
        } else {
          candles_str = desc;
        }
      }
      if (events[i].flags & HEB_F_YOM_TOV_ENDS) {
        if (this->candle_lighting_ && this->has_location_) {
          double hv = this->compute_havdalah(year, month, day, utc_offset);
          if (!isnan(hv)) {
            havdalah_str = std::string(desc) + ": " + this->format_local_time(hv, utc_offset);
          }
        } else {
          havdalah_str = desc;
        }
      }
    }

    if (this->holiday_ != nullptr)
      this->holiday_->publish_state(holiday_str);
    if (this->candles_ != nullptr)
      this->candles_->publish_state(candles_str);
    if (this->havdalah_ != nullptr)
      this->havdalah_->publish_state(havdalah_str);

    // --- Parasha (weekly Torah reading) ---
    if (this->sedrot_ && this->parasha_ != nullptr) {
      heb_sedra_t sedra = heb_sedra_new(hd.year, this->israel_);
      heb_parsha_t p = heb_sedra_lookup(&sedra, &hd);
      if (!p.chag) {
        char p_buf[128];
        heb_parsha_render(&p, this->locale_.c_str(), p_buf, sizeof(p_buf));
        this->parasha_->publish_state(std::string(p_buf));
      }
    }

    // --- Omer ---
    if (this->omer_ && this->omer_text_ != nullptr) {
      // Omer is counted from 16 Nisan through 5 Sivan
      heb_hdate_t omer_start;
      heb_hdate_create(&omer_start, hd.year, HEB_H_NISAN, 16);
      heb_hdate_t omer_end;
      heb_hdate_create(&omer_end, hd.year, HEB_H_SIVAN, 5);
      if (hd.abs >= omer_start.abs && hd.abs <= omer_end.abs) {
        int omer_day = (int)(hd.abs - omer_start.abs) + 1;
        heb_omer_t omer = heb_omer_new(hd, omer_day);
        char o_buf[128];
        heb_omer_render(&omer, this->locale_.c_str(), o_buf, sizeof(o_buf));
        this->omer_text_->publish_state(std::string(o_buf));
      } else {
        this->omer_text_->publish_state("");
      }
    }

    // --- Zmanim (numeric sensors as epoch seconds) ---
    if (this->has_location_) {
      heb_zmanim_t z = heb_zmanim_new(this->latitude_, this->longitude_,
                                      this->elevation_, year, month, day,
                                      utc_offset);
      z.use_elevation = this->use_elevation_;

      double sunrise = heb_zmanim_sunrise(&z);
      double sunset = heb_zmanim_sunset(&z);
      double chatzot = heb_zmanim_chatzot(&z);

      if (this->sunrise_sensor_ != nullptr && !isnan(sunrise))
        this->sunrise_sensor_->publish_state(this->utc_hours_to_epoch(sunrise, year, month, day));
      if (this->sunset_sensor_ != nullptr && !isnan(sunset))
        this->sunset_sensor_->publish_state(this->utc_hours_to_epoch(sunset, year, month, day));
      if (this->chatzot_sensor_ != nullptr && !isnan(chatzot))
        this->chatzot_sensor_->publish_state(this->utc_hours_to_epoch(chatzot, year, month, day));
    }

    ESP_LOGD(TAG, "Hebrew date: %s, holidays: %s", hd_buf,
             holiday_str.empty() ? "(none)" : holiday_str.c_str());
  }

  // --- Configuration setters ---
  void set_time(time::RealTime *t) { time_ = t; }
  void set_latitude(double lat) { latitude_ = lat; has_location_ = true; }
  void set_longitude(double lon) { longitude_ = lon; }
  void set_elevation(double elev) { elevation_ = elev; }
  void set_israel(bool v) { israel_ = v; }
  void set_locale(const std::string &v) { locale_ = v; }
  void set_candle_lighting(bool v) { candle_lighting_ = v; }
  void set_candle_lighting_mins(int v) { candle_lighting_mins_ = v; }
  void set_havdalah_mins(int v) { havdalah_mins_ = v; }
  void set_havdalah_deg(double v) { havdalah_deg_ = v; }
  void set_use_elevation(bool v) { use_elevation_ = v; }
  void set_omer(bool v) { omer_ = v; }
  void set_sedrot(bool v) { sedrot_ = v; }
  void set_molad(bool v) { molad_ = v; }
  void set_no_rosh_chodesh(bool v) { no_rosh_chodesh_ = v; }
  void set_no_modern(bool v) { no_modern_ = v; }
  void set_no_minor_fast(bool v) { no_minor_fast_ = v; }
  void set_no_special_shabbat(bool v) { no_special_shabbat_ = v; }

  // Text sensor setters
  void set_hebrew_date(text_sensor::TextSensor *s) { hebrew_date_ = s; }
  void set_parasha(text_sensor::TextSensor *s) { parasha_ = s; }
  void set_holiday(text_sensor::TextSensor *s) { holiday_ = s; }
  void set_candles(text_sensor::TextSensor *s) { candles_ = s; }
  void set_havdalah(text_sensor::TextSensor *s) { havdalah_ = s; }
  void set_omer_text(text_sensor::TextSensor *s) { omer_text_ = s; }
  void set_dow(text_sensor::TextSensor *s) { dow_ = s; }

  // Numeric sensor setters
  void set_sunrise_sensor(sensor::Sensor *s) { sunrise_sensor_ = s; }
  void set_sunset_sensor(sensor::Sensor *s) { sunset_sensor_ = s; }
  void set_chatzot_sensor(sensor::Sensor *s) { chatzot_sensor_ = s; }

 protected:
  time::RealTime *time_{nullptr};

  // Location
  double latitude_{0};
  double longitude_{0};
  double elevation_{0};
  bool has_location_{false};

  // Options
  bool israel_{false};
  std::string locale_{"en"};
  bool candle_lighting_{false};
  int candle_lighting_mins_{18};
  int havdalah_mins_{0};  // 0 = use degrees
  double havdalah_deg_{8.5};
  bool use_elevation_{false};
  bool omer_{false};
  bool sedrot_{false};
  bool molad_{false};
  bool no_rosh_chodesh_{false};
  bool no_modern_{false};
  bool no_minor_fast_{false};
  bool no_special_shabbat_{false};

  // Text sensors
  text_sensor::TextSensor *hebrew_date_{nullptr};
  text_sensor::TextSensor *parasha_{nullptr};
  text_sensor::TextSensor *holiday_{nullptr};
  text_sensor::TextSensor *candles_{nullptr};
  text_sensor::TextSensor *havdalah_{nullptr};
  text_sensor::TextSensor *omer_text_{nullptr};
  text_sensor::TextSensor *dow_{nullptr};

  // Numeric sensors
  sensor::Sensor *sunrise_sensor_{nullptr};
  sensor::Sensor *sunset_sensor_{nullptr};
  sensor::Sensor *chatzot_sensor_{nullptr};

  // --- Helpers ---

  // Convert UTC decimal hours to Unix epoch seconds
  double utc_hours_to_epoch(double utc_hours, int year, int month, int day) {
    struct tm t = {};
    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
    t.tm_isdst = 0;
    time_t epoch = timegm(&t);
    return (double)epoch + utc_hours * 3600.0;
  }

  // Format UTC decimal hours as local HH:MM string
  std::string format_local_time(double utc_hours, int utc_offset_min) {
    double local = utc_hours + (double)utc_offset_min / 60.0;
    while (local < 0) local += 24;
    while (local >= 24) local -= 24;
    int h = (int)local;
    int m = (int)((local - h) * 60 + 0.5);
    if (m >= 60) { m -= 60; h++; }
    if (h >= 24) h -= 24;
    char buf[16];
    snprintf(buf, sizeof(buf), "%d:%02d", h, m);
    return std::string(buf);
  }

  // Compute candle-lighting time as UTC decimal hours
  double compute_candle_lighting(int year, int month, int day, int utc_offset) {
    heb_zmanim_t z = heb_zmanim_new(latitude_, longitude_, elevation_,
                                    year, month, day, utc_offset);
    z.use_elevation = use_elevation_;
    double sunset = heb_zmanim_sunset(&z);
    if (isnan(sunset)) return NAN;
    return sunset - (double)candle_lighting_mins_ / 60.0;
  }

  // Compute havdalah time as UTC decimal hours
  double compute_havdalah(int year, int month, int day, int utc_offset) {
    heb_zmanim_t z = heb_zmanim_new(latitude_, longitude_, elevation_,
                                    year, month, day, utc_offset);
    z.use_elevation = use_elevation_;
    if (havdalah_mins_ > 0) {
      double sunset = heb_zmanim_sunset(&z);
      if (isnan(sunset)) return NAN;
      return sunset + (double)havdalah_mins_ / 60.0;
    }
    return heb_zmanim_tzeit(&z, havdalah_deg_);
  }
};

}  // namespace hebcal
}  // namespace esphome
