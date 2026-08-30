/*
 * test_molad.c - 1:1 port of molad_test.go ExampleNew to C.
 * License: GPL-2.0-or-later
 */
#include "hebtest.h"
#include "molad.h"
#include "hdate.h"

/* The Go test ExampleNew verifies:
 *   molad := molad.New(5783, hdate.Iyyar)
 *   dayOfWeek := molad.Date.Weekday().String()  -> "Thursday"
 *   molad.Minutes -> 8
 *   molad.Chalakim -> 13
 *   molad.Hours -> 14
 */
HEBTEST_FUNC(test_molad) {
    heb_molad_t m = heb_molad_new(5783, HEB_H_IYYAR);

    HEB_CHECK_INT_EQ(heb_hdate_weekday(&m.date), HEB_THURSDAY);
    HEB_CHECK_INT_EQ(m.minutes, 8);
    HEB_CHECK_INT_EQ(m.chalakim, 13);
    HEB_CHECK_INT_EQ(m.hours, 14);

    return hebtest_failures;
}
