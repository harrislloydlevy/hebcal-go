/*
 * hebcal-c - Locale translation lookup.
 * Ported from github.com/hebcal/locales (Go).
 *
 * License: GPL-2.0-or-later
 */
#include "locale.h"
#include "hdate.h"          /* for heb_hebrew_strip_nikkud */
#include "locale_data.h"
#include <string.h>

/* Case-insensitive ASCII comparison for locale names. */
static int locale_ieq(const char *a, const char *b) {
    while (*a && *b) {
        char ca = *a, cb = *b;
        if (ca >= 'A' && ca <= 'Z') ca += 32;
        if (cb >= 'A' && cb <= 'Z') cb += 32;
        if (ca != cb) return 0;
        a++; b++;
    }
    return *a == *b;
}

int heb_locale_is_hebrew(const char *locale) {
    return locale_ieq(locale, "he") || locale_ieq(locale, "he-x-nonikud");
}

/* Binary search in the sorted Hebrew entries array. */
static const char *lookup_he(const char *key) {
    int lo = 0, hi = HEB_HE_ENTRIES_COUNT - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        const char *k, *v;
#if defined(__AVR__)
        /* PROGMEM read */
        k = (const char *)pgm_read_ptr_near(&heb_he_entries[mid].key);
        v = (const char *)pgm_read_ptr_near(&heb_he_entries[mid].val);
#else
        k = heb_he_entries[mid].key;
        v = heb_he_entries[mid].val;
#endif
        int cmp = strcmp(key, k);
        if (cmp == 0) return v;
        if (cmp < 0) hi = mid - 1;
        else lo = mid + 1;
    }
    return NULL;
}

int heb_locale_lookup(const char *key, const char *locale,
                      char *buf, size_t n) {
    if (!key || n == 0) { if (buf && n) buf[0] = '\0'; return 0; }

    if (heb_locale_is_hebrew(locale)) {
        const char *val = lookup_he(key);
        if (val) {
            if (locale_ieq(locale, "he-x-nonikud")) {
                return heb_hebrew_strip_nikkud(val, buf, n);
            }
            size_t len = strlen(val);
            if (len >= n) len = n - 1;
            memcpy(buf, val, len);
            buf[len] = '\0';
            return (int)len;
        }
    }

    /* Identity passthrough for en and unfound keys */
    size_t len = strlen(key);
    if (len >= n) len = n - 1;
    memcpy(buf, key, len);
    buf[len] = '\0';
    return (int)len;
}
