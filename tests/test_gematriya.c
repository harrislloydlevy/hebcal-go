/*
 * test_gematriya.c - 1:1 port of gematriya_test.go to C.
 * License: GPL-2.0-or-later
 */
#include "hebtest.h"
#include "gematriya.h"

/* Hebrew letters as UTF-8 escape sequences for portability. */
#define ALEF    "\xd7\x90"
#define BET     "\xd7\x91"
#define GIMEL   "\xd7\x92"
#define DALET   "\xd7\x93"
#define HEH     "\xd7\x94"
#define VAV     "\xd7\x95"
#define ZAYIN   "\xd7\x96"
#define CHET    "\xd7\x97"
#define TET     "\xd7\x98"
#define YUD     "\xd7\x99"
#define KAF     "\xd7\x9b"
#define LAMED   "\xd7\x9c"
#define MEM     "\xd7\x9e"
#define NUN     "\xd7\xa0"
#define SAMECH  "\xd7\xa1"
#define AYIN    "\xd7\xa2"
#define PEH     "\xd7\xa4"
#define TSADE   "\xd7\xa6"
#define KUF     "\xd7\xa7"
#define RESH    "\xd7\xa8"
#define SHIN    "\xd7\xa9"
#define TAV     "\xd7\xaa"

#define GERESH    "\xd7\xb3"
#define GERSHAYIM "\xd7\xb4"

HEBTEST_FUNC(test_gematriya) {
    struct { int number; const char *want; } tests[] = {
        {3,     GIMEL GERESH},
        {14,    YUD GERSHAYIM DALET},
        {15,    TET GERSHAYIM VAV},
        {16,    TET GERSHAYIM ZAYIN},
        {17,    YUD GERSHAYIM ZAYIN},
        {20,    KAF GERESH},
        {25,    KAF GERSHAYIM HEH},
        {60,    SAMECH GERESH},
        {115,   KUF TET GERSHAYIM VAV},
        {116,   KUF TET GERSHAYIM ZAYIN},
        {123,   KUF KAF GERSHAYIM GIMEL},
        {400,   TAV GERESH},
        {500,   TAV GERSHAYIM KUF},
        {613,   TAV RESH YUD GERSHAYIM GIMEL},
        {900,   TAV TAV GERSHAYIM KUF},
        {999,   TAV TAV KUF TSADE GERSHAYIM TET},
        {5749,  TAV SHIN MEM GERSHAYIM TET},
        {5774,  TAV SHIN AYIN GERSHAYIM DALET},
        {5780,  TAV SHIN GERSHAYIM PEH},
        {1123,  ALEF GERESH KUF KAF GERSHAYIM GIMEL},
        {3761,  GIMEL GERESH TAV SHIN SAMECH GERSHAYIM ALEF},
        {6749,  VAV GERESH TAV SHIN MEM GERSHAYIM TET},
        {8765,  CHET GERESH TAV SHIN SAMECH GERSHAYIM HEH},
        {16123, TET ZAYIN GERESH KUF KAF GERSHAYIM GIMEL},
        {22700, KAF BET GERESH TAV GERSHAYIM SHIN},
        {6000,  VAV GERESH},
        {7007,  ZAYIN GERESH ZAYIN GERESH},
        {5000,  ""},
        {0,     ""},
        {-1,    ""},
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        char buf[64];
        heb_gematriya(tests[i].number, buf, sizeof(buf));
        HEB_CHECK_STR_EQ(buf, tests[i].want);
    }

    return hebtest_failures;
}
