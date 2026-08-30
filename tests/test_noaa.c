/*
 * test_noaa.c - Tests for NOAA solar calculator.
 * License: GPL-2.0-or-later
 */
#include "hebtest.h"
#include "noaa.h"
#include <math.h>

HEBTEST_FUNC(test_noaa) {
    /* JulianDay for Jan 1, 2000 = 2451544.5 (J2000.0 minus 0.5 for noon) */
    HEB_CHECK_DOUBLE_NEAR(heb_julian_day(2000, 1, 1), 2451544.5, 0.001);

    /* JulianDay for June 5, 2020 */
    HEB_CHECK_DOUBLE_NEAR(heb_julian_day(2020, 6, 5), 2459005.5, 0.001);

    /* JulianCenturies for J2000.0 = 0 */
    HEB_CHECK_DOUBLE_NEAR(heb_julian_centuries(2451545.0), 0.0, 1e-10);

    /* Chicago: lat=41.85003, lon=-87.65005, elevation=180 */
    heb_geolocation_t chicago = {41.85003, -87.65005, 180.0};
    heb_noaa_calc_t calc = heb_noaa_new();

    /* Sunrise on June 5, 2020 in Chicago (sea level, matching Go test)
     * Expected: 10:16:19 UTC = ~10.2719 hours */
    double sunrise = heb_noaa_utc_sunrise(&calc, &chicago, 2020, 6, 5,
                                           HEB_GEOMETRIC_ZENITH, 0);
    HEB_CHECK_DOUBLE_NEAR(sunrise, 10.2719, 0.003);

    /* Sunset on June 5, 2020 in Chicago (sea level)
     * Expected: 01:22:27 UTC (next day) = ~1.3742 hours */
    double sunset = heb_noaa_utc_sunset(&calc, &chicago, 2020, 6, 5,
                                         HEB_GEOMETRIC_ZENITH, 0);
    HEB_CHECK_DOUBLE_NEAR(sunset, 1.3742, 0.003);

    /* Solar noon on June 5, 2020 in Chicago
     * Expected: 12:49:22 local CDT = 17:49:22 UTC = ~17.823 hours */
    double noon = heb_noaa_utc_noon(&calc, &chicago, 2020, 6, 5);
    HEB_CHECK_DOUBLE_NEAR(noon, 17.823, 0.003);

    /* Tel Aviv: lat=32.08088, lon=34.78057, elevation=15 */
    heb_geolocation_t telaviv = {32.08088, 34.78057, 15.0};

    /* Sunrise on March 6, 2021 in Tel Aviv (sea level)
     * Expected: 04:02:15 UTC = ~4.0375 hours */
    sunrise = heb_noaa_utc_sunrise(&calc, &telaviv, 2021, 3, 6,
                                    HEB_GEOMETRIC_ZENITH, 0);
    HEB_CHECK_DOUBLE_NEAR(sunrise, 4.0375, 0.003);

    /* Sunset on March 6, 2021 in Tel Aviv (sea level)
     * Expected: 15:42:22 UTC = ~15.7061 hours */
    sunset = heb_noaa_utc_sunset(&calc, &telaviv, 2021, 3, 6,
                                  HEB_GEOMETRIC_ZENITH, 0);
    HEB_CHECK_DOUBLE_NEAR(sunset, 15.7061, 0.003);

    /* Solar elevation at a specific time
     * Chicago, June 5, 2020, 16:00 UTC (11:00 AM CDT)
     * Sun should be high in the sky (summer) */
    double elev = heb_noaa_solar_elevation(&chicago, 2020, 6, 5, 16.0);
    HEB_CHECK(elev > 60.0 && elev < 75.0); /* roughly 69° */

    return hebtest_failures;
}
