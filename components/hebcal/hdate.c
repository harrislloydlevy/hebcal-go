/*
 * hebcal-c - Hebrew date conversions.
 * Ported from github.com/hebcal/hdate (Go).
 *
 * License: GPL-2.0-or-later
 */
#include "hdate.h"
#include "greg.h"
#include "tables_data.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

/* === PROGMEM table accessors === */

static int8_t tab_days_in_month(int t, int m) {
#if defined(__AVR__)
    return pgm_read_byte(&heb_days_in_month_tab[t][m]);
#else
    return heb_days_in_month_tab[t][m];
#endif
}

static int16_t tab_month_offset(int t, int m) {
#if defined(__AVR__)
    return pgm_read_word(&heb_month_offset_tab[t][m]);
#else
    return heb_month_offset_tab[t][m];
#endif
}

static int8_t tab_month_by_day(int t, int d) {
#if defined(__AVR__)
    return pgm_read_byte(&heb_month_by_day_of_year[t][d]);
#else
    return heb_month_by_day_of_year[t][d];
#endif
}

/* === Year-type helpers === */

static int year_type(int daysInYear) {
    int t = daysInYear % 10 - 3; /* 353->0, 354->1, 355->2 */
    if (daysInYear > 355) {
        t += 3; /* leap years shift by 3 */
    }
    return t;
}

static int year_len_of_type(int t) {
    return 353 + t % 3 + 30 * (t / 3);
}

/* daysInMonth for a year-type t and month m.  Out-of-range months report 30. */
static int dim_type(int t, heb_hmonth_t m) {
    if (m < HEB_H_NISAN || m > HEB_H_ADAR2) {
        return 30;
    }
    return tab_days_in_month(t, m);
}

/* monthOffset for a year-type t and month m.  Falls back to summing for
 * out-of-range indices. */
static int64_t month_offset(int t, heb_hmonth_t m) {
    if (m < 0 || m > HEB_H_ADAR2 + 1) {
        int64_t offset = 0;
        if (m < HEB_H_TISHREI) {
            int lastMonth = HEB_H_ADAR1;
            if (year_len_of_type(t) > 355) lastMonth = HEB_H_ADAR2;
            for (int x = HEB_H_TISHREI; x <= lastMonth; x++)
                offset += dim_type(t, (heb_hmonth_t)x);
            for (int x = HEB_H_NISAN; x < (int)m; x++)
                offset += dim_type(t, (heb_hmonth_t)x);
        } else {
            for (int x = HEB_H_TISHREI; x < (int)m; x++)
                offset += dim_type(t, (heb_hmonth_t)x);
        }
        return offset;
    }
    return (int64_t)tab_month_offset(t, m);
}

/* === Core calendar math === */

int heb_hdate_is_leap_year(int year) {
    return (1 + year * 7) % 19 < 7;
}

int heb_hdate_months_in_year(int year) {
    return heb_hdate_is_leap_year(year) ? 13 : 12;
}

/* elapsedDays0: days from Sunday before Hebrew calendar start to the mean
 * conjunction of Tishrei in Hebrew year. */
static int64_t elapsed_days0(int year) {
    int64_t prevYear = (int64_t)year - 1;
    int64_t mElapsed =
        235 * (prevYear / 19) +
        12 * (prevYear % 19) +
        (((prevYear % 19) * 7 + 1) / 19);

    int64_t pElapsed = 204 + 793 * (mElapsed % 1080);

    int64_t hElapsed =
        5 +
        12 * mElapsed +
        793 * (mElapsed / 1080) +
        (pElapsed / 1080);

    int64_t parts = (pElapsed % 1080) + 1080 * (hElapsed % 24);

    int64_t day = 1 + 29 * mElapsed + (hElapsed / 24);

    int64_t altDay = day;

    if ((parts >= 19440) ||
        (((day % 7) == 2) && (parts >= 9924) && !heb_hdate_is_leap_year(year)) ||
        (((day % 7) == 1) && (parts >= 16789) && heb_hdate_is_leap_year((int)prevYear))) {
        altDay = day + 1;
    }

    if (altDay % 7 == 0 || altDay % 7 == 3 || altDay % 7 == 5) {
        return altDay + 1;
    }
    return altDay;
}

