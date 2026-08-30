/*
 * hebcal-c - Halachic times (Zmanim).
 * Ported from github.com/hebcal/hebcal-go/zmanim (Go).
 *
 * License: GPL-2.0-or-later (zmanim), LGPL-2.1 (NOAA astronomy)
 */
#ifndef HEB_ZMANIM_H
#define HEB_ZMANIM_H

#include "noaa.h"
#include <stddef.h>

/* Zmanim configuration and state. */
typedef struct {
    heb_geolocation_t geo;   /* lat, lon, elevation */
    heb_noaa_calc_t calc;    /* solar calculator config */
    int year;                /* Gregorian year */
    int month;               /* Gregorian month (1-12) */
    int day;                 /* Gregorian day (1-31) */
    int utc_offset;          /* minutes from UTC (east positive) */
    int use_elevation;       /* 1 = adjust for elevation */
} heb_zmanim_t;

/* Constructs a Zmanim for the given location and date. */
heb_zmanim_t heb_zmanim_new(double lat, double lon, double elevation,
                            int year, int month, int day, int utc_offset);

/* --- Core solar events (UTC decimal hours, NaN if no event) --- */
double heb_zmanim_sunrise(const heb_zmanim_t *z);
double heb_zmanim_sunset(const heb_zmanim_t *z);
double heb_zmanim_sea_level_sunrise(const heb_zmanim_t *z);
double heb_zmanim_sea_level_sunset(const heb_zmanim_t *z);
double heb_zmanim_greg_eve(const heb_zmanim_t *z);  /* previous day's sunset */

/* --- Halachic times (UTC decimal hours, NaN if no event) --- */
double heb_zmanim_hour(const heb_zmanim_t *z);       /* sha'ah zemanit in hours */
double heb_zmanim_night_hour(const heb_zmanim_t *z); /* night hour in hours */
double heb_zmanim_hour_offset(const heb_zmanim_t *z, double hours);
double heb_zmanim_night_hour_offset(const heb_zmanim_t *z, double hours);

double heb_zmanim_chatzot(const heb_zmanim_t *z);
double heb_zmanim_chatzot_night(const heb_zmanim_t *z);

/* --- Degree-based times (UTC decimal hours, NaN if no event) --- */
double heb_zmanim_time_at_angle(const heb_zmanim_t *z, double angle, int rising);
double heb_zmanim_dawn(const heb_zmanim_t *z);
double heb_zmanim_dusk(const heb_zmanim_t *z);
double heb_zmanim_alot_hashachar(const heb_zmanim_t *z);
double heb_zmanim_misheyakir(const heb_zmanim_t *z);
double heb_zmanim_misheyakir_machmir(const heb_zmanim_t *z);
double heb_zmanim_tzeit(const heb_zmanim_t *z, double angle);
double heb_zmanim_bein_hashmashos(const heb_zmanim_t *z);

/* --- Gra zmanim --- */
double heb_zmanim_sof_zman_shma(const heb_zmanim_t *z);
double heb_zmanim_sof_zman_tfilla(const heb_zmanim_t *z);
double heb_zmanim_mincha_gedola(const heb_zmanim_t *z);
double heb_zmanim_mincha_ketana(const heb_zmanim_t *z);
double heb_zmanim_plag_hamincha(const heb_zmanim_t *z);

/* --- MGA zmanim --- */
double heb_zmanim_sof_zman_shma_mga(const heb_zmanim_t *z);
double heb_zmanim_sof_zman_tfilla_mga(const heb_zmanim_t *z);

/* --- Offset times (UTC decimal hours) --- */
/* offset in minutes; round_time rounds to nearest minute */
double heb_zmanim_sunrise_offset(const heb_zmanim_t *z, int offset, int round_time);
double heb_zmanim_sunset_offset(const heb_zmanim_t *z, int offset, int round_time);

/* --- Utility: format UTC decimal hours as local HH:MM --- */
void heb_zmanim_format_time(double utc_hours, int utc_offset, char *buf, size_t n);

/* --- Constants --- */
#define HEB_TZEIT_3_SMALL_STARS  8.5
#define HEB_TZEIT_3_MEDIUM_STARS 7.083

#endif /* HEB_ZMANIM_H */
