/*
 * hebtest - minimal C test framework for hebcal-c.
 *
 * Provides ASSERT_* macros and a test runner pattern.  Each test_*.c file
 * defines a function that returns the number of failures; tests_main.c
 * calls them all and reports the total.
 *
 * License: GPL-2.0-or-later
 */
#ifndef HEBTEST_H
#define HEBTEST_H

#include <stdio.h>
#include <string.h>
#include <math.h>

extern int hebtest_failures;
extern int hebtest_checks;

#define HEBTEST_INIT()  int hebtest_failures = 0; int hebtest_checks = 0

#define HEB_CHECK(cond) do { \
    hebtest_checks++; \
    if (!(cond)) { \
        fprintf(stderr, "  FAIL: %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        hebtest_failures++; \
    } \
} while (0)

#define HEB_CHECK_INT64_EQ(actual, expected) do { \
    hebtest_checks++; \
    int64_t _a = (actual), _e = (expected); \
    if (_a != _e) { \
        fprintf(stderr, "  FAIL: %s:%d: got %lld, expected %lld\n", \
                __FILE__, __LINE__, (long long)_a, (long long)_e); \
        hebtest_failures++; \
    } \
} while (0)

#define HEB_CHECK_INT_EQ(actual, expected) do { \
    hebtest_checks++; \
    int _a = (actual), _e = (expected); \
    if (_a != _e) { \
        fprintf(stderr, "  FAIL: %s:%d: got %d, expected %d\n", \
                __FILE__, __LINE__, _a, _e); \
        hebtest_failures++; \
    } \
} while (0)

#define HEB_CHECK_STR_EQ(actual, expected) do { \
    hebtest_checks++; \
    const char *_a = (actual), *_e = (expected); \
    if (strcmp(_a, _e) !=  test_str_cmp(_a, _e)) { \
        fprintf(stderr, "  FAIL: %s:%d: got \"%s\", expected \"%s\"\n", \
                __FILE__, __LINE__, _a, _e); \
        hebtest_failures++; \
    } \
} while (0)

/* String comparison helper: returns 0 on match (like strcmp). */
static inline int test_str_cmp(const char *a, const char *b) {
    return strcmp(a, b);
}

/* Override the macro to use strcmp properly */
#undef HEB_CHECK_STR_EQ
#define HEB_CHECK_STR_EQ(actual, expected) do { \
    hebtest_checks++; \
    const char *_a = (actual), *_e = (expected); \
    if (!_a) _a = "(null)"; \
    if (!_e) _e = "(null)"; \
    if (strcmp(_a, _e) != 0) { \
        fprintf(stderr, "  FAIL: %s:%d: got \"%s\", expected \"%s\"\n", \
                __FILE__, __LINE__, _a, _e); \
        hebtest_failures++; \
    } \
} while (0)

#define HEB_CHECK_DOUBLE_NEAR(actual, expected, tol) do { \
    hebtest_checks++; \
    double _a = (actual), _e = (expected), _t = (tol); \
    if (fabs(_a - _e) > _t) { \
        fprintf(stderr, "  FAIL: %s:%d: got %g, expected %g (tol %g)\n", \
                __FILE__, __LINE__, _a, _e, _t); \
        hebtest_failures++; \
    } \
} while (0)

/* Each test file declares one of these. */
#define HEBTEST_FUNC(name) int name(void)

/* Function pointer type for a test function. */
typedef int (*hebtest_fn)(void);

#endif /* HEBTEST_H */
