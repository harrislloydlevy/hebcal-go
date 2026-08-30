/*
 * hebcal-c - Holiday definitions and lookup.
 * Ported from github.com/hebcal/hebcal-go/hebcal/holidays.go (Go).
 *
 * License: GPL-2.0-or-later
 */
#ifndef HEB_HOLIDAYS_H
#define HEB_HOLIDAYS_H

#include "hdate.h"
#include "event.h"
#include <stddef.h>

/* Generates all holidays for a Hebrew year.
 * Writes up to max_events events into the events array.
 * Returns the number of events written.
 * If il is nonzero, uses the Israel schedule; otherwise Diaspora. */
int heb_holidays_for_year(int year, int il,
                           heb_event_t *events, int max_events);

/* Looks up holidays for a specific Hebrew date.
 * Writes up to max_events events into the events array.
 * Returns the number of events found. */
int heb_holidays_for_date(const heb_hdate_t *hd, int il,
                           heb_event_t *events, int max_events);

#endif /* HEB_HOLIDAYS_H */
