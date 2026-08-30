/*
 * hebcal-c - HebrewCalendar orchestrator.
 * Ported from github.com/hebcal/hebcal-go/hebcal/hebcal.go (Go).
 *
 * Generates a calendar of events for a given year range, combining
 * holidays, Hebrew dates, Omer, Sedra, and candle-lighting times.
 *
 * License: GPL-2.0-or-later
 */
#ifndef HEB_HEBCAL_H
#define HEB_HEBCAL_H

#include "hdate.h"
#include "event.h"
#include "zmanim.h"
#include <stddef.h>

/* Calendar generation options. */
typedef struct {
    int year;              /* Gregorian year (default: current year) */
    int is_hebrew_year;    /* 1 = treat year as Hebrew year */
    int num_years;         /* generate N years (default 1) */
    int il;                /* 1 = Israel schedule */
    int no_holidays;       /* 1 = suppress holidays */
    int no_rosh_chodesh;   /* 1 = suppress Rosh Chodesh */
    int no_minor_fast;     /* 1 = suppress minor fasts */
    int no_special_shabbat;/* 1 = suppress special Shabbatot */
    int no_modern;         /* 1 = suppress modern holidays */
    int sedrot;            /* 1 = include weekly Torah readings */
    int omer;              /* 1 = include Omer counting */
    int molad;             /* 1 = include molad announcements */
    int add_hebrew_dates;  /* 1 = print Hebrew date for all days */
    int candle_lighting;   /* 1 = include candle-lighting times */
    int suppress_havdalah; /* 1 = suppress havdalah times */
    int candle_lighting_mins; /* minutes before sunset (default 18) */
    int havdalah_mins;     /* minutes after sunset for havdalah (0 = use degrees) */
    double havdalah_deg;   /* solar depression for havdalah (default 8.5) */
    int use_elevation;     /* 1 = use elevation for sunrise/sunset */

    /* Location for zmanim (set candle_lighting=1 to use) */
    double latitude;
    double longitude;
    double elevation;
    int utc_offset;        /* minutes from UTC */
} heb_cal_options_t;

/* Initializes options with defaults. */
heb_cal_options_t heb_cal_default_options(void);

/* Generates a calendar for the given options.
 * Writes up to max_events events into the events array.
 * Returns the number of events, or -1 on error. */
int heb_hebrew_calendar(const heb_cal_options_t *opts,
                         heb_event_t *events, int max_events);

#endif /* HEB_HEBCAL_H */
