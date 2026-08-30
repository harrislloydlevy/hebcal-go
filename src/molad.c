/*
 * hebcal-c - Molad (New Moon) calculation.
 * Ported from github.com/hebcal/hebcal-go/molad (Go).
 *
 * License: GPL-2.0-or-later
 */
#include "molad.h"
#include <string.h>

/* Days from beginning of Sunday till molad BaHaRaD.
 * 1 day, 5 hours, 204 chalakim = (24+5)*1080 + 204 = 31524 */
#define CHALAKIM_MOLAD_TOHU    31524LL
#define CHALAKIM_PER_MINUTE    18
#define CHALAKIM_PER_HOUR      1080
#define CHALAKIM_PER_DAY       25920LL  /* 24 * 1080 */
#define CHALAKIM_PER_MONTH     765433LL /* (29*24+12)*1080 + 793 */

static int get_jewish_month_of_year(int year, heb_hmonth_t month) {
    int offset = heb_hdate_is_leap_year(year) ? 1 : 0;
    return ((int)month + (offset + 5)) % (offset + 12) + 1;
}

static int64_t get_chalakim_since_molad_tohu(int year, heb_hmonth_t month) {
    int monthOfYear = get_jewish_month_of_year(year, month);
    int64_t prevYear = (int64_t)year - 1;
    int64_t monthsElapsed =
        235 * (prevYear / 19) +
        12 * (prevYear % 19) +
        ((7 * (prevYear % 19) + 1) / 19) +
        (int64_t)(monthOfYear - 1);
    return CHALAKIM_MOLAD_TOHU + (CHALAKIM_PER_MONTH * monthsElapsed);
}

static int64_t molad_to_abs_date(int64_t chalakim) {
    return (chalakim / CHALAKIM_PER_DAY) + HEB_HDATE_EPOCH - 1;
}

static void set_molad_time(heb_molad_t *m, int chalakim) {
    m->hours = chalakim / CHALAKIM_PER_HOUR;
    chalakim = chalakim - (m->hours * CHALAKIM_PER_HOUR);
    m->minutes = chalakim / CHALAKIM_PER_MINUTE;
    m->chalakim = chalakim - m->minutes * CHALAKIM_PER_MINUTE;
}

heb_molad_t heb_molad_new(int year, heb_hmonth_t month) {
    heb_molad_t m;
    memset(&m, 0, sizeof(m));

    int64_t chalakim = get_chalakim_since_molad_tohu(year, month);
    m.date = heb_hdate_from_rd(molad_to_abs_date(chalakim));

    int64_t conjunctionDay = chalakim / CHALAKIM_PER_DAY;
    int64_t conjunctionParts = chalakim - conjunctionDay * CHALAKIM_PER_DAY;
    set_molad_time(&m, (int)conjunctionParts);

    if (m.hours >= 6) {
        m.date = heb_hdate_next(&m.date);
    }
    m.hours = (m.hours + 18) % 24;
    return m;
}
