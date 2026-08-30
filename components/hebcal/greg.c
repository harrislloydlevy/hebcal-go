/*
 * hebcal-c - Gregorian / Julian calendar conversions.
 * Ported from github.com/hebcal/greg (Go).
 *
 * License: GPL-2.0-or-later
 */
#include "greg.h"
#include <stddef.h>

/* 1-based month lengths in a non-leap year.  Index 0 unused. */
static const int month_len[13] = {
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

int heb_greg_days_in(heb_month_greg_t m, int year) {
    if (m == HEB_FEBRUARY && heb_greg_is_leap_year(year)) {
        return 29;
    }
    return month_len[m];
}

int heb_greg_is_leap_year(int year) {
    /* Adjust and pretend 0 was a valid year to simplify the math. */
    if (year < 0) {
        year++;
    }
    if (year % 400 == 0) {
        return 1;
    }
    if (year > HEB_GREG_ADJ_BEGIN_YEAR && year % 100 == 0) {
        return 0;
    }
    return (year % 4 == 0);
}

/* floor division: quotient of x/y rounded toward -inf. */
static int64_t quotient(int64_t x, int64_t y) {
    int64_t q = x / y;
    if (x % y != 0 && ((x < 0) != (y < 0))) {
        q--;
    }
    return q;
}

/* floor mod: x mod y, taking the sign of y. */
static int64_t floormod(int64_t x, int64_t y) {
    int64_t m = x % y;
    if (m != 0 && ((m < 0) != (y < 0))) {
        m += y;
    }
    return m;
}

/* divmod returning both floor-quotient and floor-remainder via pointers. */
static void divmod(int64_t x, int64_t y, int64_t *q, int64_t *r) {
    *q = x / y;
    *r = x % y;
    if (*r != 0 && ((*r < 0) != (y < 0))) {
        (*q)--;
        (*r) += y;
    }
}

static int64_t greg_month_offset(int year, heb_month_greg_t month) {
    if (month <= HEB_FEBRUARY) {
        return 0;
    }
    if (heb_greg_is_leap_year(year)) {
        return -1;
    }
    return -2;
}

/* Reports whether the date falls after the Julian->Gregorian switch. */
static int is_gregorian(int year, heb_month_greg_t month, int day) {
    if (year != HEB_GREG_ADJ_BEGIN_YEAR) {
        return year > HEB_GREG_ADJ_BEGIN_YEAR;
    }
    if (month != HEB_GREG_ADJ_BEGIN_MONTH) {
        return month > HEB_GREG_ADJ_BEGIN_MONTH;
    }
    return day > HEB_GREG_ADJ_BEGIN_DAY;
}

heb_rd_t heb_greg_to_rd(int year, heb_month_greg_t month, int day, int *err) {
    if (err) *err = 0;

    if (year == 0) {
        if (err) *err = 1; /* "There was no year 0" */
        return -1;
    }
    if (year >= HEB_GREG_ADJ_BEGIN_YEAR && year <= HEB_GREG_ADJ_END_YEAR &&
        month >= HEB_GREG_ADJ_BEGIN_MONTH && month <= HEB_GREG_ADJ_END_MONTH &&
        day > HEB_GREG_ADJ_BEGIN_DAY && day < HEB_GREG_ADJ_END_DAY) {
        if (err) *err = 2; /* date in the 1752 gap */
        return -1;
    }

    /* Days up to the preceding year.  No year 0, so positive years step
     * down by one; negative years are already "off by one". */
    int64_t py = (int64_t)year;
    if (year > 0) {
        py--;
    }

    int64_t abs =
        365 * py +
        quotient(py, 4) +
        quotient(367 * (int64_t)month - 362, 12) +
        greg_month_offset(year, month) +
        (int64_t)day;

    if (is_gregorian(year, month, day)) {
        abs -= quotient(py, 100);
        abs += quotient(py, 400);
    } else {
        /* Julian dates need 2-day adjustment relative to Gregorian epoch. */
        abs -= 2;
    }
    return abs;
}

/* yearFromFixed: finds the year and day-within-year for a given R.D. */
static void year_from_fixed(heb_rd_t rataDie, int *year_out, int64_t *day_out) {
    int64_t l0 = rataDie - 1;
    int64_t n400, n100, d1, d2, n4, d3, n1, day;

    if (rataDie > HEB_GREG_ADJ_BEGIN_RD) {
        divmod(l0, 146097, &n400, &d1);
        divmod(d1, 36524, &n100, &d2);
    } else {
        l0 += 2;
        divmod(l0, 146100, &n400, &d1);
        divmod(d1, 36525, &n100, &d2);
    }
    divmod(d2, 1461, &n4, &d3);
    n1 = quotient(d3, 365);
    day = d3 - 365 * n1;

    int year = (int)(400 * n400 + 100 * n100 + 4 * n4 + n1);

    if (year < 0) {
        year--;
    }

    if (n100 == 4 || n1 == 4) {
        if (year == 0) {
            *year_out = -1;
            *day_out = 365;
            return;
        }
        *year_out = year;
        *day_out = 365;
        return;
    }
    *year_out = year + 1;
    *day_out = day;
}

heb_gdate_t heb_greg_from_rd(heb_rd_t rataDie) {
    heb_gdate_t result;
    int64_t day;
    year_from_fixed(rataDie, &result.year, &day);
    day++; /* 1-based day of year */

    int leap = heb_greg_is_leap_year(result.year);
    heb_month_greg_t month;
    for (month = HEB_JANUARY; month <= HEB_DECEMBER; month = (heb_month_greg_t)(month + 1)) {
        int64_t mlen = (int64_t)month_len[month];
        if (leap && month == HEB_FEBRUARY) {
            mlen++;
        }
        if (mlen >= day) {
            break;
        }
        day -= mlen;
    }
    result.month = month;
    result.day = (int)day;
    return result;
}

/* === Proleptic Gregorian === */

int heb_greg_proleptic_is_leap_year(int year) {
    int64_t gyear = (int64_t)year;
    if (floormod(gyear, 4) == 0) {
        int64_t n = floormod(gyear, 400);
        if (n != 100 && n != 200 && n != 300) {
            return 1;
        }
    }
    return 0;
}

static int64_t proleptic_month_offset(int year, heb_month_greg_t month) {
    if (month <= HEB_FEBRUARY) {
        return 0;
    }
    if (heb_greg_proleptic_is_leap_year(year)) {
        return -1;
    }
    return -2;
}

heb_rd_t heb_greg_proleptic_to_rd(int year, heb_month_greg_t month, int day) {
    int64_t py = (int64_t)year - 1;
    return 365 * py +
           quotient(py, 4) -
           quotient(py, 100) +
           quotient(py, 400) +
           quotient(367 * (int64_t)month - 362, 12) +
           proleptic_month_offset(year, month) +
           (int64_t)day;
}

static int proleptic_year_from_fixed(heb_rd_t rataDie) {
    int64_t l0 = rataDie - 1;
    int64_t n400, d1, n100, d2, n4, d3, n1;
    divmod(l0, 146097, &n400, &d1);
    divmod(d1, 36524, &n100, &d2);
    divmod(d2, 1461, &n4, &d3);
    n1 = quotient(d3, 365);
    int year = (int)(400 * n400 + 100 * n100 + 4 * n4 + n1);
    if (n100 == 4 || n1 == 4) {
        return year;
    }
    return year + 1;
}

heb_gdate_t heb_greg_proleptic_from_rd(heb_rd_t rataDie) {
    heb_gdate_t result;
    int year = proleptic_year_from_fixed(rataDie);
    result.year = year;

    heb_rd_t jan1 = heb_greg_proleptic_to_rd(year, HEB_JANUARY, 1);

    int64_t correction;
    if (rataDie < heb_greg_proleptic_to_rd(year, HEB_MARCH, 1)) {
        correction = 0;
    } else if (heb_greg_proleptic_is_leap_year(year)) {
        correction = 1;
    } else {
        correction = 2;
    }

    int64_t prior_days = rataDie - jan1;
    int m = (int)quotient(12 * (prior_days + correction) + 373, 367);
    result.month = (heb_month_greg_t)m;
    result.day = (int)(rataDie - heb_greg_proleptic_to_rd(year, result.month, 1) + 1);
    return result;
}
