/*
 * test_sedra.c - 1:1 port of sedra_test.go to C.
 * License: GPL-2.0-or-later
 */
#include "hebtest.h"
#include "sedra.h"
#include "hdate.h"

HEBTEST_FUNC(test_sedra) {
    /* TestSedra_Lookup */
    {
        heb_sedra_t s = heb_sedra_new(5749, 0);

        /* Oct 1, 1988 - Chag */
        heb_hdate_t hd = heb_hdate_from_gregorian(1988, HEB_OCTOBER, 1);
        heb_parsha_t p = heb_sedra_lookup(&s, &hd);
        HEB_CHECK_INT_EQ(p.chag, 1);

        /* Nov 5, 1988 - Chayei Sara */
        hd = heb_hdate_from_gregorian(1988, HEB_NOVEMBER, 5);
        p = heb_sedra_lookup(&s, &hd);
        HEB_CHECK_INT_EQ(p.chag, 0);
        HEB_CHECK_STR_EQ(p.name1, "Chayei Sara");
        HEB_CHECK_INT_EQ(p.num1, 5);
        HEB_CHECK(p.name2 == NULL);

        /* Jul 15, 1989 - Chukat-Balak */
        hd = heb_hdate_from_gregorian(1989, HEB_JULY, 15);
        p = heb_sedra_lookup(&s, &hd);
        HEB_CHECK_INT_EQ(p.chag, 0);
        HEB_CHECK_STR_EQ(p.name1, "Chukat");
        HEB_CHECK_STR_EQ(p.name2, "Balak");
        HEB_CHECK_INT_EQ(p.num1, 39);
        HEB_CHECK_INT_EQ(p.num2, 40);
    }

    /* 5781 tests */
    {
        heb_sedra_t s = heb_sedra_new(5781, 0);

        /* Apr 24, 2021 - Achrei Mot-Kedoshim */
        heb_hdate_t hd = heb_hdate_from_gregorian(2021, HEB_APRIL, 24);
        heb_parsha_t p = heb_sedra_lookup(&s, &hd);
        HEB_CHECK_INT_EQ(p.chag, 0);
        HEB_CHECK_STR_EQ(p.name1, "Achrei Mot");
        HEB_CHECK_STR_EQ(p.name2, "Kedoshim");
        HEB_CHECK_INT_EQ(p.num1, 29);
        HEB_CHECK_INT_EQ(p.num2, 30);

        /* Oct 17, 2020 - Bereshit */
        hd = heb_hdate_from_gregorian(2020, HEB_OCTOBER, 17);
        p = heb_sedra_lookup(&s, &hd);
        HEB_CHECK_INT_EQ(p.chag, 0);
        HEB_CHECK_STR_EQ(p.name1, "Bereshit");
        HEB_CHECK_INT_EQ(p.num1, 1);
        HEB_CHECK(p.name2 == NULL);
    }

    /* ExampleSedra_FindParshaNum: parsha 16 → 15 Sh'vat 5749 */
    {
        heb_sedra_t s = heb_sedra_new(5749, 0);
        heb_hdate_t hd;
        int rc = heb_sedra_find_parsha_num(&s, 16, &hd);
        HEB_CHECK_INT_EQ(rc, 0);
        char buf[64];
        heb_hdate_to_string(&hd, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "15 Sh'vat 5749");
    }

    /* TestFindParshaNumErrors */
    {
        heb_sedra_t s = heb_sedra_new(5784, 0);
        heb_hdate_t hd;
        /* 42 and 43 are doubled in Diaspora */
        HEB_CHECK_INT_EQ(heb_sedra_find_parsha_num(&s, 42, &hd), 1);
        HEB_CHECK_INT_EQ(heb_sedra_find_parsha_num(&s, 43, &hd), 1);
        /* Beshalach (16) is always found */
        HEB_CHECK_INT_EQ(heb_sedra_find_parsha_num(&s, 16, &hd), 0);
        HEB_CHECK_INT_EQ(hd.year, 5784);
        /* Out of range */
        HEB_CHECK_INT_EQ(heb_sedra_find_parsha_num(&s, 0, &hd), 1);
        HEB_CHECK_INT_EQ(heb_sedra_find_parsha_num(&s, 55, &hd), 1);
        HEB_CHECK_INT_EQ(heb_sedra_find_parsha_num(&s, 100, &hd), 1);
    }

    /* TestSedraYearTypes */
    {
        int years[] = {5701, 5702, 5703, 5708, 5710, 5711, 5713, 5714,
                        5715, 5717, 5719, 5726, 5734, 5736};
        for (size_t i = 0; i < sizeof(years)/sizeof(years[0]); i++) {
            heb_hdate_t hd;
            heb_sedra_t diaspora = heb_sedra_new(years[i], 0);
            HEB_CHECK_INT_EQ(heb_sedra_find_parsha_num(&diaspora, 1, &hd), 0);
            HEB_CHECK_INT_EQ(hd.year, years[i]);
            heb_sedra_t il = heb_sedra_new(years[i], 1);
            HEB_CHECK_INT_EQ(heb_sedra_find_parsha_num(&il, 1, &hd), 0);
            HEB_CHECK_INT_EQ(hd.year, years[i]);
        }
    }

    /* TestSedraEarlyYears */
    {
        int years[] = {3762, 3761, 3760, 3759, 100, 2, 1};
        for (size_t i = 0; i < sizeof(years)/sizeof(years[0]); i++) {
            heb_sedra_t s = heb_sedra_new(years[i], 0);
            HEB_CHECK_INT_EQ(s.year, years[i]);
        }
    }

    return hebtest_failures;
}