/* elapsedDays: same as elapsed_days0 (cache omitted; computation is O(1)). */
static int64_t elapsed_days(int year) {
    return elapsed_days0(year);
}

/* yearInfo: returns elapsed days and year type via pointers. */
static void year_info(int year, int64_t *elapsed, int *t) {
    *elapsed = elapsed_days(year);
    *t = year_type((int)(elapsed_days(year + 1) - *elapsed));
}

static int64_t new_year(int year) {
    return HEB_HDATE_EPOCH + elapsed_days(year);
}

int heb_hdate_days_in_year(int year) {
    return (int)(elapsed_days(year + 1) - elapsed_days(year));
}

int heb_hdate_long_cheshvan(int year) {
    return heb_hdate_days_in_year(year) % 10 == 5;
}

int heb_hdate_short_kislev(int year) {
    return heb_hdate_days_in_year(year) % 10 == 3;
}

int heb_hdate_days_in_month(heb_hmonth_t month, int year) {
    int64_t elapsed;
    int t;
    year_info(year, &elapsed, &t);
    return dim_type(t, month);
}

/* === R.D. conversions === */

heb_rd_t heb_hdate_to_rd(int year, heb_hmonth_t month, int day) {
    int64_t elapsed;
    int t;
    year_info(year, &elapsed, &t);
    return HEB_HDATE_EPOCH + elapsed + month_offset(t, month) + (int64_t)day - 1;
}

int heb_hdate_create(heb_hdate_t *hd, int year, heb_hmonth_t month, int day) {
    memset(hd, 0, sizeof(*hd));
    if (year < 1) return HEB_HDATE_E_YEAR;
    if (month == HEB_H_ADAR2 && !heb_hdate_is_leap_year(year)) {
        month = HEB_H_ADAR1;
    }
    if (month == HEB_H_ADAR2 + 1) {
        month = HEB_H_NISAN;
    }
    if (month < HEB_H_NISAN || month > HEB_H_ADAR2) {
        return HEB_HDATE_E_MONTH;
    }
    int64_t elapsed;
    int t;
    year_info(year, &elapsed, &t);
    if (day < 1 || day > dim_type(t, month)) {
        return HEB_HDATE_E_DAY;
    }
    hd->year = year;
    hd->month = month;
    hd->day = day;
    hd->abs = HEB_HDATE_EPOCH + elapsed + month_offset(t, month) + (int64_t)day - 1;
    return HEB_HDATE_E_OK;
}

heb_hdate_t heb_hdate_from_rd(heb_rd_t rataDie) {
    heb_hdate_t hd;
    memset(&hd, 0, sizeof(hd));

    int year = (int)((double)(rataDie - HEB_HDATE_EPOCH) / HEB_HDATE_AVG_YEAR_DAYS);
    if (year < 1) year = 1;
    while (year > 1 && new_year(year) > rataDie) {
        year--;
    }
    while (new_year(year + 1) <= rataDie) {
        year++;
    }

    int64_t elapsed;
    int t;
    year_info(year, &elapsed, &t);
    int dayOfYear = (int)(rataDie - (HEB_HDATE_EPOCH + elapsed)); /* 0-based */
    heb_hmonth_t month = (heb_hmonth_t)tab_month_by_day(t, dayOfYear);
    int day = dayOfYear - (int)month_offset(t, month) + 1;

    hd.year = year;
    hd.month = month;
    hd.day = day;
    hd.abs = rataDie;
    return hd;
}

heb_hdate_t heb_hdate_from_gregorian(int year, heb_month_greg_t month, int day) {
    int err;
    heb_rd_t rd = heb_greg_to_rd(year, month, day, &err);
    (void)err;
    return heb_hdate_from_rd(rd);
}

