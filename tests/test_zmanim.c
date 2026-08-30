/*
 * test_zmanim.c - Tests for halachic times (zmanim).
 * Ported from zmanim_test.go to C.
 * License: GPL-2.0-or-later
 */
#include "hebtest.h"
#include "zmanim.h"
#include <math.h>

/* Chicago: lat=41.85003, lon=-87.65005, elevation=180, UTC offset=-300 (CDT) */
/* Tel Aviv: lat=32.08088, lon=34.78057, elevation=15, UTC offset=120 (IST) */

HEBTEST_FUNC(test_zmanim) {
    /* TestZmanimChicago - June 5, 2020 */
    {
        heb_zmanim_t z = heb_zmanim_new(41.85003, -87.65005, 180.0,
                                        2020, 6, 5, -300);
        /* Go test uses UseElevation=false (default) */

        /* Sunrise: 05:16:19 CDT = 10:16:19 UTC = ~10.2719h */
        HEB_CHECK_DOUBLE_NEAR(heb_zmanim_sunrise(&z), 10.2719, 0.003);

        /* Sunset: 20:22:27 CDT = 01:22:27 UTC (next day) = ~1.3742h */
        HEB_CHECK_DOUBLE_NEAR(heb_zmanim_sunset(&z), 1.3742, 0.003);

        /* Halachic hour: 4530.667 seconds */
        double h = heb_zmanim_hour(&z) * 3600.0;
        HEB_CHECK_DOUBLE_NEAR(h, 4530.667, 2.0);

        /* Chatzot: 12:49:23 CDT = 17:49:23 UTC = ~17.8231h */
        HEB_CHECK_DOUBLE_NEAR(heb_zmanim_chatzot(&z), 17.823, 0.004);

        /* Alot HaShachar: 03:25:29 CDT = 08:25:29 UTC = ~8.4247h */
        HEB_CHECK_DOUBLE_NEAR(heb_zmanim_alot_hashachar(&z), 8.425, 0.005);

        /* Misheyakir: 04:03:04 CDT = 09:03:04 UTC = ~9.0512h */
        HEB_CHECK_DOUBLE_NEAR(heb_zmanim_misheyakir(&z), 9.051, 0.005);

        /* SofZmanShma: 09:02:51 CDT = 14:02:51 UTC = ~14.0475h */
        HEB_CHECK_DOUBLE_NEAR(heb_zmanim_sof_zman_shma(&z), 14.048, 0.005);

        /* SofZmanTfilla: 10:18:21 CDT = 15:18:21 UTC = ~15.3058h */
        HEB_CHECK_DOUBLE_NEAR(heb_zmanim_sof_zman_tfilla(&z), 15.306, 0.005);

        /* MinchaGedola: 12:49:23 CDT = 17:49:23 UTC + 0.5*hour ≈ 18.453h */
        HEB_CHECK_DOUBLE_NEAR(heb_zmanim_mincha_gedola(&z), 18.452, 0.01);

        /* PlagHaMincha: 18:48:03 CDT = 23:48:03 UTC = ~23.801h */
        HEB_CHECK_DOUBLE_NEAR(heb_zmanim_plag_hamincha(&z), 23.80, 0.01);

        /* Tzeit 8.5: 21:13:47 CDT = 02:13:47 UTC (next day) = ~2.230h */
        HEB_CHECK_DOUBLE_NEAR(heb_zmanim_tzeit(&z, 8.5), 2.230, 0.005);

        /* GregEve (previous day's sunset): 20:21:48 CDT = 01:21:48 UTC = ~1.363h */
        HEB_CHECK_DOUBLE_NEAR(heb_zmanim_greg_eve(&z), 1.363, 0.005);
    }

    /* TestZmanimTelAviv - March 6, 2021 */
    {
        heb_zmanim_t z = heb_zmanim_new(32.08088, 34.78057, 15.0,
                                        2021, 3, 6, 120);

        /* Sunrise: 06:02:15 IST = 04:02:15 UTC = ~4.0375h */
        HEB_CHECK_DOUBLE_NEAR(heb_zmanim_sunrise(&z), 4.0375, 0.002);

        /* Sunset: 17:42:22 IST = 15:42:22 UTC = ~15.7061h */
        HEB_CHECK_DOUBLE_NEAR(heb_zmanim_sunset(&z), 15.7061, 0.002);

        /* Chatzot: 11:52:18 IST = 09:52:18 UTC = ~9.8717h */
        HEB_CHECK_DOUBLE_NEAR(heb_zmanim_chatzot(&z), 9.872, 0.003);

        /* Alot HaShachar: 04:50:09 IST = 02:50:09 UTC = ~2.836h */
        HEB_CHECK_DOUBLE_NEAR(heb_zmanim_alot_hashachar(&z), 2.836, 0.003);
    }

    /* Test formatting */
    {
        char buf[32];
        heb_zmanim_format_time(10.2719, -300, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "05:16:19");

        heb_zmanim_format_time(15.7061, 120, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "17:42:22");
    }

    return hebtest_failures;
}
