/*
 * test_greg.c - 1:1 port of greg_test.go (Go) to C.
 * License: GPL-2.0-or-later
 */
#include "hebtest.h"
#include "greg.h"

HEBTEST_FUNC(test_greg) {
    int err;

    /* TestGreg2RD */
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(1995, HEB_DECEMBER, 17, &err), 728644);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(1888, HEB_DECEMBER, 31, &err), 689578);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(2005, HEB_APRIL, 2, &err), 732038);

    /* TestGreg2RDEarlyCE */
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(88, HEB_DECEMBER, 30, &err), 32139);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(1, HEB_JANUARY, 1, &err), -1);

    /* TestGreg2RDNegative */
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(1, HEB_JANUARY, 2, &err), 0);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(1, HEB_JANUARY, 1, &err), -1);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-1, HEB_DECEMBER, 31, &err), -2);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-1, HEB_DECEMBER, 30, &err), -3);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-1, HEB_DECEMBER, 29, &err), -4);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-1, HEB_NOVEMBER, 13, &err), -50);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-1, HEB_OCTOBER, 31, &err), -63);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-1, HEB_OCTOBER, 1, &err), -93);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-1, HEB_MARCH, 1, &err), -307);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-1, HEB_FEBRUARY, 29, &err), -308);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-1, HEB_FEBRUARY, 28, &err), -309);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-1, HEB_FEBRUARY, 27, &err), -310);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-1, HEB_FEBRUARY, 1, &err), -336);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-1, HEB_JANUARY, 31, &err), -337);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-1, HEB_JANUARY, 16, &err), -352);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-1, HEB_JANUARY, 1, &err), -367);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-2, HEB_DECEMBER, 31, &err), -368);

    /* TestGreg2RDNegative2 */
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-2, HEB_DECEMBER, 1, &err), -398);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-2, HEB_JANUARY, 1, &err), -732);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-3, HEB_JANUARY, 1, &err), -1097);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-4, HEB_JANUARY, 1, &err), -1462);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-100, HEB_DECEMBER, 20, &err), -36173);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-1000, HEB_JUNE, 15, &err), -365086);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-101, HEB_DECEMBER, 20, &err), -36538);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(-101, HEB_JANUARY, 1, &err), -36892);

    /* TestRD2Greg */
    {
        heb_gdate_t gd = heb_greg_from_rd(737553);
        HEB_CHECK_INT_EQ(gd.year, 2020);
        HEB_CHECK_INT_EQ(gd.month, HEB_MAY);
        HEB_CHECK_INT_EQ(gd.day, 8);
    }
    {
        heb_gdate_t gd = heb_greg_from_rd(689578);
        HEB_CHECK_INT_EQ(gd.year, 1888);
        HEB_CHECK_INT_EQ(gd.month, HEB_DECEMBER);
        HEB_CHECK_INT_EQ(gd.day, 31);
    }
    {
        heb_gdate_t gd = heb_greg_from_rd(732038);
        HEB_CHECK_INT_EQ(gd.year, 2005);
        HEB_CHECK_INT_EQ(gd.month, HEB_APRIL);
        HEB_CHECK_INT_EQ(gd.day, 2);
    }

    /* TestRD2Greg88ce */
    {
        heb_gdate_t gd = heb_greg_from_rd(32139);
        HEB_CHECK_INT_EQ(gd.year, 88);
        HEB_CHECK_INT_EQ(gd.month, HEB_DECEMBER);
        HEB_CHECK_INT_EQ(gd.day, 30);
    }
    {
        heb_gdate_t gd = heb_greg_from_rd(32140);
        HEB_CHECK_INT_EQ(gd.year, 88);
        HEB_CHECK_INT_EQ(gd.month, HEB_DECEMBER);
        HEB_CHECK_INT_EQ(gd.day, 31);
    }
    {
        heb_gdate_t gd = heb_greg_from_rd(32141);
        HEB_CHECK_INT_EQ(gd.year, 89);
        HEB_CHECK_INT_EQ(gd.month, HEB_JANUARY);
        HEB_CHECK_INT_EQ(gd.day, 1);
    }

    /* TestRD2GregAtTransition */
    {
        heb_gdate_t gd = heb_greg_from_rd(639796);
        HEB_CHECK_INT_EQ(gd.year, 1752);
        HEB_CHECK_INT_EQ(gd.month, HEB_SEPTEMBER);
        HEB_CHECK_INT_EQ(gd.day, 2);
    }
    {
        heb_gdate_t gd = heb_greg_from_rd(639797);
        HEB_CHECK_INT_EQ(gd.year, 1752);
        HEB_CHECK_INT_EQ(gd.month, HEB_SEPTEMBER);
        HEB_CHECK_INT_EQ(gd.day, 14);
    }

    /* TestGreg2RDAtTransition */
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(1752, HEB_SEPTEMBER, 2, &err), 639796);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(1752, HEB_SEPTEMBER, 14, &err), 639797);
    HEB_CHECK_INT64_EQ(heb_greg_to_rd(1752, HEB_SEPTEMBER, 15, &err), 639798);

    /* TestRD2Greg1ce */
    {
        heb_gdate_t gd = heb_greg_from_rd(-1);
        HEB_CHECK_INT_EQ(gd.year, 1);
        HEB_CHECK_INT_EQ(gd.month, HEB_JANUARY);
        HEB_CHECK_INT_EQ(gd.day, 1);
    }
    {
        heb_gdate_t gd = heb_greg_from_rd(0);
        HEB_CHECK_INT_EQ(gd.year, 1);
        HEB_CHECK_INT_EQ(gd.month, HEB_JANUARY);
        HEB_CHECK_INT_EQ(gd.day, 2);
    }
    {
        heb_gdate_t gd = heb_greg_from_rd(1);
        HEB_CHECK_INT_EQ(gd.year, 1);
        HEB_CHECK_INT_EQ(gd.month, HEB_JANUARY);
        HEB_CHECK_INT_EQ(gd.day, 3);
    }

    /* TestRD2GregNegative */
    {
        heb_gdate_t gd = heb_greg_from_rd(-732);
        HEB_CHECK_INT_EQ(gd.year, -2);
        HEB_CHECK_INT_EQ(gd.month, HEB_JANUARY);
        HEB_CHECK_INT_EQ(gd.day, 1);
    }
    {
        heb_gdate_t gd = heb_greg_from_rd(-36538);
        HEB_CHECK_INT_EQ(gd.year, -101);
        HEB_CHECK_INT_EQ(gd.month, HEB_DECEMBER);
        HEB_CHECK_INT_EQ(gd.day, 20);
    }
    {
        heb_gdate_t gd = heb_greg_from_rd(-2);
        HEB_CHECK_INT_EQ(gd.year, -1);
        HEB_CHECK_INT_EQ(gd.month, HEB_DECEMBER);
        HEB_CHECK_INT_EQ(gd.day, 31);
    }
    {
        heb_gdate_t gd = heb_greg_from_rd(-367);
        HEB_CHECK_INT_EQ(gd.year, -1);
        HEB_CHECK_INT_EQ(gd.month, HEB_JANUARY);
        HEB_CHECK_INT_EQ(gd.day, 1);
    }
    {
        heb_gdate_t gd = heb_greg_from_rd(-368);
        HEB_CHECK_INT_EQ(gd.year, -2);
        HEB_CHECK_INT_EQ(gd.month, HEB_DECEMBER);
        HEB_CHECK_INT_EQ(gd.day, 31);
    }

    /* Examples */
    HEB_CHECK_INT_EQ(heb_greg_days_in(HEB_FEBRUARY, 2004), 29);
    HEB_CHECK_INT_EQ(heb_greg_is_leap_year(2024), 1);
    HEB_CHECK_INT_EQ(heb_greg_is_leap_year(2023), 0);
    HEB_CHECK_INT_EQ(heb_greg_is_leap_year(1900), 0);
    HEB_CHECK_INT_EQ(heb_greg_is_leap_year(1700), 1); /* Julian */

    /* Proleptic tests (from hdate_test.go TestProlepticGreg) */
    HEB_CHECK_INT64_EQ(heb_greg_proleptic_to_rd(2008, HEB_NOVEMBER, 13), 733359);
    HEB_CHECK_INT64_EQ(heb_greg_proleptic_to_rd(1752, HEB_SEPTEMBER, 14), 639797);
    HEB_CHECK_INT64_EQ(heb_greg_proleptic_to_rd(1752, HEB_SEPTEMBER, 13), 639796);
    HEB_CHECK_INT64_EQ(heb_greg_proleptic_to_rd(1752, HEB_SEPTEMBER, 9), 639792);
    HEB_CHECK_INT64_EQ(heb_greg_proleptic_to_rd(1752, HEB_SEPTEMBER, 2), 639785);

    {
        heb_gdate_t gd = heb_greg_proleptic_from_rd(733359);
        HEB_CHECK_INT_EQ(gd.year, 2008);
        HEB_CHECK_INT_EQ(gd.month, HEB_NOVEMBER);
        HEB_CHECK_INT_EQ(gd.day, 13);
    }
    {
        heb_gdate_t gd = heb_greg_proleptic_from_rd(639796);
        HEB_CHECK_INT_EQ(gd.year, 1752);
        HEB_CHECK_INT_EQ(gd.month, HEB_SEPTEMBER);
        HEB_CHECK_INT_EQ(gd.day, 13);
    }

    return hebtest_failures;
}
