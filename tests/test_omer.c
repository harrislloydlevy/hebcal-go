/*
 * test_omer.c - 1:1 port of omer_test.go to C.
 * License: GPL-2.0-or-later
 */
#include "hebtest.h"
#include "omer.h"
#include "hdate.h"
#include "locale.h"
#include <string.h>

HEBTEST_FUNC(test_omer) {
    /* TestSefira */
    {
        heb_hdate_t hd;
        heb_hdate_create(&hd, 5770, HEB_H_SIVAN, 2);
        heb_omer_t ev = heb_omer_new(hd, 46);

        char buf[128];
        heb_omer_sefira(&ev, "en", buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Eternity within Majesty");

        heb_omer_sefira(&ev, "he", buf, sizeof(buf));
        /* נֶּצַח שֶׁבְּמַּלְכוּת */
        HEB_CHECK_STR_EQ(buf, "\xd7\xa0\xd6\xb6\xd6\xbc\xd7\xa6\xd6\xb7\xd7\x97 "
                              "\xd7\xa9\xd6\xb6\xd7\x81\xd7\x91\xd6\xb0\xd6\xbc\xd7\x9e\xd6\xb7\xd6\xbc\xd7\x9c\xd6\xb0\xd7\x9b\xd7\x95\xd6\xbc\xd7\xaa");

        heb_omer_sefira(&ev, "translit", buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Netzach sheb'Malkhut");
    }

    /* TestTodayIsEn */
    {
        heb_hdate_t hd;
        heb_omer_t ev;
        char buf[256];

        heb_hdate_create(&hd, 5770, HEB_H_NISAN, 16);
        ev = heb_omer_new(hd, 1);
        heb_omer_today_is(&ev, "en", buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Today is 1 day of the Omer");

        heb_hdate_create(&hd, 5770, HEB_H_NISAN, 17);
        ev = heb_omer_new(hd, 2);
        heb_omer_today_is(&ev, "en", buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Today is 2 days of the Omer");

        heb_hdate_create(&hd, 5770, HEB_H_NISAN, 22);
        ev = heb_omer_new(hd, 7);
        heb_omer_today_is(&ev, "en", buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Today is 7 days, which is 1 week of the Omer");

        heb_hdate_create(&hd, 5770, HEB_H_NISAN, 23);
        ev = heb_omer_new(hd, 8);
        heb_omer_today_is(&ev, "en", buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Today is 8 days, which is 1 week and 1 day of the Omer");

        heb_hdate_create(&hd, 5770, HEB_H_NISAN, 28);
        ev = heb_omer_new(hd, 13);
        heb_omer_today_is(&ev, "en", buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Today is 13 days, which is 1 week and 6 days of the Omer");

        heb_hdate_create(&hd, 5770, HEB_H_NISAN, 29);
        ev = heb_omer_new(hd, 14);
        heb_omer_today_is(&ev, "en", buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Today is 14 days, which is 2 weeks of the Omer");

        heb_hdate_create(&hd, 5770, HEB_H_IYYAR, 26);
        ev = heb_omer_new(hd, 41);
        heb_omer_today_is(&ev, "en", buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Today is 41 days, which is 5 weeks and 6 days of the Omer");

        heb_hdate_create(&hd, 5770, HEB_H_SIVAN, 2);
        ev = heb_omer_new(hd, 46);
        heb_omer_today_is(&ev, "en", buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Today is 46 days, which is 6 weeks and 4 days of the Omer");
    }

    /* TestTodayIsHe - spot-check a few representative days */
    {
        heb_hdate_t hd;
        heb_omer_t ev;
        char buf[512];

        /* Day 1: הַיוֹם יוֹם אֶחָד לָעוֹמֶר */
        heb_hdate_create(&hd, 5782, HEB_H_NISAN, 16);
        ev = heb_omer_new(hd, 1);
        heb_omer_today_is(&ev, "he", buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "\xd7\x94\xd6\xb7\xd7\x99\xd7\x95\xd6\xb9\xd7\x9d "
                              "\xd7\x99\xd7\x95\xd6\xb9\xd7\x9d "
                              "\xd7\x90\xd6\xb6\xd7\x97\xd6\xb8\xd7\x93 "
                              "\xd7\x9c\xd6\xb8\xd7\xa2\xd7\x95\xd6\xb9\xd7\x9e\xd6\xb6\xd7\xa8");

        /* Day 7: הַיוֹם שִׁבְעָה יָמִים, שְׁהֵם שָׁבוּעַ אֶחָד לָעוֹמֶר */
        ev = heb_omer_new(hd, 7);
        heb_omer_today_is(&ev, "he", buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "\xd7\x94\xd6\xb7\xd7\x99\xd7\x95\xd6\xb9\xd7\x9d "
                              "\xd7\xa9\xd6\xb4\xd7\x81\xd7\x91\xd6\xb0\xd7\xa2\xd6\xb8\xd7\x94 "
                              "\xd7\x99\xd6\xb8\xd7\x9e\xd6\xb4\xd7\x99\xd7\x9d, "
                              "\xd7\xa9\xd6\xb0\xd7\x81\xd7\x94\xd6\xb5\xd7\x9d "
                              "\xd7\xa9\xd6\xb8\xd7\x81\xd7\x91\xd7\x95\xd6\xbc\xd7\xa2\xd6\xb7 "
                              "\xd7\x90\xd6\xb6\xd7\x97\xd6\xb8\xd7\x93 "
                              "\xd7\x9c\xd6\xb8\xd7\xa2\xd7\x95\xd6\xb9\xd7\x9e\xd6\xb6\xd7\xa8");

        /* Day 14: הַיוֹם אַרְבָּעָה עָשָׂר יוֹם, שְׁהֵם שְׁנֵי שָׁבוּעוֹת לָעוֹמֶר */
        ev = heb_omer_new(hd, 14);
        heb_omer_today_is(&ev, "he", buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "\xd7\x94\xd6\xb7\xd7\x99\xd7\x95\xd6\xb9\xd7\x9d "
                              "\xd7\x90\xd6\xb7\xd7\xa8\xd6\xb0\xd7\x91\xd6\xb8\xd6\xbc\xd7\xa2\xd6\xb8\xd7\x94 "
                              "\xd7\xa2\xd6\xb8\xd7\xa9\xd6\xb8\xd7\x82\xd7\xa8 "
                              "\xd7\x99\xd7\x95\xd6\xb9\xd7\x9d, "
                              "\xd7\xa9\xd6\xb0\xd7\x81\xd7\x94\xd6\xb5\xd7\x9d "
                              "\xd7\xa9\xd6\xb0\xd7\x81\xd7\xa0\xd6\xb5\xd7\x99 "
                              "\xd7\xa9\xd6\xb8\xd7\x81\xd7\x91\xd7\x95\xd6\xbc\xd7\xa2\xd7\x95\xd6\xb9\xd7\xaa "
                              "\xd7\x9c\xd6\xb8\xd7\xa2\xd7\x95\xd6\xb9\xd7\x9e\xd6\xb6\xd7\xa8");

        /* Day 49: הַיוֹם תִּשְׁעָה וְאַרְבָּעִים יוֹם, שְׁהֵם שִׁבְעָה שָׁבוּעוֹת לָעוֹמֶר */
        ev = heb_omer_new(hd, 49);
        heb_omer_today_is(&ev, "he", buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "\xd7\x94\xd6\xb7\xd7\x99\xd7\x95\xd6\xb9\xd7\x9d "
                              "\xd7\xaa\xd6\xb4\xd6\xbc\xd7\xa9\xd6\xb0\xd7\x81\xd7\xa2\xd6\xb8\xd7\x94 "
                              "\xd7\x95\xd6\xb0\xd7\x90\xd6\xb7\xd7\xa8\xd6\xb0\xd7\x91\xd6\xb8\xd6\xbc\xd7\xa2\xd6\xb4\xd7\x99\xd7\x9d "
                              "\xd7\x99\xd7\x95\xd6\xb9\xd7\x9d, "
                              "\xd7\xa9\xd6\xb0\xd7\x81\xd7\x94\xd6\xb5\xd7\x9d "
                              "\xd7\xa9\xd6\xb4\xd7\x81\xd7\x91\xd6\xb0\xd7\xa2\xd6\xb8\xd7\x94 "
                              "\xd7\xa9\xd6\xb8\xd7\x81\xd7\x91\xd7\x95\xd6\xbc\xd7\xa2\xd7\x95\xd6\xb9\xd7\xaa "
                              "\xd7\x9c\xd6\xb8\xd7\xa2\xd7\x95\xd6\xb9\xd7\x9e\xd6\xb6\xd7\xa8");

        /* Day 13 no-nikud: היום שלושה עשר יום, שהם שבוע אחד וששה ימים לעומר */
        ev = heb_omer_new(hd, 13);
        heb_omer_today_is(&ev, "he-x-NoNikud", buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "\xd7\x94\xd7\x99\xd7\x95\xd7\x9d "
                              "\xd7\xa9\xd7\x9c\xd7\x95\xd7\xa9\xd7\x94 "
                              "\xd7\xa2\xd7\xa9\xd7\xa8 "
                              "\xd7\x99\xd7\x95\xd7\x9d, "
                              "\xd7\xa9\xd7\x94\xd7\x9d "
                              "\xd7\xa9\xd7\x91\xd7\x95\xd7\xa2 "
                              "\xd7\x90\xd7\x97\xd7\x93 "
                              "\xd7\x95\xd7\xa9\xd7\xa9\xd7\x94 "
                              "\xd7\x99\xd7\x9e\xd7\x99\xd7\x9d "
                              "\xd7\x9c\xd7\xa2\xd7\x95\xd7\x9e\xd7\xa8");
    }

    /* TestEmoji - spot check */
    {
        heb_hdate_t hd;
        heb_hdate_create(&hd, 5770, HEB_H_NISAN, 28);
        heb_omer_t ev = heb_omer_new(hd, 13);
        char buf[8];
        heb_omer_emoji(&ev, buf, sizeof(buf));
        /* ⑬ = U+246C = E2 91 AC */
        HEB_CHECK_STR_EQ(buf, "\xe2\x91\xac");
    }

    return hebtest_failures;
}
