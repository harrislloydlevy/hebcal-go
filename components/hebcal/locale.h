/*
 * hebcal-c - Locale translation lookup (en + he).
 * Ported from github.com/hebcal/locales (Go).
 *
 * English is identity passthrough; Hebrew uses a lookup table.
 * he-x-NoNikud strips niqqud from the Hebrew result.
 *
 * License: GPL-2.0-or-later
 */
#ifndef HEB_LOCALE_H
#define HEB_LOCALE_H

#include <stddef.h>

/* Looks up a translation key in the given locale.
 * Writes the result into buf (n bytes). Returns bytes written (excl. NUL).
 * If not found, returns the key as-is (identity passthrough). */
int heb_locale_lookup(const char *key, const char *locale,
                      char *buf, size_t n);

/* Returns 1 if locale is a Hebrew locale (he or he-x-NoNikud). */
int heb_locale_is_hebrew(const char *locale);

#endif /* HEB_LOCALE_H */