heb_hdate_t heb_hdate_from_proleptic_gregorian(int year, heb_month_greg_t month, int day) {
    heb_rd_t rd = heb_greg_proleptic_to_rd(year, month, day);
    return heb_hdate_from_rd(rd);
}

/* === Accessors === */

int heb_hdate_days_in_month_hd(const heb_hdate_t *hd) {
    return heb_hdate_days_in_month(hd->month, hd->year);
}

heb_gdate_t heb_hdate_greg(const heb_hdate_t *hd) {
    return heb_greg_from_rd(hd->abs);
}

heb_gdate_t heb_hdate_proleptic_greg(const heb_hdate_t *hd) {
    return heb_greg_proleptic_from_rd(hd->abs);
}

heb_weekday_t heb_hdate_weekday(const heb_hdate_t *hd) {
    heb_rd_t rem = hd->abs % 7;
    if (rem < 0) rem += 7;
    return (heb_weekday_t)rem;
}

heb_hdate_t heb_hdate_prev(const heb_hdate_t *hd) {
    return heb_hdate_from_rd(hd->abs - 1);
}

heb_hdate_t heb_hdate_next(const heb_hdate_t *hd) {
    return heb_hdate_from_rd(hd->abs + 1);
}

int heb_hdate_is_leap_year_hd(const heb_hdate_t *hd) {
    return heb_hdate_is_leap_year(hd->year);
}

/* === Month name tables === */

static const char *const en_month_names[HEB_H_ADAR2 + 1] = {
    "", "Nisan", "Iyyar", "Sivan", "Tammuz", "Av", "Elul",
    "Tishrei", "Cheshvan", "Kislev", "Tevet", "Sh'vat",
    "Adar I", "Adar II"
};

static const char *const he_month_names_nikud[HEB_H_ADAR2 + 1] = {
    "",
    "\xd7\xa0\xd6\xb4\xd7\x99\xd7\xa1\xd6\xb8\xd7\x9f",         /* Nisan */
    "\xd7\x90\xd6\xb4\xd7\x99\xd6\xb8\xd7\x99\xd7\xa8",           /* Iyyar */
    "\xd7\xa1\xd6\xb4\xd7\x99\xd7\x95\xd6\xb8\xd7\x9f",           /* Sivan */
    "\xd7\xaa\xd6\xb7\xd6\xbc\xd7\x9e\xd6\xbc\xd7\x95\xd6\xbc\xd7\x96", /* Tamuz */
    "\xd7\x90\xd6\xb8\xd7\x91",                                   /* Av */
    "\xd7\x90\xd6\xb1\xd7\x9c\xd7\x95\xd6\xbc\xd7\x9c",           /* Elul */
    "\xd7\xaa\xd6\xb4\xd7\xa9\xd6\xb0\xd7\xa8\xd6\xb5\xd7\x99",   /* Tishrei */
    "\xd7\x97\xd6\xb6\xd7\xa9\xd6\xb0\xd7\x95\xd6\xb8\xd7\x9f",   /* Cheshvan */
    "\xd7\x9b\xd6\xb4\xd6\xbc\xd7\xa1\xd6\xb0\xd7\x9c\xd6\xb5\xd7\x95", /* Kislev */
    "\xd7\x98\xd6\xb5\xd7\x91\xd6\xb5\xd7\xaa",                   /* Tevet */
    "\xd7\xa9\xd6\xb0\xd7\x91\xd6\xb8\xd7\x98",                   /* Shvat */
    "\xd7\x90\xd6\xb7\xd7\x93\xd6\xb8\xd7\xa8 \xd7\x90\xd7\xb3",  /* Adar I */
    "\xd7\x90\xd6\xb7\xd7\x93\xd6\xb8\xd7\xa8 \xd7\x91\xd7\xb3",  /* Adar II */
};

