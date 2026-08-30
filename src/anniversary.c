/*
 * hebcal-c - Yahrzeit and birthday/anniversary calculations.
 * Ported from github.com/hebcal/hdate/anniversary.go (Go).
 *
 * License: GPL-2.0-or-later
 */
#include "anniversary.h"
#include <string.h>

int heb_anniversary_yahrzeit(int hyear, const heb_hdate_t *date, heb_hdate_t *result) {
    memset(result, 0, sizeof(*result));
    if (hyear <= date->year) {
        return 1; /* year occurs on or before original date */
    }

    heb_hmonth_t month = date->month;
    int day = date->day;

    if (month == HEB_H_CHESHVAN && day == 30 && !heb_hdate_long_cheshvan(date->year + 1)) {
        /* Heshvan 30: if first anniversary was not Heshvan 30, use day before Kislev 1 */
        heb_hdate_t hd = heb_hdate_from_rd(heb_hdate_to_rd(hyear, HEB_H_KISLEV, 1) - 1);
        month = hd.month;
        day = hd.day;
    } else if (month == HEB_H_KISLEV && day == 30 && heb_hdate_short_kislev(date->year + 1)) {
        /* Kislev 30: if first anniversary was not Kislev 30, use day before Tevet 1 */
        heb_hdate_t hd = heb_hdate_from_rd(heb_hdate_to_rd(hyear, HEB_H_TEVET, 1) - 1);
        month = hd.month;
        day = hd.day;
    } else if (month == HEB_H_ADAR2) {
        /* Adar II: use same day in last month of year (Adar or Adar II) */
        month = (heb_hmonth_t)heb_hdate_months_in_year(hyear);
    } else if (month == HEB_H_ADAR1 && day == 30 && !heb_hdate_is_leap_year(hyear)) {
        /* Adar I 30 in non-leap year: use last day in Shevat */
        month = HEB_H_SHVAT;
        day = 30;
    }

    /* Advance day to rosh chodesh if needed */
    if (month == HEB_H_CHESHVAN && day == 30 && !heb_hdate_long_cheshvan(hyear)) {
        month = HEB_H_KISLEV;
        day = 1;
    } else if (month == HEB_H_KISLEV && day == 30 && heb_hdate_short_kislev(hyear)) {
        month = HEB_H_TEVET;
        day = 1;
    }

    return heb_hdate_create(result, hyear, month, day);
}

int heb_anniversary_birthday(int hyear, const heb_hdate_t *date, heb_hdate_t *result) {
    memset(result, 0, sizeof(*result));
    int origYear = date->year;

    if (hyear == origYear) {
        *result = *date;
        return 0;
    }
    if (hyear < origYear) {
        return 1; /* year occurs before original date */
    }

    int isOrigLeap = heb_hdate_is_leap_year(origYear);
    heb_hmonth_t month = date->month;
    int day = date->day;

    if ((month == HEB_H_ADAR1 && !isOrigLeap) || (month == HEB_H_ADAR2 && isOrigLeap)) {
        month = (heb_hmonth_t)heb_hdate_months_in_year(hyear);
    } else if (month == HEB_H_CHESHVAN && day == 30 && !heb_hdate_long_cheshvan(hyear)) {
        month = HEB_H_KISLEV;
        day = 1;
    } else if (month == HEB_H_KISLEV && day == 30 && heb_hdate_short_kislev(hyear)) {
        month = HEB_H_TEVET;
        day = 1;
    } else if (month == HEB_H_ADAR1 && day == 30 && isOrigLeap && !heb_hdate_is_leap_year(hyear)) {
        month = HEB_H_NISAN;
        day = 1;
    }

    return heb_hdate_create(result, hyear, month, day);
}
