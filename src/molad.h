/*
 * hebcal-c - Molad (New Moon) calculation.
 * Ported from github.com/hebcal/hebcal-go/molad (Go).
 *
 * License: GPL-2.0-or-later
 */
#ifndef HEB_MOLAD_H
#define HEB_MOLAD_H

#include "hdate.h"

typedef struct {
    heb_hdate_t date;   /* Hebrew date of the molad */
    int hours;          /* Hour of day (0-23) */
    int minutes;        /* Minutes (0-59) */
    int chalakim;       /* Chalakim (parts, 0-17) */
} heb_molad_t;

/* Calculates the molad for a given Hebrew year and month. */
heb_molad_t heb_molad_new(int year, heb_hmonth_t month);

#endif /* HEB_MOLAD_H */