static const char *const he_month_names_nonikud[HEB_H_ADAR2 + 1] = {
    "",
    "\xd7\xa0\xd7\x99\xd7\xa1\xd7\x9f",               /* Nisan */
    "\xd7\x90\xd7\x99\xd7\x99\xd7\xa8",               /* Iyyar */
    "\xd7\xa1\xd7\x99\xd7\x95\xd7\x9f",               /* Sivan */
    "\xd7\xaa\xd7\x9e\xd7\x95\xd7\x96",               /* Tamuz */
    "\xd7\x90\xd7\x91",                                /* Av */
    "\xd7\x90\xd7\x9c\xd7\x95\xd7\x9c",               /* Elul */
    "\xd7\xaa\xd7\xa9\xd7\xa8\xd7\x99",               /* Tishrei */
    "\xd7\x97\xd7\xa9\xd7\x95\xd7\x9f",               /* Cheshvan */
    "\xd7\x9b\xd7\xa1\xd7\x9c\xd7\x95",               /* Kislev */
    "\xd7\x98\xd7\x91\xd7\xaa",                       /* Tevet */
    "\xd7\xa9\xd7\x91\xd7\x98",                       /* Shvat */
    "\xd7\x90\xd7\x93\xd7\xa8 \xd7\x90\xd7\xb3",      /* Adar I */
    "\xd7\x90\xd7\x93\xd7\xa8 \xd7\x91\xd7\xb3",      /* Adar II */
};

static const char *he_adar_nikud = "\xd7\x90\xd6\xb7\xd7\x93\xd6\xb8\xd7\xa8";       /* אַדָר */
static const char *he_adar_nonikud = "\xd7\x90\xd7\x93\xd7\xa8";                      /* אדר */

static int str_ieq(const char *a, const char *b) {
    while (*a && *b) {
        char ca = *a, cb = *b;
        if (ca >= 'A' && ca <= 'Z') ca += 32;
        if (cb >= 'A' && cb <= 'Z') cb += 32;
        if (ca != cb) return 0;
        a++; b++;
    }
    return *a == *b;
}

int heb_hdate_month_name(const heb_hdate_t *hd, const char *locale, char *buf, size_t n) {
    heb_hmonth_t month = hd->month;
    int isAdar = (month == HEB_H_ADAR1 && !heb_hdate_is_leap_year(hd->year));
    const char *name = NULL;

    if (str_ieq(locale, HEB_LOCALE_HE)) {
        if (isAdar) name = he_adar_nikud;
        else name = he_month_names_nikud[month];
    } else if (str_ieq(locale, HEB_LOCALE_HE_NONIKUD)) {
        if (isAdar) name = he_adar_nonikud;
        else name = he_month_names_nonikud[month];
    } else {
        if (isAdar) name = "Adar";
        else name = en_month_names[month];
    }
    if (!name) name = "";
    size_t len = strlen(name);
    if (len >= n) len = n - 1;
    memcpy(buf, name, len);
    buf[len] = '\0';
    return (int)len;
}

int heb_hdate_to_string(const heb_hdate_t *hd, char *buf, size_t n) {
    char mname[32];
    heb_hdate_month_name(hd, HEB_LOCALE_EN, mname, sizeof(mname));
    return snprintf(buf, n, "%d %s %d", hd->day, mname, hd->year);
}

/* === UTF-8 utilities === */

/* Decode one UTF-8 rune from s (len bytes).  Returns rune via *r, bytes
 * consumed via *size.  Handles ASCII and multibyte sequences. */
static void utf8_decode(const char *s, size_t len, int32_t *r, int *size) {
    if (len == 0) { *r = 0; *size = 0; return; }
    unsigned char c = (unsigned char)s[0];
    if (c < 0x80) { *r = c; *size = 1; return; }
    int32_t rune;
    int sz;
    if ((c & 0xE0) == 0xC0) { rune = c & 0x1F; sz = 2; }
    else if ((c & 0xF0) == 0xE0) { rune = c & 0x0F; sz = 3; }
    else if ((c & 0xF8) == 0xF0) { rune = c & 0x07; sz = 4; }
    else { *r = c; *size = 1; return; }
    for (int i = 1; i < sz && i < (int)len; i++) {
        rune = (rune << 6) | ((unsigned char)s[i] & 0x3F);
    }
    *r = rune;
    *size = sz;
}

