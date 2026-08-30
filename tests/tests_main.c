/*
 * tests_main.c - test runner for hebcal-c.
 * Calls all test_* functions and reports total pass/fail.
 * License: GPL-2.0-or-later
 */
#include "hebtest.h"

/* Declared in each test_*.c file: */
HEBTEST_FUNC(test_greg);
HEBTEST_FUNC(test_hdate);
HEBTEST_FUNC(test_anniversary);
HEBTEST_FUNC(test_gematriya);
HEBTEST_FUNC(test_molad);
HEBTEST_FUNC(test_sedra);
HEBTEST_FUNC(test_omer);
HEBTEST_FUNC(test_event);
HEBTEST_FUNC(test_noaa);
HEBTEST_FUNC(test_zmanim);
HEBTEST_FUNC(test_holidays);

HEBTEST_INIT();

int main(void) {
    int total_failures = 0;
    int total_checks = 0;

    struct {
        const char *name;
        hebtest_fn fn;
    } tests[] = {
        { "test_greg",  test_greg },
        { "test_hdate", test_hdate },
        { "test_anniversary", test_anniversary },
        { "test_gematriya", test_gematriya },
        { "test_molad", test_molad },
        { "test_sedra", test_sedra },
        { "test_omer", test_omer },
        { "test_event", test_event },
        { "test_noaa", test_noaa },
        { "test_zmanim", test_zmanim },
        { "test_holidays", test_holidays },
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        int before = hebtest_failures;
        int checks_before = hebtest_checks;
        printf("[ RUN      ] %s\n", tests[i].name);
        tests[i].fn();
        int fails = hebtest_failures - before;
        int checks = hebtest_checks - checks_before;
        if (fails == 0) {
            printf("[       OK ] %s (%d checks)\n", tests[i].name, checks);
        } else {
            printf("[  FAILED  ] %s (%d/%d checks failed)\n", tests[i].name, fails, checks);
        }
        total_failures += fails;
        total_checks += checks;
    }

    printf("\n");
    if (total_failures == 0) {
        printf("[  PASSED  ] All %d checks passed.\n", total_checks);
        return 0;
    } else {
        printf("[  FAILED  ] %d of %d checks failed.\n", total_failures, total_checks);
        return 1;
    }
}
