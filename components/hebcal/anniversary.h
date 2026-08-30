/*
 * hebcal-c - Yahrzeit and birthday/anniversary calculations.
 * Ported from github.com/hebcal/hdate/anniversary.go (Go).
 *
 * License: GPL-2.0-or-later
 */
#ifndef HEB_ANNIVERSARY_H
#define HEB_ANNIVERSARY_H

#include "hdate.h"

/* Calculates yahrzeit date for the given Hebrew year.
 * hyear must be after the original date of death.
 * Returns 0 on success (result in *result), nonzero on error. */
int heb_anniversary_yahrzeit(int hyear, const heb_hdate_t *date, heb_hdate_t *result);

/* Calculates birthday or anniversary date for the given Hebrew year.
 * hyear must be after (or equal to) the original date.
 * Returns 0 on success (result in *result), nonzero on error. */
int heb_anniversary_birthday(int hyear, const heb_hdate_t *date, heb_hdate_t *result);

#endif /* HEB_ANNIVERSARY_H */
