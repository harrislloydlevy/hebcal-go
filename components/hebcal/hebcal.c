/*
 * hebcal-c - HebrewCalendar orchestrator.
 * Ported from github.com/hebcal/hebcal-go/hebcal/hebcal.go (Go).
 *
 * License: GPL-2.0-or-later
 */
#include "hebcal.h"
#include "holidays.h"
#include "sedra.h"
#include "omer.h"
#include "molad.h"
#include "locale.h"
#include "greg.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

#ifndef NAN
#define NAN (0.0/0.0)
#endif

/* Forward declaration */
static int compare_events_helper(const heb_event_t *a, const heb_event_t *b);

heb_cal_options_t heb_cal_default_options(void) {
    heb_cal_options_t opts;
    memset(&opts, 0, sizeof(opts));
    opts.year = 0;        /* caller sets year */
    opts.num_years = 1;
    opts.candle_lighting_mins = 18;
    opts.havdalah_deg = 8.5;
    opts.havdalah_mins = 0; /* 0 = use degrees */
    return opts;
}

/* Helper: format a candle-lighting or havdalah time string */
static void format_zman_time(const heb_zmanim_t *z, double utc_hours,
                              char *buf, size_t n) {
    if (isnan(utc_hours)) {
        buf[0] = '\0';
        return;
    }
    char timeBuf[16];
    heb_zmanim_format_time(utc_hours, z->utc_offset, timeBuf, sizeof(timeBuf));
    snprintf(buf, n, "%s", timeBuf);
}

/* Helper: check if a date is a Friday or Saturday (for candle lighting/havdalah) */
static int is_candle_lighting_day(const heb_hdate_t *hd, int il) {
    heb_weekday_t dow = heb_hdate_weekday(hd);
    return dow == HEB_FRIDAY;
}

static int is_havdalah_day(const heb_hdate_t *hd, int il) {
    heb_weekday_t dow = heb_hdate_weekday(hd);
    return dow == HEB_SATURDAY;
}

/* Helper: should we emit candle lighting for this event? */
static int event_has_candle_lighting(const heb_event_t *ev) {
    return (ev->flags & (HEB_F_LIGHT_CANDLES | HEB_F_LIGHT_CANDLES_TZEIS)) != 0;
}

static int event_has_havdalah(const heb_event_t *ev) {
    return (ev->flags & (HEB_F_YOM_TOV_ENDS | HEB_F_LIGHT_CANDLES_TZEIS)) != 0;
}

