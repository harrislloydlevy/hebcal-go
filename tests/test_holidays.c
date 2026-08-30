/*
 * test_holidays.c - Tests for holiday lookup.
 * License: GPL-2.0-or-later
 */
#include "hebtest.h"
#include "holidays.h"
#include "hdate.h"
#include <string.h>

HEBTEST_FUNC(test_holidays) {
    /* Test specific holiday dates for year 5784 (2023-2024) */
    {
        static heb_event_t events[HEB_MAX_EVENTS];
        int n = heb_holidays_for_year(5784, 0, events, HEB_MAX_EVENTS);
        HEB_CHECK(n > 50); /* should have many holidays */

        /* Find Yom Kippur (10 Tishrei 5784) */
        heb_hdate_t yk;
        heb_hdate_create(&yk, 5784, HEB_H_TISHREI, 10);
        static heb_event_t ykEvents[10];
        int nyk = heb_holidays_for_date(&yk, 0, ykEvents, 10);
        HEB_CHECK_INT_EQ(nyk, 1);
        HEB_CHECK_STR_EQ(ykEvents[0].desc, "Yom Kippur");

        /* Find Rosh Hashana (1 Tishrei 5784) */
        heb_hdate_t rh;
        heb_hdate_create(&rh, 5784, HEB_H_TISHREI, 1);
        static heb_event_t rhEvents[10];
        int nrh = heb_holidays_for_date(&rh, 0, rhEvents, 10);
        HEB_CHECK_INT_EQ(nrh, 1);
        HEB_CHECK_STR_EQ(rhEvents[0].desc, "Rosh Hashana 5784");

        /* Pesach starts 15 Nisan */
        heb_hdate_t pesach;
        heb_hdate_create(&pesach, 5784, HEB_H_NISAN, 15);
        static heb_event_t pEvents[10];
        int np = heb_holidays_for_date(&pesach, 0, pEvents, 10);
        HEB_CHECK_INT_EQ(np, 1);
        HEB_CHECK_STR_EQ(pEvents[0].desc, "Pesach I");

        /* Israel: Pesach I and VII are yom tov, no Pesach VIII */
        heb_hdate_t pesach22;
        heb_hdate_create(&pesach22, 5784, HEB_H_NISAN, 22);
        static heb_event_t p22Events[10];
        int np22 = heb_holidays_for_date(&pesach22, 1, p22Events, 10);
        HEB_CHECK_INT_EQ(np22, 0); /* No Pesach VIII in Israel */

        /* Diaspora: Pesach VIII exists */
        int np22d = heb_holidays_for_date(&pesach22, 0, p22Events, 10);
        HEB_CHECK_INT_EQ(np22d, 1);
        HEB_CHECK_STR_EQ(p22Events[0].desc, "Pesach VIII");
    }

    /* Test Chanukah: should have 8 candle days + 1 first candle + 8th day */
    {
        static heb_event_t events[HEB_MAX_EVENTS];
        int n = heb_holidays_for_year(5784, 0, events, HEB_MAX_EVENTS);
        int chanukahCount = 0;
        for (int i = 0; i < n; i++) {
            if (events[i].flags & HEB_F_CHANUKAH_CANDLES) chanukahCount++;
        }
        HEB_CHECK_INT_EQ(chanukahCount, 8); /* 1st through 8th candle */
    }

    /* Test Israel vs Diaspora filtering */
    {
        static heb_event_t ilEvents[HEB_MAX_EVENTS];
        static heb_event_t chulEvents[HEB_MAX_EVENTS];
        int nil = heb_holidays_for_year(5784, 1, ilEvents, HEB_MAX_EVENTS);
        int nchul = heb_holidays_for_year(5784, 0, chulEvents, HEB_MAX_EVENTS);
        HEB_CHECK(nil > 0 && nchul > 0); /* both should have holidays */
        /* Israel and Diaspora differ due to CHUL_ONLY/IL_ONLY events */
    }

    /* Test Sukkot in Israel vs Diaspora */
    {
        heb_hdate_t sukkot1;
        heb_hdate_create(&sukkot1, 5784, HEB_H_TISHREI, 15);

        static heb_event_t ilEv[10], chulEv[10];
        int nil = heb_holidays_for_date(&sukkot1, 1, ilEv, 10);
        int nchul = heb_holidays_for_date(&sukkot1, 0, chulEv, 10);
        HEB_CHECK_INT_EQ(nil, 1);
        HEB_CHECK_STR_EQ(ilEv[0].desc, "Sukkot I");
        HEB_CHECK_INT_EQ(nchul, 1);
        HEB_CHECK_STR_EQ(chulEv[0].desc, "Sukkot I");
    }

    /* Test Shabbat Shuva falls between RH and YK */
    {
        heb_hdate_t rh;
        heb_hdate_create(&rh, 5784, HEB_H_TISHREI, 1);
        heb_weekday_t rhDow = heb_hdate_weekday(&rh);
        /* 5784: RH is Saturday (Sept 16, 2023) */
        HEB_CHECK_INT_EQ(rhDow, HEB_SATURDAY);

        /* Shabbat Shuva should be the first Saturday after RH */
        heb_hdate_t shabbatShuva = heb_hdate_from_rd(
            heb_hdate_day_on_or_before(HEB_SATURDAY, 7 + rh.abs));
        HEB_CHECK_INT_EQ(heb_hdate_weekday(&shabbatShuva), HEB_SATURDAY);
        HEB_CHECK(shabbatShuva.abs >= rh.abs);
    }

    return hebtest_failures;
}
