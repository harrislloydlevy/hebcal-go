/*
 * test_event.c - 1:1 port of event_test.go categories/basename tests to C.
 * License: GPL-2.0-or-later
 */
#include "hebtest.h"
#include "event.h"
#include "hdate.h"
#include <string.h>

HEBTEST_FUNC(test_event) {
    /* Test flagToCategory mapping */
    HEB_CHECK_STR_EQ(heb_flags_to_category(HEB_F_MAJOR_FAST), "fast");
    HEB_CHECK_STR_EQ(heb_flags_to_category(HEB_F_CHANUKAH_CANDLES), "minor");
    HEB_CHECK_STR_EQ(heb_flags_to_category(HEB_F_OMER_COUNT), "omer");
    HEB_CHECK_STR_EQ(heb_flags_to_category(HEB_F_PARSHA_HASHAVUA), "parashat");
    HEB_CHECK_STR_EQ(heb_flags_to_category(HEB_F_ROSH_CHODESH), "roshchodesh");
    HEB_CHECK_STR_EQ(heb_flags_to_category(HEB_F_MODERN_HOLIDAY), "modern");
    HEB_CHECK_STR_EQ(heb_flags_to_category(0), "unknown");

    /* Test Basename */
    {
        heb_event_t ev;
        memset(&ev, 0, sizeof(ev));
        char buf[64];

        ev.desc = "Sukkot III (CH''M)";
        heb_event_basename(&ev, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Sukkot");

        ev.desc = "Chanukah: 3 Candles";
        heb_event_basename(&ev, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Chanukah");

        ev.desc = "Sukkot VII (Hoshana Raba)";
        heb_event_basename(&ev, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Sukkot");

        ev.desc = "Erev Pesach";
        heb_event_basename(&ev, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Pesach");

        ev.desc = "Pesach Sheni";
        heb_event_basename(&ev, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Pesach Sheni");

        ev.desc = "Sukkot II";
        heb_event_basename(&ev, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Sukkot");

        ev.desc = "Rosh Hashana 5784";
        heb_event_basename(&ev, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, "Rosh Hashana");
    }

    /* Test minorHolidays / category */
    {
        heb_event_t ev;
        memset(&ev, 0, sizeof(ev));

        ev.desc = "Lag BaOmer";
        HEB_CHECK_STR_EQ(heb_event_category(&ev), "minor");

        ev.desc = "Pesach";
        ev.flags = HEB_F_CHAG;
        HEB_CHECK_STR_EQ(heb_event_category(&ev), "major");

        ev.desc = "Yom Kippur";
        ev.flags = HEB_F_MAJOR_FAST;
        HEB_CHECK_STR_EQ(heb_event_category(&ev), "fast");
    }

    return hebtest_failures;
}
