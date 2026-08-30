/*
 * hebcal-c - Halachic times (Zmanim).
 * Ported from github.com/hebcal/hebcal-go/zmanim (Go).
 *
 * License: GPL-2.0-or-later (zmanim), LGPL-2.1 (NOAA astronomy)
 */
#include "zmanim.h"
#include "greg.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#ifndef NAN
#define NAN (0.0/0.0)
#endif

heb_zmanim_t heb_zmanim_new(double lat, double lon, double elevation,
                            int year, int month, int day, int utc_offset) {
    heb_zmanim_t z;
    z.geo.latitude = lat;
    z.geo.longitude = lon;
    z.geo.elevation = elevation;
    z.calc = heb_noaa_new();
    z.year = year;
    z.month = month;
    z.day = day;
    z.utc_offset = utc_offset;
    z.use_elevation = 0;
    return z;
}

/* === Core solar events === */

double heb_zmanim_sunrise(const heb_zmanim_t *z) {
    return heb_noaa_utc_sunrise(&z->calc, &z->geo, z->year, z->month, z->day,
                                HEB_GEOMETRIC_ZENITH, z->use_elevation);
}

double heb_zmanim_sunset(const heb_zmanim_t *z) {
    return heb_noaa_utc_sunset(&z->calc, &z->geo, z->year, z->month, z->day,
                               HEB_GEOMETRIC_ZENITH, z->use_elevation);
}

double heb_zmanim_sea_level_sunrise(const heb_zmanim_t *z) {
    return heb_noaa_utc_sunrise(&z->calc, &z->geo, z->year, z->month, z->day,
                                HEB_GEOMETRIC_ZENITH, 0);
}

double heb_zmanim_sea_level_sunset(const heb_zmanim_t *z) {
    return heb_noaa_utc_sunset(&z->calc, &z->geo, z->year, z->month, z->day,
                               HEB_GEOMETRIC_ZENITH, 0);
}

/* Compute previous Gregorian date */
static void prev_date(int year, int month, int day,
                      int *py, int *pm, int *pd) {
    day--;
    if (day < 1) {
        month--;
        if (month < 1) {
            year--;
            month = 12;
        }
        day = heb_greg_days_in((heb_month_greg_t)month, year);
    }
    *py = year;
    *pm = month;
    *pd = day;
}

double heb_zmanim_greg_eve(const heb_zmanim_t *z) {
    int py, pm, pd;
    prev_date(z->year, z->month, z->day, &py, &pm, &pd);
    /* Sunset is on the previous day; need to handle date rollover for UTC.
     * The NOAA calculator returns UTC hours in [0,24). For most locations,
     * the previous day's sunset in UTC will be > 12 (afternoon/evening UTC).
     * But for locations far west, it might wrap. We return the raw value
     * and let the caller handle any needed adjustment. */
    return heb_noaa_utc_sunset(&z->calc, &z->geo, py, pm, pd,
                               HEB_GEOMETRIC_ZENITH, z->use_elevation);
}

/* === Halachic hour calculations === */

/* Returns the daylight duration in hours, handling the case where
 * sunset wraps past midnight in UTC (sunset < sunrise means next day). */
static double daylight_duration(const heb_zmanim_t *z) {
    double rise = heb_zmanim_sunrise(z);
    double set = heb_zmanim_sunset(z);
    if (isnan(rise) || isnan(set)) return NAN;
    if (set < rise) set += 24.0;
    return set - rise;
}

double heb_zmanim_hour(const heb_zmanim_t *z) {
    return daylight_duration(z) / 12.0;
}

double heb_zmanim_night_hour(const heb_zmanim_t *z) {
    double set = heb_zmanim_greg_eve(z);
    double rise = heb_zmanim_sunrise(z);
    if (isnan(set) || isnan(rise)) return NAN;
    /* GregEve is previous day's sunset. In UTC, it might be > sunrise
     * (if both are on the same UTC day, which is unusual) or < sunrise.
     * The night period goes from eve sunset to today's sunrise.
     * If eve_sunset > sunrise, it means eve_sunset is actually on the
     * previous UTC day, so subtract 24. */
    if (set > rise) set -= 24.0;
    return (rise - set) / 12.0;
}

double heb_zmanim_hour_offset(const heb_zmanim_t *z, double hours) {
    double rise = heb_zmanim_sunrise(z);
    double h = heb_zmanim_hour(z);
    if (isnan(rise) || isnan(h)) return NAN;
    return rise + hours * h;
}

double heb_zmanim_night_hour_offset(const heb_zmanim_t *z, double hours) {
    double set = heb_zmanim_greg_eve(z);
    double nh = heb_zmanim_night_hour(z);
    if (isnan(set) || isnan(nh)) return NAN;
    if (set > heb_zmanim_sunrise(z)) set -= 24.0;
    return set + hours * nh;
}

double heb_zmanim_chatzot(const heb_zmanim_t *z) {
    return heb_zmanim_hour_offset(z, 6);
}

double heb_zmanim_chatzot_night(const heb_zmanim_t *z) {
    double rise = heb_zmanim_sunrise(z);
    double nh = heb_zmanim_night_hour(z);
    if (isnan(rise) || isnan(nh)) return NAN;
    return rise - 6 * nh;
}

/* === Degree-based times === */

