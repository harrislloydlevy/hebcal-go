/*
 * hebcal-c - A Jewish Calendar Generator in C
 * Ported from github.com/hebcal/hebcal-go (Go) by Michael J. Radwin
 * and the original C version by Danny Sadinoff.
 *
 * Core type definitions shared by all hebcal-c modules.
 *
 * License: GPL-2.0-or-later (see LICENSE)
 */
#ifndef HEBCAL_TYPES_H
#define HEBCAL_TYPES_H

#include <stdint.h>

/* Rata Die (R.D.) day numbers are signed 64-bit to match the Go reference
 * exactly and to avoid overflow for any realistic year.  R.D. 1 is the
 * imaginary date Monday, January 1, 1 CE on the Gregorian calendar. */
typedef int64_t heb_rd_t;

/* Gregorian / civil months (1-based, matching Go's time.Month order). */
typedef enum {
    HEB_JANUARY   = 1,
    HEB_FEBRUARY  = 2,
    HEB_MARCH     = 3,
    HEB_APRIL     = 4,
    HEB_MAY       = 5,
    HEB_JUNE      = 6,
    HEB_JULY      = 7,
    HEB_AUGUST    = 8,
    HEB_SEPTEMBER = 9,
    HEB_OCTOBER   = 10,
    HEB_NOVEMBER  = 11,
    HEB_DECEMBER  = 12
} heb_month_greg_t;

/* Days of the week (matching Go's time.Weekday).  Sunday = 0. */
typedef enum {
    HEB_SUNDAY    = 0,
    HEB_MONDAY    = 1,
    HEB_TUESDAY   = 2,
    HEB_WEDNESDAY = 3,
    HEB_THURSDAY  = 4,
    HEB_FRIDAY    = 5,
    HEB_SATURDAY  = 6
} heb_weekday_t;

/* A Gregorian (civil) date. */
typedef struct {
    int year;               /* Gregorian year (may be negative for BCE) */
    heb_month_greg_t month; /* 1-12 */
    int day;                /* 1-31 */
} heb_gdate_t;

/* Hebrew months (1-based, matching Go's hdate.HMonth order).
 * Nisan = 1 ... Adar II = 13.  The year starts in Tishrei (7). */
typedef enum {
    HEB_H_NISAN   = 1,
    HEB_H_IYYAR   = 2,
    HEB_H_SIVAN   = 3,
    HEB_H_TAMUZ   = 4,
    HEB_H_AV      = 5,
    HEB_H_ELUL    = 6,
    HEB_H_TISHREI = 7,
    HEB_H_CHESHVAN= 8,
    HEB_H_KISLEV  = 9,
    HEB_H_TEVET   = 10,
    HEB_H_SHVAT   = 11,
    HEB_H_ADAR1   = 12,
    HEB_H_ADAR2   = 13
} heb_hmonth_t;

/* A Hebrew date.  The abs field (R.D.) is always kept in sync. */
typedef struct {
    int year;            /* Hebrew year (>= 1) */
    heb_hmonth_t month;  /* HEB_H_NISAN .. HEB_H_ADAR2 */
    int day;             /* 1-30 */
    heb_rd_t abs;        /* Rata Die */
} heb_hdate_t;

/* Convenience: a civil date + time-of-day in UTC minutes + UTC offset. */
typedef struct {
    int year;
    heb_month_greg_t month;
    int day;
    int hour;      /* 0-23 UTC */
    int minute;    /* 0-59 UTC */
    int second;    /* 0-59 UTC */
    int utc_offset; /* offset from UTC in minutes (east positive) */
} heb_datetime_t;

#endif /* HEBCAL_TYPES_H */