int heb_hebrew_calendar(const heb_cal_options_t *opts,
                         heb_event_t *events, int max_events) {
    int count = 0;

    /* Determine year range */
    int gregYear = opts->year;
    if (opts->is_hebrew_year) {
        /* Convert Hebrew year to Gregorian year range */
        /* Hebrew year starts in Tishrei, which is around September */
        heb_hdate_t rh;
        heb_hdate_create(&rh, gregYear, HEB_H_TISHREI, 1);
        heb_gdate_t gd = heb_hdate_greg(&rh);
        gregYear = gd.year;
    }

    for (int yr = 0; yr < opts->num_years; yr++) {
        int currentYear = gregYear + yr;

        /* Determine the Hebrew year(s) that overlap this Gregorian year */
        /* Jan 1 of currentYear is in the middle of a Hebrew year */
        heb_hdate_t jan1 = heb_hdate_from_gregorian(currentYear, HEB_JANUARY, 1);
        heb_hdate_t dec31 = heb_hdate_from_gregorian(currentYear, HEB_DECEMBER, 31);

        int hYearStart = jan1.year;
        int hYearEnd = dec31.year;

        /* Collect holidays for each Hebrew year that overlaps */
        static heb_event_t yearEvents[2][HEB_MAX_EVENTS];
        int yearEventCount[2];
        int numHYears = 0;

        for (int hy = hYearStart; hy <= hYearEnd && numHYears < 2; hy++) {
            yearEventCount[numHYears] = heb_holidays_for_year(
                hy, opts->il, yearEvents[numHYears], HEB_MAX_EVENTS);
            numHYears++;
        }

        /* Add holiday events that fall within this Gregorian year */
        for (int yi = 0; yi < numHYears; yi++) {
            for (int i = 0; i < yearEventCount[yi]; i++) {
                heb_gdate_t gd = heb_hdate_greg(&yearEvents[yi][i].date);
                if (gd.year == currentYear) {
                    /* Apply filters */
                    if (opts->no_holidays) continue;
                    if (opts->no_rosh_chodesh && (yearEvents[yi][i].flags & HEB_F_ROSH_CHODESH))
                        continue;
                    if (opts->no_minor_fast && (yearEvents[yi][i].flags & HEB_F_MINOR_FAST))
                        continue;
                    if (opts->no_special_shabbat && (yearEvents[yi][i].flags & HEB_F_SPECIAL_SHABBAT))
                        continue;
                    if (opts->no_modern && (yearEvents[yi][i].flags & HEB_F_MODERN_HOLIDAY))
                        continue;

                    if (count < max_events) {
                        events[count] = yearEvents[yi][i];
                        count++;
                    }
                }
            }
        }

        /* Add Omer events (Nisan 16 through Sivan 5) */
        if (opts->omer) {
            for (int hy = hYearStart; hy <= hYearEnd; hy++) {
                /* Omer starts on 16 Nisan (2nd night of Pesach) */
                heb_hdate_t omerStart;
                heb_hdate_create(&omerStart, hy, HEB_H_NISAN, 16);
                heb_hdate_t omerEnd;
                heb_hdate_create(&omerEnd, hy, HEB_H_SIVAN, 5);

                heb_hdate_t hd = omerStart;
                int omerDay = 1;
                while (hd.abs <= omerEnd.abs) {
                    heb_gdate_t gd = heb_hdate_greg(&hd);
                    if (gd.year == currentYear && count < max_events) {
                        heb_omer_t omer = heb_omer_new(hd, omerDay);
                        char desc[64];
                        heb_omer_render(&omer, "en", desc, sizeof(desc));
                        events[count].date = hd;
                        events[count].desc = NULL; /* dynamic desc - store in render */
                        events[count].flags = HEB_F_OMER_COUNT;
                        events[count].emoji = NULL;
                        events[count].chol_hamoed_day = 0;
                        events[count].chanukah_day = 0;
                        /* Store the rendered description in a static buffer */
                        /* For simplicity, we store the desc pointer from omer_render */
                        /* This is safe because omer_render writes to a local buffer */
                        /* In a real app, you'd copy this to a persistent buffer */
                        static char omerDescs[49][64];
                        if (omerDay >= 1 && omerDay <= 49) {
                            heb_omer_render(&omer, "en", omerDescs[omerDay-1], sizeof(omerDescs[0]));
                            events[count].desc = omerDescs[omerDay-1];
                        }
                        count++;
                    }
                    hd = heb_hdate_next(&hd);
                    omerDay++;
                }
            }
        }

        /* Add Sedra (weekly Torah readings on Saturdays) */
        if (opts->sedrot) {
            for (int hy = hYearStart; hy <= hYearEnd; hy++) {
                heb_sedra_t sedra = heb_sedra_new(hy, opts->il);
                heb_hdate_t sat = heb_hdate_from_rd(sedra.first_saturday);
                /* Iterate Saturdays through the Hebrew year */
                for (int week = 0; week < sedra.array_len; week++) {
                    heb_gdate_t gd = heb_hdate_greg(&sat);
                    if (gd.year == currentYear && count < max_events) {
                        heb_parsha_t p = heb_sedra_lookup_rd(&sedra, sat.abs);
                        if (!p.chag) {
                            char desc[128];
                            heb_parsha_render(&p, "en", desc, sizeof(desc));
                            static char sedraDescs[100][128];
                            if (week < 100) {
                                snprintf(sedraDescs[week], sizeof(sedraDescs[0]),
                                         "Parashat %s", desc);
                                events[count].date = sat;
                                events[count].desc = sedraDescs[week];
                                events[count].flags = HEB_F_PARSHA_HASHAVUA;
                                events[count].emoji = NULL;
                                events[count].chol_hamoed_day = 0;
                                events[count].chanukah_day = 0;
                                count++;
                            }
                        }
                    }
                    sat.abs += 7;
                    /* Update sat's Hebrew date */
                    sat = heb_hdate_from_rd(sat.abs);
                }
            }
        }

        /* Add Hebrew dates for every day */
        if (opts->add_hebrew_dates) {
            int dayOfYear = 1;
            int daysInYear = heb_greg_is_leap_year(currentYear) ? 366 : 365;
            for (int d = 0; d < daysInYear && count < max_events; d++) {
                heb_hdate_t hd = heb_hdate_from_gregorian(
                    currentYear, HEB_JANUARY, 1);
                hd.abs += d;
                hd = heb_hdate_from_rd(hd.abs);
                char desc[64];
                heb_hdate_to_string(&hd, desc, sizeof(desc));
                static char hdDescs[366][64];
                if (d < 366) {
                    strcpy(hdDescs[d], desc);
                    events[count].date = hd;
                    events[count].desc = hdDescs[d];
                    events[count].flags = HEB_F_HEBREW_DATE;
                    events[count].emoji = NULL;
                    events[count].chol_hamoed_day = 0;
                    events[count].chanukah_day = 0;
                    count++;
                }
            }
        }

        /* Add candle-lighting and havdalah times */
        if (opts->candle_lighting) {
            /* Iterate through holiday events and add candle lighting/havdalah */
            for (int i = 0; i < count; i++) {
                heb_gdate_t gd = heb_hdate_greg(&events[i].date);
                if (gd.year != currentYear) continue;

                heb_zmanim_t z = heb_zmanim_new(
                    opts->latitude, opts->longitude, opts->elevation,
                    gd.year, gd.month, gd.day, opts->utc_offset);
                z.use_elevation = opts->use_elevation;

                heb_weekday_t dow = heb_hdate_weekday(&events[i].date);

                if (event_has_candle_lighting(&events[i])) {
                    double sunset = heb_zmanim_sunset(&z);
                    if (!isnan(sunset)) {
                        double cl = sunset - (double)opts->candle_lighting_mins / 60.0;
                        /* Round to nearest minute */
                        cl = floor(cl * 60.0 + 0.5) / 60.0;
                        char timeBuf[16];
                        format_zman_time(&z, cl, timeBuf, sizeof(timeBuf));
                        /* Append time to description */
                        static char clDescs[100][128];
                        static int clIdx = 0;
                        if (clIdx < 100) {
                            snprintf(clDescs[clIdx], sizeof(clDescs[0]),
                                     "%s: %s", events[i].desc, timeBuf);
                            events[i].desc = clDescs[clIdx];
                            clIdx++;
                        }
                    }
                }

                if (event_has_havdalah(&events[i]) && !opts->suppress_havdalah) {
                    double havdalah;
                    if (opts->havdalah_mins > 0) {
                        double sunset = heb_zmanim_sunset(&z);
                        havdalah = sunset + (double)opts->havdalah_mins / 60.0;
                    } else {
                        havdalah = heb_zmanim_tzeit(&z, opts->havdalah_deg);
                    }
                    if (!isnan(havdalah)) {
                        /* Round to nearest minute */
                        havdalah = floor(havdalah * 60.0 + 0.5) / 60.0;
                        char timeBuf[16];
                        format_zman_time(&z, havdalah, timeBuf, sizeof(timeBuf));
                        static char hvDescs[100][128];
                        static int hvIdx = 0;
                        if (hvIdx < 100) {
                            snprintf(hvDescs[hvIdx], sizeof(hvDescs[0]),
                                     "%s: %s", events[i].desc, timeBuf);
                            events[i].desc = hvDescs[hvIdx];
                            hvIdx++;
                        }
                    }
                }
            }
        }
    }

    /* Sort by date */
    /* Simple insertion sort (qsort is already used in holidays, but we need
     * to sort the combined list) */
    for (int i = 1; i < count; i++) {
        heb_event_t tmp = events[i];
        int j = i - 1;
        while (j >= 0 && compare_events_helper(&events[j], &tmp) > 0) {
            events[j + 1] = events[j];
            j--;
        }
        events[j + 1] = tmp;
    }

    return count;
}

/* Comparison helper (defined here since qsort comparator is in holidays.c) */
static int compare_events_helper(const heb_event_t *a, const heb_event_t *b) {
    if (a->date.abs != b->date.abs)
        return (a->date.abs < b->date.abs) ? -1 : 1;
    /* Primary events (holidays) before secondary (omer, sedra, hebdate) */
    int priA = (a->flags & (HEB_F_OMER_COUNT | HEB_F_PARSHA_HASHAVUA | HEB_F_HEBREW_DATE)) ? 1 : 0;
    int priB = (b->flags & (HEB_F_OMER_COUNT | HEB_F_PARSHA_HASHAVUA | HEB_F_HEBREW_DATE)) ? 1 : 0;
    if (priA != priB) return priA - priB;
    if (a->desc && b->desc)
        return strcmp(a->desc, b->desc);
    return 0;
}
