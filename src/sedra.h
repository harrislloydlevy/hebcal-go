/*
 * hebcal-c - Weekly Torah reading (Parashat HaShavua).
 * Ported from github.com/hebcal/hebcal-go/sedra (Go).
 *
 * License: GPL-2.0-or-later
 */
#ifndef HEB_SEDRA_H
#define HEB_SEDRA_H

#include "hdate.h"
#include <stddef.h>

/* A parsha result from the sedra lookup. */
typedef struct {
    const char *name1;   /* First parsha name (English), or NULL if Chag */
    const char *name2;   /* Second parsha name for doubles, or NULL */
    int num1;            /* First parsha number (1-indexed), 0 if Chag */
    int num2;            /* Second parsha number for doubles, 0 if single */
    int chag;            /* 1 if this is a special holiday reading */
} heb_parsha_t;

/* Sedra schedule for an entire Hebrew year. */
typedef struct {
    int year;            /* Hebrew year */
    int il;              /* Israel schedule flag */
    heb_rd_t first_saturday; /* R.D. of the first Shabbat of the year */
    const int *array;    /* Pointer to the sedra array for this year */
    int array_len;       /* Length of the sedra array */
} heb_sedra_t;

/* Constructs a Sedra schedule for the given Hebrew year. */
heb_sedra_t heb_sedra_new(int year, int il);

/* Looks up the parsha read on the Saturday on or after the given date. */
heb_parsha_t heb_sedra_lookup(const heb_sedra_t *s, const heb_hdate_t *hd);

/* Looks up the parsha by R.D. date. */
heb_parsha_t heb_sedra_lookup_rd(const heb_sedra_t *s, heb_rd_t rataDie);

/* Finds the date on which the given parsha number (Bereshit=1) is read.
 * Returns 0 on success (result in *result), nonzero on error. */
int heb_sedra_find_parsha_num(const heb_sedra_t *s, int num,
                              heb_hdate_t *result);

/* Renders a parsha name (e.g. "Chukat-Balak") in the given locale. */
void heb_parsha_render(const heb_parsha_t *p, const char *locale,
                       char *buf, size_t n);

/* Returns the 54 parsha names array (English transliteration). */
const char *const *heb_parshiot(void);

/* Number of parshiyot. */
#define HEB_NUM_PARSHIOT 54

#endif /* HEB_SEDRA_H */
