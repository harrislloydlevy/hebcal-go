/*
 * hebcal-c - Hebrew gematriya (numbers as Hebrew letters).
 * Ported from github.com/hebcal/gematriya (Go).
 *
 * License: GPL-2.0-or-later
 */
#include "gematriya.h"
#include <string.h>

/* Hebrew letter codepoints as UTF-8 sequences. */
/* geresh U+05F3 = 0xD7 0xB3, gershayim U+05F4 = 0xD7 0xB4 */
#define GERESH    "\xd7\xb3"
#define GERSHAYIM "\xd7\xb4"
#define TAV       "\xd7\xaa"

/* units: 0(unused), א ב ג ד ה ו ז ח ט */
static const char *const units_utf8[10] = {
    "", "\xd7\x90", "\xd7\x91", "\xd7\x92", "\xd7\x93",
    "\xd7\x94", "\xd7\x95", "\xd7\x96", "\xd7\x97", "\xd7\x98"
};
/* tens: 0(unused), י כ ל מ נ ס ע פ צ */
static const char *const tens_utf8[10] = {
    "", "\xd7\x99", "\xd7\x9b", "\xd7\x9c", "\xd7\x9e",
    "\xd7\xa0", "\xd7\xa1", "\xd7\xa2", "\xd7\xa4", "\xd7\xa6"
};
/* hundreds: 0(unused), ק ר ש */
static const char *const hundreds_utf8[4] = {
    "", "\xd7\xa7", "\xd7\xa8", "\xd7\xa9"
};

/* Append a UTF-8 string to buf at position *pos. */
static void append_str(char *buf, size_t n, size_t *pos, const char *s) {
    size_t len = strlen(s);
    if (*pos + len < n) {
        memcpy(buf + *pos, s, len);
        *pos += len;
    }
}

/* Append the letters spelling num (1-999), most significant first. */
static void append_letters(char *buf, size_t n, size_t *pos, int num) {
    if (num <= 0) return;
    for (; num >= 400; num -= 400)
        append_str(buf, n, pos, TAV);
    {
        int h = num / 100;
        if (h != 0) {
            append_str(buf, n, pos, hundreds_utf8[h]);
            num %= 100;
        }
    }
    if (num == 15 || num == 16) {
        /* 15 = ט״ו, 16 = ט״ז (avoid spelling names of God) */
        append_str(buf, n, pos, units_utf8[9]);
        append_str(buf, n, pos, units_utf8[num - 9]);
        return;
    }
    {
        int t = num / 10;
        if (t != 0)
            append_str(buf, n, pos, tens_utf8[t]);
    }
    {
        int u = num % 10;
        if (u != 0)
            append_str(buf, n, pos, units_utf8[u]);
    }
}

int heb_gematriya(int number, char *buf, size_t n) {
    if (n == 0) return 0;
    buf[0] = '\0';
    size_t pos = 0;

    int thousands = number / 1000;
    if (thousands > 0 && thousands != 5) {
        append_letters(buf, n, &pos, thousands);
        append_str(buf, n, &pos, GERESH);
    }

    size_t start = pos;
    append_letters(buf, n, &pos, number % 1000);

    size_t count = pos - start;
    if (count == 0) {
        /* nothing below the thousands (which already carries a geresh) */
    } else if (count <= 2) {
        /* 1 letter: append geresh; 2+ bytes: it's a single letter */
        append_str(buf, n, &pos, GERESH);
    } else {
        /* Multiple letters: insert gershayim before the final letter.
         * We need to shift the last letter (1-2 bytes) right by 2 bytes
         * (gershayim). But since all Hebrew letters are 2 bytes in UTF-8,
         * the last letter is exactly 2 bytes. */
        if (pos >= 2 && n > pos + 2) {
            /* Move last 2 bytes right by 2, insert gershayim */
            memmove(buf + pos - 2 + 2, buf + pos - 2, 2);
            memcpy(buf + pos - 2, GERSHAYIM, 2);
            pos += 2;
        }
    }

    if (pos < n) buf[pos] = '\0';
    else buf[n - 1] = '\0';
    return (int)pos;
}
