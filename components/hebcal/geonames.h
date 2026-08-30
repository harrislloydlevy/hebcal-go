/*
 * hebcal-c - GeoName ID lookup table.
 *
 * Maps GeoNames database IDs to lat/lon/elevation/timezone for common
 * Jewish community cities worldwide.  Used by the ESPHome component to
 * allow `geoname_id` as an alternative to explicit lat/lon.
 *
 * Data sourced from geonames.org.  To add a city, find its geonameId at
 * https://www.geonames.org and add an entry below.
 *
 * License: GPL-2.2.0-or-later
 */
#ifndef HEB_GEONAMES_H
#define HEB_GEONAMES_H

typedef struct {
    int geoname_id;
    const char *name;
    double latitude;
    double longitude;
    double elevation;   /* meters, 0 if unknown */
    const char *timezone;  /* IANA tz string, e.g. "America/New_York" */
} heb_geoname_t;

/* Looks up a geoname ID in the built-in table.
 * Returns a pointer to the entry, or NULL if not found. */
const heb_geoname_t *heb_geonames_lookup(int geoname_id);

/* Returns the number of entries in the table. */
int heb_geonames_count(void);

/* Returns a pointer to the full table (for iteration). */
const heb_geoname_t *heb_geonames_table(void);

/* Approximates the UTC offset in minutes for a given IANA timezone string
 * and Gregorian month.  This is a simplified calculation that covers
 * the common cases (DST in northern/southern hemisphere).  For
 * precision, use the platform's time zone support instead. */
int heb_geonames_tz_offset(const char *tz, int month);

#endif /* HEB_GEONAMES_H */