static int32_t to_lower_ascii(int32_t r) {
    if (r >= 'A' && r <= 'Z') return r + ('a' - 'A');
    return r;
}

/* Get the last rune and (optionally) second-to-last rune of a UTF-8 string.
 * If strip_geresh is nonzero, a trailing ׳ (U+05F3) is skipped first.
 * Returns 1 if last rune found (stored in *last), 0 if string empty.
 * second-to-last stored in *prev (0 if none). */
static int last_runes(const char *s, int strip_geresh, int32_t *last, int32_t *prev) {
    size_t len = strlen(s);
    if (len == 0) { *last = 0; *prev = 0; return 0; }

    /* Find last rune position */
    size_t end = len;
    if (strip_geresh) {
        /* Check if last bytes are ׳ (U+05F3 = 0xD7 0xB3) */
        if (end >= 2 && (unsigned char)s[end-2] == 0xD7 && (unsigned char)s[end-1] == 0xB3) {
            end -= 2;
            if (end == 0) { *last = 0; *prev = 0; return 0; }
        }
    }

    /* Walk backwards to find start of last rune */
    size_t p = end - 1;
    while (p > 0 && ((unsigned char)s[p] & 0xC0) == 0x80) p--;

    int32_t r; int sz;
    utf8_decode(s + p, end - p, &r, &sz);
    *last = r;

    /* Find second-to-last rune */
    if (p == 0) { *prev = 0; return 1; }
    size_t end2 = p;
    size_t p2 = end2 - 1;
    while (p2 > 0 && ((unsigned char)s[p2] & 0xC0) == 0x80) p2--;
    int32_t r2; int sz2;
    utf8_decode(s + p2, end2 - p2, &r2, &sz2);
    *prev = r2;
    return 1;
}

/* adarRegex.MatchString(s) — simplified implementation of
 * (?i)(1|[^i]i|a|א)(׳?)$ */
static int adar_regex_match(const char *s) {
    int32_t last, prev;
    if (!last_runes(s, 1, &last, &prev)) return 0;

    int32_t lower = to_lower_ascii(last);
    if (last == '1') return 1;
    if (lower == 'a') return 1;
    if (last == 0x05D0) return 1; /* א */
    if (lower == 'i') {
        int32_t pl = to_lower_ascii(prev);
        if (pl != 0 && pl != 'i') return 1;
    }
    return 0;
}

