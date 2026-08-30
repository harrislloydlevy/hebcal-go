/*
 * hebcal-c - Gregorian / Julian calendar conversions.
 * Ported from github.com/hebcal/greg (Go).
 *
 * Converts between Gregorian/Julian dates and R.D. (Rata Die) day numbers.
 * Uses the historical Julian->Gregorian switch of September 1752 (British
 * National Act), plus a Proleptic Gregorian variant with no gap.
 *
 * License: GPL-2.0-or-later
 */
#ifndef HEB_GREG_H
#define HEB_GREG_H

#include "hebcal_types.h"

/* The Julian-to-Gregorian transition dates (September 1752).  Days between
 * Sep 2 and Sep 14 (exclusive) never existed. */
#define HEB_GREG_ADJ_BEGIN_YEAR   1752
#define HEB_GREG_ADJ_BEGIN_MONTH  HEB_SEPTEMBER
#define HEB_GREG_ADJ_BEGIN_DAY    2
#define HEB_GREG_ADJ_END_YEAR     1752
#define HEB_GREG_ADJ_END_MONTH    HEB_SEPTEMBER
#define HEB_GREG_ADJ_END_DAY      14

/* R.D. of the last Julian day (Sep 2, 1752).  Dates at or below this are
 * Julian; dates above are Gregorian. */
#define HEB_GREG_ADJ_BEGIN_RD     639796

/* Returns the number of days in the Gregorian/Julian month. */
int heb_greg_days_in(heb_month_greg_t m, int year);

/* Returns true if year is a leap year.
 * Years before the Gregorian switch follow the Julian rule (every 4th). */
int heb_greg_is_leap_year(int year);

/* Converts a Gregorian/Julian date to R.D.  Returns -1 and sets *err on
 * invalid input (year 0, or date in the 1752 gap).  On success *err = 0. */
heb_rd_t heb_greg_to_rd(int year, heb_month_greg_t month, int day, int *err);

/* Converts an R.D. to a Gregorian/Julian date. */
heb_gdate_t heb_greg_from_rd(heb_rd_t rataDie);

/* --- Proleptic Gregorian (no Julian switch, no 1752 gap) --- */

int heb_greg_proleptic_is_leap_year(int year);
heb_rd_t heb_greg_proleptic_to_rd(int year, heb_month_greg_t month, int day);
heb_gdate_t heb_greg_proleptic_from_rd(heb_rd_t rataDie);

#endif /* HEB_GREG_H */
