/*
 * hebcal-c - NOAA solar position calculator.
 * Ported from github.com/hebcal/noaa-go (Go), which is a port of
 * the KosherJava Zmanim API by Eliyahu Hershfeld.
 *
 * Calculates sunrise, sunset, solar noon, solar elevation and azimuth
 * using the NOAA solar position algorithm.
 *
 * License: LGPL-2.1-or-later (see LICENSE for details)
 */
#ifndef HEB_NOAA_H
#define HEB_NOAA_H

/* Geometric zenith (90 degrees) - the zenith of astronomical sunrise/sunset. */
#define HEB_GEOMETRIC_ZENITH 90.0

/* Julian day of J2000.0 epoch (Jan 1, 2000). */
#define HEB_JD_J2000 2451545.0

/* Julian days per century. */
#define HEB_JD_PER_CENTURY 36525.0

/* Geo location for solar calculations. */
typedef struct {
    double latitude;   /* degrees, -90 to 90, south negative */
    double longitude;  /* degrees, -180 to 180, west negative */
    double elevation;  /* meters above sea level, >= 0 */
} heb_geolocation_t;

/* Calculator configuration. */
typedef struct {
    double refraction;           /* degrees (default 34/60) */
    double solar_radius;         /* degrees (default 16/60) */
    int use_apparent_solar_radius; /* 1 = date-based, 0 = fixed */
    double earth_radius;         /* KM (default 6371.0088) */
} heb_noaa_calc_t;

/* Initializes a calculator with default settings. */
heb_noaa_calc_t heb_noaa_new(void);

/* Computes the Julian day for a Gregorian date. */
double heb_julian_day(int year, int month, int day);

/* Converts Julian day to Julian centuries since J2000.0. */
double heb_julian_centuries(double jd);

/* Calculates UTC sunrise in 24-hour decimal format (5.75 = 05:45).
 * Returns NaN if the event does not occur (polar regions).
 * zenith: 90.0 for true sunrise, higher for twilight.
 * adjust_for_elevation: 1 to account for location elevation. */
double heb_noaa_utc_sunrise(const heb_noaa_calc_t *calc,
                            const heb_geolocation_t *geo,
                            int year, int month, int day,
                            double zenith, int adjust_for_elevation);

/* Calculates UTC sunset in 24-hour decimal format. */
double heb_noaa_utc_sunset(const heb_noaa_calc_t *calc,
                           const heb_geolocation_t *geo,
                           int year, int month, int day,
                           double zenith, int adjust_for_elevation);

/* Calculates UTC solar noon in 24-hour decimal format. */
double heb_noaa_utc_noon(const heb_noaa_calc_t *calc,
                         const heb_geolocation_t *geo,
                         int year, int month, int day);

/* Calculates UTC solar midnight in 24-hour decimal format. */
double heb_noaa_utc_midnight(const heb_noaa_calc_t *calc,
                             const heb_geolocation_t *geo,
                             int year, int month, int day);

/* Calculates solar elevation in degrees at a given UTC time.
 * utc_hours: 24-hour decimal UTC time. Can be negative for no event. */
double heb_noaa_solar_elevation(const heb_geolocation_t *geo,
                                int year, int month, int day,
                                double utc_hours);

/* Calculates solar azimuth in degrees at a given UTC time. */
double heb_noaa_solar_azimuth(const heb_geolocation_t *geo,
                              int year, int month, int day,
                              double utc_hours);

/* Adjusts zenith for refraction, solar radius, and elevation. */
double heb_noaa_adjust_zenith(const heb_noaa_calc_t *calc,
                              double zenith, double elevation,
                              int day_of_year);

/* Returns the elevation adjustment in degrees. */
double heb_noaa_elevation_adjustment(const heb_noaa_calc_t *calc,
                                     double elevation);

#endif /* HEB_NOAA_H */