int heb_hdate_month_from_name(const char *name, heb_hmonth_t *month) {
    size_t len = strlen(name);
    if (len == 0) return 1;

    int32_t r0, r1;
    int sz0;
    utf8_decode(name, len, &r0, &sz0);
    if (sz0 == 0) return 1;
    int32_t r1_val = 0;
    if ((size_t)sz0 < len) {
        int sz1;
        utf8_decode(name + sz0, len - sz0, &r1_val, &sz1);
    }
    r0 = to_lower_ascii(r0);
    r1 = to_lower_ascii(r1_val);

    switch (r0) {
    case 'n': case 0x05E0: /* נ */
        if (r1 == 'o') return 1; /* catches "november" */
        *month = HEB_H_NISAN; return 0;
    case 'i':
        *month = HEB_H_IYYAR; return 0;
    case 'e':
        *month = HEB_H_ELUL; return 0;
    case 'c': case 0x05D7: /* ח */
        *month = HEB_H_CHESHVAN; return 0;
    case 'k': case 0x05DB: /* כ */
        *month = HEB_H_KISLEV; return 0;
    case 's':
        switch (r1) {
        case 'i': *month = HEB_H_SIVAN; return 0;
        case 'h': *month = HEB_H_SHVAT; return 0;
        }
        break;
    case 't':
        switch (r1) {
        case 'a': *month = HEB_H_TAMUZ; return 0;
        case 'i': *month = HEB_H_TISHREI; return 0;
        case 'e': *month = HEB_H_TEVET; return 0;
        }
        break;
    case 'a':
        switch (r1) {
        case 'v': *month = HEB_H_AV; return 0;
        case 'd':
            if (adar_regex_match(name)) { *month = HEB_H_ADAR1; return 0; }
            *month = HEB_H_ADAR2; return 0;
        }
        break;
    case 0x05E1: /* ס */
        *month = HEB_H_SIVAN; return 0;
    case 0x05D8: /* ט */
        *month = HEB_H_TEVET; return 0;
    case 0x05E9: /* ש */
        *month = HEB_H_SHVAT; return 0;
    case 0x05D0: /* א */
        switch (r1) {
        case 0x05D1: *month = HEB_H_AV; return 0;     /* ב */
        case 0x05D3:                                   /* ד */
            if (adar_regex_match(name)) { *month = HEB_H_ADAR1; return 0; }
            *month = HEB_H_ADAR2; return 0;
        case 0x05D9: *month = HEB_H_IYYAR; return 0;  /* י */
        case 0x05DC: *month = HEB_H_ELUL; return 0;   /* ל */
        }
        break;
    case 0x05EA: /* ת */
        switch (r1) {
        case 0x05DE: *month = HEB_H_TAMUZ; return 0;   /* מ */
        case 0x05E9: *month = HEB_H_TISHREI; return 0; /* ש */
        }
        break;
    }
    return 1; /* unable to parse */
}

/* === Day-of-week utilities === */

heb_rd_t heb_hdate_day_on_or_before(heb_weekday_t dayOfWeek, heb_rd_t rataDie) {
    heb_rd_t rem = (rataDie - (heb_rd_t)dayOfWeek) % 7;
    if (rem < 0) rem += 7;
    return rataDie - rem;
}

static heb_hdate_t on_or_before(heb_weekday_t dayOfWeek, heb_rd_t rataDie) {
    return heb_hdate_from_rd(heb_hdate_day_on_or_before(dayOfWeek, rataDie));
}

heb_hdate_t heb_hdate_before(const heb_hdate_t *hd, heb_weekday_t dayOfWeek) {
    return on_or_before(dayOfWeek, hd->abs - 1);
}

heb_hdate_t heb_hdate_on_or_before(const heb_hdate_t *hd, heb_weekday_t dayOfWeek) {
    return on_or_before(dayOfWeek, hd->abs);
}

heb_hdate_t heb_hdate_nearest(const heb_hdate_t *hd, heb_weekday_t dayOfWeek) {
    return on_or_before(dayOfWeek, hd->abs + 3);
}

heb_hdate_t heb_hdate_on_or_after(const heb_hdate_t *hd, heb_weekday_t dayOfWeek) {
    return on_or_before(dayOfWeek, hd->abs + 6);
}

heb_hdate_t heb_hdate_after(const heb_hdate_t *hd, heb_weekday_t dayOfWeek) {
    return on_or_before(dayOfWeek, hd->abs + 7);
}

/* === Hebrew nikud utilities === */

static int is_nikkud(int32_t r) {
    return r >= 0x0590 && r <= 0x05c7 && r != 0x05be;
}

int heb_hebrew_strip_nikkud(const char *src, char *dst, size_t n) {
    size_t di = 0;
    const char *s = src;
    size_t slen = strlen(src);
    while (*s && di < n - 1) {
        int32_t r;
        int sz;
        utf8_decode(s, slen - (size_t)(s - src), &r, &sz);
        if (!is_nikkud(r)) {
            for (int i = 0; i < sz && di < n - 1; i++) {
                dst[di++] = s[i];
            }
        }
        s += sz;
    }
    dst[di] = '\0';
    return (int)di;
}