double heb_zmanim_time_at_angle(const heb_zmanim_t *z, double angle, int rising) {
    double zenith = HEB_GEOMETRIC_ZENITH + angle;
    if (rising) {
        return heb_noaa_utc_sunrise(&z->calc, &z->geo, z->year, z->month, z->day,
                                    zenith, 0);
    }
    return heb_noaa_utc_sunset(&z->calc, &z->geo, z->year, z->month, z->day,
                               zenith, 0);
}

double heb_zmanim_dawn(const heb_zmanim_t *z) {
    return heb_zmanim_time_at_angle(z, 6.0, 1);
}

double heb_zmanim_dusk(const heb_zmanim_t *z) {
    return heb_zmanim_time_at_angle(z, 6.0, 0);
}

double heb_zmanim_alot_hashachar(const heb_zmanim_t *z) {
    return heb_zmanim_time_at_angle(z, 16.1, 1);
}

double heb_zmanim_misheyakir(const heb_zmanim_t *z) {
    return heb_zmanim_time_at_angle(z, 11.5, 1);
}

double heb_zmanim_misheyakir_machmir(const heb_zmanim_t *z) {
    return heb_zmanim_time_at_angle(z, 10.2, 1);
}

double heb_zmanim_tzeit(const heb_zmanim_t *z, double angle) {
    if (angle == 0) angle = HEB_TZEIT_3_SMALL_STARS;
    return heb_zmanim_time_at_angle(z, angle, 0);
}

double heb_zmanim_bein_hashmashos(const heb_zmanim_t *z) {
    double tzeis = heb_zmanim_tzeit(z, HEB_TZEIT_3_MEDIUM_STARS);
    if (isnan(tzeis)) return NAN;
    return tzeis - 13.5 / 60.0; /* 13.5 minutes before tzeis 7.083 */
}

/* === Gra zmanim === */

double heb_zmanim_sof_zman_shma(const heb_zmanim_t *z) {
    return heb_zmanim_hour_offset(z, 3);
}

double heb_zmanim_sof_zman_tfilla(const heb_zmanim_t *z) {
    return heb_zmanim_hour_offset(z, 4);
}

double heb_zmanim_mincha_gedola(const heb_zmanim_t *z) {
    return heb_zmanim_hour_offset(z, 6.5);
}

double heb_zmanim_mincha_ketana(const heb_zmanim_t *z) {
    return heb_zmanim_hour_offset(z, 9.5);
}

double heb_zmanim_plag_hamincha(const heb_zmanim_t *z) {
    return heb_zmanim_hour_offset(z, 10.75);
}

/* === MGA zmanim === */

static double sof_zman_mga(const heb_zmanim_t *z, double hours) {
    double sunrise = heb_zmanim_sea_level_sunrise(z);
    double sunset = heb_zmanim_sea_level_sunset(z);
    if (isnan(sunrise) || isnan(sunset)) return NAN;
    double alot72 = sunrise - 72.0 / 60.0;  /* 72 minutes before sunrise */
    double tzeit72 = sunset + 72.0 / 60.0;  /* 72 minutes after sunset */
    if (tzeit72 < alot72) tzeit72 += 24.0;
    double temporalHour = (tzeit72 - alot72) / 12.0;
    return alot72 + hours * temporalHour;
}

double heb_zmanim_sof_zman_shma_mga(const heb_zmanim_t *z) {
    return sof_zman_mga(z, 3);
}

double heb_zmanim_sof_zman_tfilla_mga(const heb_zmanim_t *z) {
    return sof_zman_mga(z, 4);
}

/* === Offset times === */

double heb_zmanim_sunrise_offset(const heb_zmanim_t *z, int offset, int round_time) {
    double t = heb_zmanim_sunrise(z);
    if (isnan(t)) return NAN;
    /* offset is in minutes; convert to hours */
    double result = t + (double)offset / 60.0;
    if (round_time) {
        /* Round to nearest minute (seconds = 0) */
        double total_minutes = result * 60.0;
        if (offset > 0) {
            /* For positive offsets, round up if seconds >= 30 */
            total_minutes = floor(total_minutes);
            double secs = (result * 3600.0) - total_minutes * 60.0;
            if (secs >= 30.0) total_minutes += 1.0;
        } else {
            total_minutes = floor(total_minutes);
        }
        result = total_minutes / 60.0;
    }
    return result;
}

double heb_zmanim_sunset_offset(const heb_zmanim_t *z, int offset, int round_time) {
    double t = heb_zmanim_sunset(z);
    if (isnan(t)) return NAN;
    double result = t + (double)offset / 60.0;
    if (round_time) {
        double total_minutes = result * 60.0;
        if (offset > 0) {
            total_minutes = floor(total_minutes);
            double secs = (result * 3600.0) - total_minutes * 60.0;
            if (secs >= 30.0) total_minutes += 1.0;
        } else {
            total_minutes = floor(total_minutes);
        }
        result = total_minutes / 60.0;
    }
    return result;
}

/* === Utility === */

void heb_zmanim_format_time(double utc_hours, int utc_offset, char *buf, size_t n) {
    if (isnan(utc_hours)) {
        snprintf(buf, n, "undefined");
        return;
    }
    /* Convert to local time */
    double local = utc_hours + (double)utc_offset / 60.0;
    /* Wrap to [0, 24) */
    while (local < 0) local += 24;
    while (local >= 24) local -= 24;
    /* Convert to total seconds, then round */
    long total_sec = (long)(local * 3600.0 + 0.5);
    int h = (int)(total_sec / 3600);
    int m = (int)((total_sec % 3600) / 60);
    int s = (int)(total_sec % 60);
    snprintf(buf, n, "%02d:%02d:%02d", h, m, s);
}
