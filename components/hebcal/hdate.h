/*
 * hebcal-c - Hebrew date conversions.
 * Ported from github.com/hebcal/hdate (Go).
 *
 * Converts between Hebrew and Gregorian dates using R.D. (Rata Die) day
 * numbers.  Also includes Yahrzeit and birthday/anniversary calculations.
 *
 * License: GPL-2.0-or-later
 */
#ifndef HEB_HDATE_H
#define HEB_HDATE_H

#include "hebcal_types.h"
#include <stddef.h>

/* Hebrew Calendar epoch in R.D. (Rata Die) numbers. */
#define HEB_HDATE_EPOCH ((heb_rd_t)(-1373428))

/* Average Hebrew year length in days (used for year estimation in FromRD). */
#define HEB_HDATE_AVG_YEAR_DAYS 365.24682220597794

/* --- Year-level queries --- */

/* Returns true if Hebrew year is a leap year (13 months). */
int heb_hdate_is_leap_year(int year);

/* Returns the number of months in this Hebrew year (12 or 13). */
int heb_hdate_months_in_year(int year);

/* Returns the number of days in the Hebrew year (353/354/355/383/384/385). */
int heb_hdate_days_in_year(int year);

/* Returns true if Cheshvan is long (30 days) in Hebrew year. */
int heb_hdate_long_cheshvan(int year);

/* Returns true if Kislev is short (29 days) in Hebrew year. */
int heb_hdate_short_kislev(int year);

/* Returns the number of days in a Hebrew month in a given year (29 or 30). */
int heb_hdate_days_in_month(heb_hmonth_t month, int year);

/* --- R.D. conversions --- */

/* Converts Hebrew date to R.D. (Rata Die) fixed days. */
heb_rd_t heb_hdate_to_rd(int year, heb_hmonth_t month, int day);

/* Converts R.D. to Hebrew date.  Panics (undefined) if rataDie <= Epoch. */
heb_hdate_t heb_hdate_from_rd(heb_rd_t rataDie);

/* --- Construction --- */

/* Creates an HDate from year, Hebrew month, and day.
 * Returns 0 on success, nonzero on error (see HEB_HDATE_E_*).
 * On error, the returned HDate fields are zeroed. */
#define HEB_HDATE_E_OK         0
#define HEB_HDATE_E_YEAR       1  /* year < 1 */
#define HEB_HDATE_E_MONTH      2  /* month out of range */
#define HEB_HDATE_E_DAY        3  /* day out of range */

int heb_hdate_create(heb_hdate_t *hd, int year, heb_hmonth_t month, int day);

/* Creates an HDate from Gregorian year, month, day (historical calendar). */
heb_hdate_t heb_hdate_from_gregorian(int year, heb_month_greg_t month, int day);

/* Creates an HDate from a Proleptic Gregorian date. */
heb_hdate_t heb_hdate_from_proleptic_gregorian(int year, heb_month_greg_t month, int day);

/* --- Accessors --- */
#define heb_hdate_year(hd)   ((hd)->year)
#define heb_hdate_month(hd)  ((hd)->month)
#define heb_hdate_day(hd)    ((hd)->day)
#define heb_hdate_abs(hd)    ((hd)->abs)

/* Returns the number of days in this date's month (29 or 30). */
int heb_hdate_days_in_month_hd(const heb_hdate_t *hd);

/* Converts this Hebrew date to Gregorian year, month, day. */
heb_gdate_t heb_hdate_greg(const heb_hdate_t *hd);

/* Converts this Hebrew date to Proleptic Gregorian year, month, day. */
heb_gdate_t heb_hdate_proleptic_greg(const heb_hdate_t *hd);

/* Returns the day of the week (HEB_SUNDAY=0 .. HEB_SATURDAY=6). */
heb_weekday_t heb_hdate_weekday(const heb_hdate_t *hd);

/* Returns the previous Hebrew date. */
heb_hdate_t heb_hdate_prev(const heb_hdate_t *hd);

/* Returns the next Hebrew date. */
heb_hdate_t heb_hdate_next(const heb_hdate_t *hd);

/* Returns true if this HDate occurs during a Hebrew leap year. */
int heb_hdate_is_leap_year_hd(const heb_hdate_t *hd);

/* --- String rendering --- */

/* Locale codes for month name rendering. */
#define HEB_LOCALE_EN           "en"
#define HEB_LOCALE_HE           "he"
#define HEB_LOCALE_HE_NONIKUD   "he-x-NoNikud"

/* Writes the month name for the given locale into buf (n bytes).
 * Returns the number of bytes written (not including NUL). */
int heb_hdate_month_name(const heb_hdate_t *hd, const char *locale, char *buf, size_t n);

/* Writes a string representation (e.g. "15 Cheshvan 5769") into buf.
 * Returns the number of bytes written (not including NUL). */
int heb_hdate_to_string(const heb_hdate_t *hd, char *buf, size_t n);

/* --- Month name parsing --- */

/* Parses a Hebrew month name string to a month number.
 * Returns 0 on success (sets *month), nonzero on failure. */
int heb_hdate_month_from_name(const char *name, heb_hmonth_t *month);

/* --- Day-of-week utilities --- */

/* Returns the R.D. of the given dayOfWeek on or before rataDie. */
heb_rd_t heb_hdate_day_on_or_before(heb_weekday_t dayOfWeek, heb_rd_t rataDie);

/* Returns the HDate for dayOfWeek before hd. */
heb_hdate_t heb_hdate_before(const heb_hdate_t *hd, heb_weekday_t dayOfWeek);

/* Returns the HDate for dayOfWeek on or before hd. */
heb_hdate_t heb_hdate_on_or_before(const heb_hdate_t *hd, heb_weekday_t dayOfWeek);

/* Returns the HDate for the nearest dayOfWeek to hd. */
heb_hdate_t heb_hdate_nearest(const heb_hdate_t *hd, heb_weekday_t dayOfWeek);

/* Returns the HDate for dayOfWeek on or after hd. */
heb_hdate_t heb_hdate_on_or_after(const heb_hdate_t *hd, heb_weekday_t dayOfWeek);

/* Returns the HDate for dayOfWeek after hd. */
heb_hdate_t heb_hdate_after(const heb_hdate_t *hd, heb_weekday_t dayOfWeek);

/* --- Hebrew nikud utilities --- */

/* Removes niqqud (vowel points) and cantillation marks from a UTF-8 string.
 * Writes the result into dst (n bytes).  Returns bytes written (excl. NUL). */
int heb_hebrew_strip_nikkud(const char *src, char *dst, size_t n);

#endif /* HEB_HDATE_H */
