/*
 * hebcal-c - Sefirat HaOmer (Counting of the Omer).
 * Ported from github.com/hebcal/hebcal-go/omer (Go).
 *
 * License: GPL-2.0-or-later
 */
#ifndef HEB_OMER_H
#define HEB_OMER_H

#include "hdate.h"
#include <stddef.h>

typedef struct {
    heb_hdate_t date;
    int omer_day;
    int week_number;
    int days_within_weeks;
} heb_omer_t;

/* Constructs an OmerEvent for the given Hebrew date and day of the Omer (1-49). */
heb_omer_t heb_omer_new(heb_hdate_t date, int omerDay);

/* Renders "Nth day of the Omer" (en) or gematriya + " day of the Omer" (he). */
void heb_omer_render(const heb_omer_t *ev, const char *locale,
                     char *buf, size_t n);

/* Renders the full "Today is N days..." text. */
void heb_omer_today_is(const heb_omer_t *ev, const char *locale,
                       char *buf, size_t n);

/* Renders the sefira name (e.g. "Eternity within Majesty"). */
void heb_omer_sefira(const heb_omer_t *ev, const char *locale,
                     char *buf, size_t n);

/* Renders the emoji (circled number) for the Omer day. */
void heb_omer_emoji(const heb_omer_t *ev, char *buf, size_t n);

/* Returns the number of complete weeks (accounting for the 7th day rule). */
int heb_omer_get_weeks(const heb_omer_t *ev);

#endif /* HEB_OMER_H */
