/*
 * test_hdate.c - 1:1 port of hdate_test.go + elapsed_test.go to C.
 * License: GPL-2.0-or-later
 */
#include "hebtest.h"
#include "hdate.h"
#include "greg.h"

/* Helper: convert HDate to ISO date string (YYYY-MM-DD) like Go's hd2iso */
static void hd2iso(const heb_hdate_t *hd, char *buf, size_t n) {
    heb_gdate_t gd = heb_hdate_greg(hd);
    snprintf(buf, n, "%04d-%02d-%02d", gd.year, gd.month, gd.day);
}

/* Helper: format Gregorian as M/D/YYYY (like Go's fmt.Sprintf("%d/%d/%d")) */
static void greg_mdy(const heb_gdate_t *gd, char *buf, size_t n) {
    snprintf(buf, n, "%d/%d/%d", gd->month, gd->day, gd->year);
}

HEBTEST_FUNC(test_hdate) {
    /* TestHebrew2RD */
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(5769, HEB_H_CHESHVAN, 15), 733359);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(5708, HEB_H_IYYAR, 6), 711262);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(3762, HEB_H_TISHREI, 1), 249);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(3761, HEB_H_NISAN, 1), 72);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(3761, HEB_H_TEVET, 18), 1);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(3761, HEB_H_TEVET, 17), 0);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(3761, HEB_H_TEVET, 16), -1);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(3761, HEB_H_TEVET, 1), -16);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(9999, HEB_H_ELUL, 29), 2278650);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(5765, HEB_H_TISHREI, 1), 731840);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(5765, HEB_H_SHVAT, 1), 731957);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(5765, HEB_H_ADAR1, 1), 731987);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(5765, HEB_H_ADAR2, 1), 732017);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(5765, HEB_H_ADAR2, 22), 732038);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(5765, HEB_H_NISAN, 1), 732046);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(5706, HEB_H_KISLEV, 7), 710347);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(4682, HEB_H_NISAN, 15), 336499);

    /* TestRD2Hebrew */
    {
        char buf[64];
        heb_hdate_t hd = heb_hdate_from_rd(733359);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "15 Cheshvan 5769");
    }
    {
        char buf[64];
        heb_hdate_t hd = heb_hdate_from_rd(711262);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "6 Iyyar 5708");
    }
    {
        char buf[64];
        heb_hdate_t hd = heb_hdate_from_rd(249);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "1 Tishrei 3762");
    }
    {
        char buf[64];
        heb_hdate_t hd = heb_hdate_from_rd(72);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "1 Nisan 3761");
    }
    {
        char buf[64];
        heb_hdate_t hd = heb_hdate_from_rd(79);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "8 Nisan 3761");
    }
    {
        char buf[64];
        heb_hdate_t hd = heb_hdate_from_rd(1);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "18 Tevet 3761");
    }
    {
        char buf[64];
        heb_hdate_t hd = heb_hdate_from_rd(0);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "17 Tevet 3761");
    }
    {
        char buf[64];
        heb_hdate_t hd = heb_hdate_from_rd(-1);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "16 Tevet 3761");
    }
    {
        char buf[64];
        heb_hdate_t hd = heb_hdate_from_rd(-16);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "1 Tevet 3761");
    }
    {
        char buf[64];
        heb_hdate_t hd = heb_hdate_from_rd(-17);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "30 Kislev 3761");
    }
    {
        char buf[64];
        heb_hdate_t hd = heb_hdate_from_rd(2278650);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "29 Elul 9999");
    }
    {
        char buf[64];
        heb_hdate_t hd = heb_hdate_from_rd(732038);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "22 Adar II 5765");
    }

    /* TestMonthNames - via en_month_names table */
    {
        char buf[32];
        heb_hdate_t hd;
        heb_hdate_create(&hd, 5782, HEB_H_NISAN, 1);
        heb_hdate_month_name(&hd, HEB_LOCALE_EN, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Nisan");
        heb_hdate_create(&hd, 5782, HEB_H_TISHREI, 1);
        heb_hdate_month_name(&hd, HEB_LOCALE_EN, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Tishrei");
        heb_hdate_create(&hd, 5782, HEB_H_SHVAT, 1);
        heb_hdate_month_name(&hd, HEB_LOCALE_EN, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Sh'vat");
        heb_hdate_create(&hd, 5782, HEB_H_ADAR1, 1);
        heb_hdate_month_name(&hd, HEB_LOCALE_EN, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Adar I");
        heb_hdate_create(&hd, 5782, HEB_H_ADAR2, 1);
        heb_hdate_month_name(&hd, HEB_LOCALE_EN, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Adar II");
    }

    /* TestMonthNames2 (Hebrew) */
    {
        char buf[64];
        heb_hdate_t hd;
        heb_hdate_create(&hd, 5782, HEB_H_ADAR1, 15); /* leap year */
        heb_hdate_month_name(&hd, HEB_LOCALE_EN, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Adar I");
        heb_hdate_month_name(&hd, HEB_LOCALE_HE, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "\xd7\x90\xd6\xb7\xd7\x93\xd6\xb8\xd7\xa8 \xd7\x90\xd7\xb3");
        heb_hdate_month_name(&hd, HEB_LOCALE_HE_NONIKUD, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "\xd7\x90\xd7\x93\xd7\xa8 \xd7\x90\xd7\xb3");

        heb_hdate_create(&hd, 5783, HEB_H_ADAR1, 15); /* non-leap */
        heb_hdate_month_name(&hd, HEB_LOCALE_EN, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Adar");
        heb_hdate_month_name(&hd, HEB_LOCALE_HE, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "\xd7\x90\xd6\xb7\xd7\x93\xd6\xb8\xd7\xa8");
        heb_hdate_month_name(&hd, HEB_LOCALE_HE_NONIKUD, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "\xd7\x90\xd7\x93\xd7\xa8");
    }

    /* TestAdar2ResetToAdar1 */
    {
        heb_hdate_t hd;
        heb_hdate_create(&hd, 5782, HEB_H_ADAR1, 15);
        HEB_CHECK_INT_EQ(hd.month, HEB_H_ADAR1);
        heb_hdate_create(&hd, 5782, HEB_H_ADAR2, 15);
        HEB_CHECK_INT_EQ(hd.month, HEB_H_ADAR2);
        heb_hdate_create(&hd, 5783, HEB_H_ADAR1, 15);
        HEB_CHECK_INT_EQ(hd.month, HEB_H_ADAR1);
        heb_hdate_create(&hd, 5783, HEB_H_ADAR2, 15);
        HEB_CHECK_INT_EQ(hd.month, HEB_H_ADAR1); /* reset to Adar1 */
    }

    /* TestMonthFromName */
    {
        heb_hmonth_t m;
        struct { const char *s; heb_hmonth_t expected; } cases[] = {
            {"adar", HEB_H_ADAR2},
            {"Adar I", HEB_H_ADAR1},
            {"Adar II", HEB_H_ADAR2},
            {"Adar 1", HEB_H_ADAR1},
            {"Adar 2", HEB_H_ADAR2},
            {"Adar1", HEB_H_ADAR1},
            {"Adar2", HEB_H_ADAR2},
            {"Iyyar", HEB_H_IYYAR},
            {"Iyar", HEB_H_IYYAR},
            {"tammuz", HEB_H_TAMUZ},
            {"Si", HEB_H_SIVAN},
            {"Sh", HEB_H_SHVAT},
        };
        for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); i++) {
            HEB_CHECK_INT_EQ(heb_hdate_month_from_name(cases[i].s, &m), 0);
            HEB_CHECK_INT_EQ(m, cases[i].expected);
        }

        /* Hebrew month names */
        const char *he_cases[][2] = {
            {"\xd7\x90\xd7\x93\xd7\xa8 \xd7\x90", "\xd7\x90\xd7\x93\xd7\xa8 \xd7\x91"}, /* אדר א / אדר ב */
        };
        HEB_CHECK_INT_EQ(heb_hdate_month_from_name("\xd7\x90\xd7\x93\xd7\xa8 \xd7\x90", &m), 0);
        HEB_CHECK_INT_EQ(m, HEB_H_ADAR1);
        HEB_CHECK_INT_EQ(heb_hdate_month_from_name("\xd7\x90\xd7\x93\xd7\xa8 \xd7\x91", &m), 0);
        HEB_CHECK_INT_EQ(m, HEB_H_ADAR2);
        HEB_CHECK_INT_EQ(heb_hdate_month_from_name("\xd7\x90\xd7\x93\xd7\xa8 \xd7\x90\xd7\xb3", &m), 0);
        HEB_CHECK_INT_EQ(m, HEB_H_ADAR1);
        HEB_CHECK_INT_EQ(heb_hdate_month_from_name("\xd7\x90\xd7\x93\xd7\xa8 \xd7\x91\xd7\xb3", &m), 0);
        HEB_CHECK_INT_EQ(m, HEB_H_ADAR2);
        HEB_CHECK_INT_EQ(heb_hdate_month_from_name("\xd7\x90\xd7\x93\xd7\xa8", &m), 0);
        HEB_CHECK_INT_EQ(m, HEB_H_ADAR2);
        (void)he_cases;
    }

    /* TestMonthFromNameEmpty */
    {
        heb_hmonth_t m;
        HEB_CHECK_INT_EQ(heb_hdate_month_from_name("", &m), 1);
    }

    /* TestMonthFromName1Char */
    {
        heb_hmonth_t m;
        HEB_CHECK_INT_EQ(heb_hdate_month_from_name("i", &m), 0);
        HEB_CHECK_INT_EQ(m, HEB_H_IYYAR);
        HEB_CHECK_INT_EQ(heb_hdate_month_from_name("S", &m), 1); /* single S fails */
    }

    /* TestDaysInHebYear */
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(5779), 385);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(5780), 355);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(5781), 353);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(5782), 384);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(5783), 355);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(5784), 383);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(5785), 355);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(5786), 354);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(5787), 385);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(5788), 355);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(5789), 354);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(3762), 383);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(3671), 354);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(1234), 353);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(123), 355);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(2), 355);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(1), 355);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(5761), 353);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(5762), 354);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(5763), 385);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(5764), 355);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(5765), 383);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_year(5766), 354);

    /* TestDaysInMonth */
    HEB_CHECK_INT_EQ(heb_hdate_days_in_month(HEB_H_IYYAR, 5780), 29);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_month(HEB_H_SIVAN, 5780), 30);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_month(HEB_H_CHESHVAN, 5782), 29);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_month(HEB_H_CHESHVAN, 5783), 30);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_month(HEB_H_KISLEV, 5783), 30);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_month(HEB_H_KISLEV, 5784), 29);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_month(HEB_H_TISHREI, 5765), 30);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_month(HEB_H_CHESHVAN, 5765), 29);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_month(HEB_H_KISLEV, 5765), 29);
    HEB_CHECK_INT_EQ(heb_hdate_days_in_month(HEB_H_TEVET, 5765), 29);

    /* TestWeekday */
    {
        heb_hdate_t hd;
        heb_hdate_create(&hd, 5769, HEB_H_CHESHVAN, 15);
        HEB_CHECK_INT_EQ(heb_hdate_weekday(&hd), HEB_THURSDAY);
        heb_hdate_create(&hd, 5708, HEB_H_IYYAR, 6);
        HEB_CHECK_INT_EQ(heb_hdate_weekday(&hd), HEB_SATURDAY);
        heb_hdate_create(&hd, 5708, HEB_H_IYYAR, 7);
        HEB_CHECK_INT_EQ(heb_hdate_weekday(&hd), HEB_SUNDAY);
        heb_hdate_create(&hd, 3762, HEB_H_TISHREI, 1);
        HEB_CHECK_INT_EQ(heb_hdate_weekday(&hd), HEB_THURSDAY);
        heb_hdate_create(&hd, 3761, HEB_H_NISAN, 1);
        HEB_CHECK_INT_EQ(heb_hdate_weekday(&hd), HEB_TUESDAY);
        heb_hdate_create(&hd, 3761, HEB_H_TEVET, 18);
        HEB_CHECK_INT_EQ(heb_hdate_weekday(&hd), HEB_MONDAY);
        heb_hdate_create(&hd, 3761, HEB_H_TEVET, 17);
        HEB_CHECK_INT_EQ(heb_hdate_weekday(&hd), HEB_SUNDAY);
        heb_hdate_create(&hd, 3761, HEB_H_TEVET, 16);
        HEB_CHECK_INT_EQ(heb_hdate_weekday(&hd), HEB_SATURDAY);
        heb_hdate_create(&hd, 3761, HEB_H_TEVET, 1);
        HEB_CHECK_INT_EQ(heb_hdate_weekday(&hd), HEB_FRIDAY);
        heb_hdate_create(&hd, 3333, HEB_H_SIVAN, 29);
        HEB_CHECK_INT_EQ(heb_hdate_weekday(&hd), HEB_TUESDAY);
        heb_hdate_create(&hd, 3333, HEB_H_SIVAN, 23);
        HEB_CHECK_INT_EQ(heb_hdate_weekday(&hd), HEB_WEDNESDAY);
    }

    /* TestBefore/OnOrBefore/Nearest/OnOrAfter/After */
    {
        heb_hdate_t hd, result;
        char buf[32];
        /* TestBefore: 2014-02-19 is 15 Adar1 5774 */
        hd = heb_hdate_from_gregorian(2014, HEB_FEBRUARY, 19);
        result = heb_hdate_before(&hd, HEB_SATURDAY);
        hd2iso(&result, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "2014-02-15");

        /* TestOnOrBefore */
        hd = heb_hdate_from_gregorian(2014, HEB_FEBRUARY, 19);
        result = heb_hdate_on_or_before(&hd, HEB_SATURDAY);
        hd2iso(&result, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "2014-02-15");
        hd = heb_hdate_from_gregorian(2014, HEB_FEBRUARY, 22);
        result = heb_hdate_on_or_before(&hd, HEB_SATURDAY);
        hd2iso(&result, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "2014-02-22");
        hd = heb_hdate_from_gregorian(2014, HEB_FEBRUARY, 23);
        result = heb_hdate_on_or_before(&hd, HEB_SATURDAY);
        hd2iso(&result, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "2014-02-22");

        /* TestNearest */
        hd = heb_hdate_from_gregorian(2014, HEB_FEBRUARY, 19);
        result = heb_hdate_nearest(&hd, HEB_SATURDAY);
        hd2iso(&result, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "2014-02-22");
        hd = heb_hdate_from_gregorian(2014, HEB_FEBRUARY, 18);
        result = heb_hdate_nearest(&hd, HEB_SATURDAY);
        hd2iso(&result, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "2014-02-15");

        /* TestOnOrAfter */
        hd = heb_hdate_from_gregorian(2014, HEB_FEBRUARY, 19);
        result = heb_hdate_on_or_after(&hd, HEB_SATURDAY);
        hd2iso(&result, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "2014-02-22");
        hd = heb_hdate_from_gregorian(2014, HEB_FEBRUARY, 22);
        result = heb_hdate_on_or_after(&hd, HEB_SATURDAY);
        hd2iso(&result, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "2014-02-22");
        hd = heb_hdate_from_gregorian(2014, HEB_FEBRUARY, 23);
        result = heb_hdate_on_or_after(&hd, HEB_SATURDAY);
        hd2iso(&result, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "2014-03-01");

        /* TestAfter */
        hd = heb_hdate_from_gregorian(2014, HEB_FEBRUARY, 19);
        result = heb_hdate_after(&hd, HEB_SATURDAY);
        hd2iso(&result, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "2014-02-22");
        hd = heb_hdate_from_gregorian(2014, HEB_FEBRUARY, 22);
        result = heb_hdate_after(&hd, HEB_SATURDAY);
        hd2iso(&result, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "2014-03-01");
        hd = heb_hdate_from_gregorian(2014, HEB_FEBRUARY, 23);
        result = heb_hdate_after(&hd, HEB_SATURDAY);
        hd2iso(&result, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "2014-03-01");
    }

    /* TestToString */
    {
        char buf[64];
        heb_hdate_t hd;
        heb_hdate_create(&hd, 5511, HEB_H_TEVET, 4);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "4 Tevet 5511");
        heb_hdate_create(&hd, 5782, HEB_H_ELUL, 4);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "4 Elul 5782");
        heb_hdate_create(&hd, 5749, HEB_H_ADAR2, 29);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "29 Adar II 5749");
    }

    /* TestGreg */
    {
        heb_hdate_t hd;
        heb_gdate_t gd;
        heb_hdate_create(&hd, 5765, HEB_H_ADAR2, 22);
        gd = heb_hdate_greg(&hd);
        HEB_CHECK_INT_EQ(gd.year, 2005);
        HEB_CHECK_INT_EQ(gd.month, HEB_APRIL);
        HEB_CHECK_INT_EQ(gd.day, 2);

        heb_hdate_create(&hd, 5513, HEB_H_TISHREI, 6);
        gd = heb_hdate_greg(&hd);
        HEB_CHECK_INT_EQ(gd.year, 1752);
        HEB_CHECK_INT_EQ(gd.month, HEB_SEPTEMBER);
        HEB_CHECK_INT_EQ(gd.day, 14);

        heb_hdate_create(&hd, 5513, HEB_H_TISHREI, 5);
        gd = heb_hdate_greg(&hd);
        HEB_CHECK_INT_EQ(gd.year, 1752);
        HEB_CHECK_INT_EQ(gd.month, HEB_SEPTEMBER);
        HEB_CHECK_INT_EQ(gd.day, 2);
    }

    /* TestProlepticGreg */
    {
        heb_hdate_t hd;
        heb_gdate_t gd;
        heb_hdate_create(&hd, 5765, HEB_H_ADAR2, 22);
        gd = heb_hdate_proleptic_greg(&hd);
        HEB_CHECK_INT_EQ(gd.year, 2005);
        HEB_CHECK_INT_EQ(gd.month, HEB_APRIL);
        HEB_CHECK_INT_EQ(gd.day, 2);

        heb_hdate_create(&hd, 5513, HEB_H_TISHREI, 6);
        gd = heb_hdate_proleptic_greg(&hd);
        HEB_CHECK_INT_EQ(gd.year, 1752);
        HEB_CHECK_INT_EQ(gd.month, HEB_SEPTEMBER);
        HEB_CHECK_INT_EQ(gd.day, 14);

        heb_hdate_create(&hd, 5513, HEB_H_TISHREI, 5);
        gd = heb_hdate_proleptic_greg(&hd);
        HEB_CHECK_INT_EQ(gd.year, 1752);
        HEB_CHECK_INT_EQ(gd.month, HEB_SEPTEMBER);
        HEB_CHECK_INT_EQ(gd.day, 13);
    }

    /* TestFromGregorian */
    {
        char buf[64];
        heb_hdate_t hd;
        hd = heb_hdate_from_gregorian(2008, HEB_NOVEMBER, 13);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "15 Cheshvan 5769");
        hd = heb_hdate_from_gregorian(1752, HEB_SEPTEMBER, 14);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "6 Tishrei 5513");
        hd = heb_hdate_from_gregorian(1752, HEB_SEPTEMBER, 2);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "5 Tishrei 5513");
    }

    /* TestFromProlepticGregorian */
    {
        char buf[64];
        heb_hdate_t hd;
        hd = heb_hdate_from_proleptic_gregorian(2008, HEB_NOVEMBER, 13);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "15 Cheshvan 5769");
        hd = heb_hdate_from_proleptic_gregorian(1752, HEB_SEPTEMBER, 14);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "6 Tishrei 5513");
        hd = heb_hdate_from_proleptic_gregorian(1752, HEB_SEPTEMBER, 13);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "5 Tishrei 5513");
        hd = heb_hdate_from_proleptic_gregorian(1752, HEB_SEPTEMBER, 9);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "1 Tishrei 5513");
        hd = heb_hdate_from_proleptic_gregorian(1752, HEB_SEPTEMBER, 2);
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "23 Elul 5512");
    }

    /* TestAbsOnValue */
    {
        heb_hdate_t hd;
        heb_hdate_create(&hd, 5769, HEB_H_CHESHVAN, 15);
        HEB_CHECK_INT64_EQ(hd.abs, 733359);
        heb_hdate_t hd2 = heb_hdate_from_rd(733359);
        HEB_CHECK_INT64_EQ(hd2.abs, hd.abs);
    }

    /* TestElapsedDays: elapsedDays(year) = ToRD(year, Tishrei, 1) - Epoch */
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(5780, HEB_H_TISHREI, 1) - HEB_HDATE_EPOCH, 2110760);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(5708, HEB_H_TISHREI, 1) - HEB_HDATE_EPOCH, 2084447);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(3762, HEB_H_TISHREI, 1) - HEB_HDATE_EPOCH, 1373677);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(3671, HEB_H_TISHREI, 1) - HEB_HDATE_EPOCH, 1340455);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(1234, HEB_H_TISHREI, 1) - HEB_HDATE_EPOCH, 450344);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(123, HEB_H_TISHREI, 1) - HEB_HDATE_EPOCH, 44563);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(2, HEB_H_TISHREI, 1) - HEB_HDATE_EPOCH, 356);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(1, HEB_H_TISHREI, 1) - HEB_HDATE_EPOCH, 1);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(5762, HEB_H_TISHREI, 1) - HEB_HDATE_EPOCH, 2104174);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(5763, HEB_H_TISHREI, 1) - HEB_HDATE_EPOCH, 2104528);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(5764, HEB_H_TISHREI, 1) - HEB_HDATE_EPOCH, 2104913);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(5765, HEB_H_TISHREI, 1) - HEB_HDATE_EPOCH, 2105268);
    HEB_CHECK_INT64_EQ(heb_hdate_to_rd(5766, HEB_H_TISHREI, 1) - HEB_HDATE_EPOCH, 2105651);

    return hebtest_failures;
}
