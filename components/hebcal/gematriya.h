/*
 * hebcal-c - Hebrew gematriya (numbers as Hebrew letters).
 * Ported from github.com/hebcal/gematriya (Go).
 *
 * License: GPL-2.0-or-later
 */
#ifndef HEB_GEMATRIYA_H
#define HEB_GEMATRIYA_H

#include <stddef.h>

/* Converts a numerical value to a string of Hebrew letters (gematriya).
 * Writes into buf (n bytes).  Returns bytes written (excl. NUL).
 *
 * When specifying years in the present millennium, the thousands (5) are
 * omitted, matching the Go reference. */
int heb_gematriya(int number, char *buf, size_t n);

#endif /* HEB_GEMATRIYA_H */
