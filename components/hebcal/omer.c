/*
 * hebcal-c - Sefirat HaOmer (Counting of the Omer).
 * Ported from github.com/hebcal/hebcal-go/omer (Go).
 *
 * License: GPL-2.0-or-later
 */
#include "omer.h"
#include "gematriya.h"
#include "locale.h"
#include "hdate.h"
#include <string.h>
#include <stdio.h>

heb_omer_t heb_omer_new(heb_hdate_t date, int omerDay) {
    heb_omer_t ev;
    ev.date = date;
    ev.omer_day = omerDay;
    ev.week_number = ((omerDay - 1) / 7) + 1;
    ev.days_within_weeks = omerDay % 7;
    if (ev.days_within_weeks == 0) ev.days_within_weeks = 7;
    return ev;
}

int heb_omer_get_weeks(const heb_omer_t *ev) {
    if (ev->days_within_weeks == 7) return ev->week_number;
    return ev->week_number - 1;
}

/* English ordinal: 1st, 2nd, 3rd, 4th, ... 21st, 22nd, ... */
static void ordinal(int n, char *buf, size_t len) {
    const char *suffix;
    int mod100 = n % 100;
    if (mod100 >= 11 && mod100 <= 13) {
        suffix = "th";
    } else {
        switch (n % 10) {
        case 1: suffix = "st"; break;
        case 2: suffix = "nd"; break;
        case 3: suffix = "rd"; break;
        default: suffix = "th"; break;
        }
    }
    snprintf(buf, len, "%d%s", n, suffix);
}

void heb_omer_render(const heb_omer_t *ev, const char *locale,
                     char *buf, size_t n) {
    if (heb_locale_is_hebrew(locale)) {
        char gem[16];
        heb_gematriya(ev->omer_day, gem, sizeof(gem));
        char dayStr[32];
        heb_locale_lookup("day of the Omer", locale, dayStr, sizeof(dayStr));
        snprintf(buf, n, "%s %s", gem, dayStr);
    } else {
        char ord[16];
        ordinal(ev->omer_day, ord, sizeof(ord));
        char dayStr[32];
        heb_locale_lookup("day of the Omer", locale, dayStr, sizeof(dayStr));
        snprintf(buf, n, "%s %s", ord, dayStr);
    }
}

/* === Hebrew "Today is..." text (ported from omer.go todayIsHe) === */

static const char *const he_tens[] = {
    "",
    "\xd7\xa2\xd6\xb2\xd7\xa9\xd6\xb8\xd7\x82\xd7\xa8\xd6\xb8\xd7\x94",     /* עֲשָׂרָה */
    "\xd7\xa2\xd6\xb6\xd7\xa9\xd6\xb0\xd7\x82\xd7\xa8\xd6\xb4\xd7\x99\xd7\x9d", /* עֶשְׂרִים */
    "\xd7\xa9\xd6\xb0\xd7\x81\xd7\x9c\xd7\x95\xd6\xb9\xd7\xa9\xd6\xb4\xd7\x81\xd7\x99\xd7\x9d", /* שְׁלוֹשִׁים */
    "\xd7\x90\xd6\xb7\xd7\xa8\xd6\xb0\xd7\x91\xd6\xb8\xd6\xbc\xd7\xa2\xd6\xb4\xd7\x99\xd7\x9d"  /* אַרְבָּעִים */
};

static const char *const he_ones[] = {
    "",
    "\xd7\x90\xd6\xb6\xd7\x97\xd6\xb8\xd7\x93",     /* אֶחָד */
    "\xd7\xa9\xd6\xb0\xd7\x81\xd7\xa0\xd6\xb7\xd7\x99\xd6\xb4\xd7\x9d",     /* שְׁנַיִם */
    "\xd7\xa9\xd6\xb0\xd7\x81\xd7\x9c\xd7\x95\xd6\xb9\xd7\xa9\xd6\xb8\xd7\x81\xd7\x94",     /* שְׁלוֹשָׁה */
    "\xd7\x90\xd6\xb7\xd7\xa8\xd6\xb0\xd7\x91\xd6\xb8\xd6\xbc\xd7\xa2\xd6\xb8\xd7\x94",     /* אַרְבָּעָה */
    "\xd7\x97\xd6\xb2\xd7\x9e\xd6\xb4\xd7\xa9\xd6\xb8\xd7\x81\xd7\x94",     /* חֲמִשָׁה */
    "\xd7\xa9\xd6\xb4\xd7\x81\xd7\xa9\xd6\xb8\xd7\x81\xd7\x94",             /* שִׁשָׁה */
    "\xd7\xa9\xd6\xb4\xd7\x81\xd7\x91\xd6\xb0\xd7\xa2\xd6\xb8\xd7\x94",     /* שִׁבְעָה */
    "\xd7\xa9\xd6\xb0\xd7\x81\xd7\x9e\xd7\x95\xd6\xb9\xd7\xa0\xd6\xb8\xd7\x94",     /* שְׁמוֹנָה */
    "\xd7\xaa\xd6\xb4\xd6\xbc\xd7\xa9\xd6\xb0\xd7\x81\xd7\xa2\xd6\xb8\xd7\x94",     /* תִּשְׁעָה */
};

#define HE_SHNEI     "\xd7\xa9\xd6\xb0\xd7\x81\xd7\xa0\xd6\xb5\xd7\x99"      /* שְׁנֵי */
#define HE_YAMIM     "\xd7\x99\xd6\xb8\xd7\x9e\xd6\xb4\xd7\x99\xd7\x9d"      /* יָמִים */
#define HE_SHNEI_YAMIM HE_SHNEI " " HE_YAMIM                                 /* שְׁנֵי יָמִים */
#define HE_SHAVUOT   "\xd7\xa9\xd6\xb8\xd7\x81\xd7\x91\xd7\x95\xd6\xbc\xd7\xa2\xd7\x95\xd6\xb9\xd7\xaa" /* שָׁבוּעוֹת */
#define HE_YOM       "\xd7\x99\xd7\x95\xd6\xb9\xd7\x9d"                      /* יוֹם */
#define HE_YOM_ECHAD HE_YOM " " HE_ONES_1
#define HE_ONES_1    "\xd7\x90\xd6\xb6\xd7\x97\xd6\xb8\xd7\x93"              /* אֶחָד */
#define HE_ASAR      "\xd7\xa2\xd6\xb8\xd7\xa9\xd6\xb8\xd7\x82\xd7\xa8"      /* עָשָׂר */
#define HE_SHEHEM    "\xd7\xa9\xd6\xb0\xd7\x81\xd7\x94\xd6\xb5\xd7\x9d"      /* שְׁהֵם */
#define HE_SHAVUA    "\xd7\xa9\xd6\xb8\xd7\x81\xd7\x91\xd7\x95\xd6\xbc\xd7\xa2\xd6\xb7" /* שָׁבוּעַ */
#define HE_LAOMER    "\xd7\x9c\xd6\xb8\xd7\xa2\xd7\x95\xd6\xb9\xd7\x9e\xd6\xb6\xd7\xa8" /* לָעוֹמֶר */
#define HE_V         "\xd7\x95\xd6\xb0"                                       /* וְ */
#define HE_HAYOM     "\xd7\x94\xd6\xb7\xd7\x99\xd7\x95\xd6\xb9\xd7\x9d"     /* הַיוֹם */

/* Helper: append a string to buf at position *pos */
static void app(char *buf, size_t n, size_t *pos, const char *s) {
    size_t l = strlen(s);
    if (*pos + l < n) { memcpy(buf + *pos, s, l); *pos += l; }
}

static void today_is_he(int omer, char *buf, size_t n) {
    size_t pos = 0;
    int ten = omer / 10;
    int one = omer % 10;

    app(buf, n, &pos, HE_HAYOM " ");

    if (10 < omer && omer < 20) {
        app(buf, n, &pos, he_ones[one]);
        app(buf, n, &pos, " " HE_ASAR);
    } else if (omer > 9) {
        app(buf, n, &pos, he_ones[one]);
        if (one != 0) {
            app(buf, n, &pos, " " HE_V);
        }
    }

    if (omer > 2) {
        if ((omer > 20) || (omer == 10) || (omer == 20)) {
            app(buf, n, &pos, he_tens[ten]);
        }
        if (omer < 11) {
            app(buf, n, &pos, he_ones[one]);
            app(buf, n, &pos, " " HE_YAMIM " ");
        } else {
            app(buf, n, &pos, " " HE_YOM " ");
        }
    } else if (omer == 1) {
        app(buf, n, &pos, HE_YOM_ECHAD " ");
    } else { /* omer == 2 */
        app(buf, n, &pos, HE_SHNEI_YAMIM " ");
    }

    if (omer > 6) {
        /* trim trailing space */
        if (pos > 0 && buf[pos-1] == ' ') pos--;
        app(buf, n, &pos, ", " HE_SHEHEM " ");
        int weeks = omer / 7;
        int days = omer % 7;
        if (weeks > 2) {
            app(buf, n, &pos, he_ones[weeks]);
            app(buf, n, &pos, " " HE_SHAVUOT " ");
        } else if (weeks == 1) {
            app(buf, n, &pos, HE_SHAVUA " " HE_ONES_1 " ");
        } else { /* weeks == 2 */
            app(buf, n, &pos, HE_SHNEI " " HE_SHAVUOT " ");
        }
        if (days != 0) {
            app(buf, n, &pos, HE_V);
            if (days > 2) {
                app(buf, n, &pos, he_ones[days]);
                app(buf, n, &pos, " " HE_YAMIM " ");
            } else if (days == 1) {
                app(buf, n, &pos, HE_YOM_ECHAD " ");
            } else { /* days == 2 */
                app(buf, n, &pos, HE_SHNEI_YAMIM " ");
            }
        }
    }

    app(buf, n, &pos, HE_LAOMER);
    buf[pos] = '\0';
}

void heb_omer_today_is(const heb_omer_t *ev, const char *locale,
                       char *buf, size_t n) {
    if (heb_locale_is_hebrew(locale)) {
        today_is_he(ev->omer_day, buf, n);
        if (strstr(locale, "nonikud") || strstr(locale, "NoNikud") ||
            strstr(locale, "NONIKUD")) {
            char tmp[512];
            heb_hebrew_strip_nikkud(buf, tmp, sizeof(tmp));
            size_t l = strlen(tmp);
            if (l >= n) l = n - 1;
            memcpy(buf, tmp, l);
            buf[l] = '\0';
        }
        return;
    }

    /* English */
    int omer = ev->omer_day;
    const char *totalDaysStr = (omer == 1) ? "day" : "days";
    int pos = snprintf(buf, n, "Today is %d %s", omer, totalDaysStr);

    if (ev->week_number > 1 || omer == 7) {
        int day7 = (ev->days_within_weeks == 7);
        int numWeeks = ev->week_number - 1;
        if (day7) numWeeks = ev->week_number;
        const char *weeksStr = (numWeeks == 1) ? "week" : "weeks";
        pos += snprintf(buf + pos, n - pos, ", which is %d %s", numWeeks, weeksStr);
        if (!day7) {
            const char *dayStr = (ev->days_within_weeks == 1) ? "day" : "days";
            pos += snprintf(buf + pos, n - pos, " and %d %s",
                            ev->days_within_weeks, dayStr);
        }
    }
    snprintf(buf + pos, n - pos, " of the Omer");
}

/* Sefirot names */
static const char *const sefirot_en[] = {
    "", "Lovingkindness", "Might", "Beauty", "Eternity",
    "Splendor", "Foundation", "Majesty"
};
static const char *const sefirot_translit[] = {
    "", "Chesed", "Gevurah", "Tiferet", "Netzach",
    "Hod", "Yesod", "Malkhut"
};

void heb_omer_sefira(const heb_omer_t *ev, const char *locale,
                     char *buf, size_t n) {
    const char *weekStr = sefirot_en[ev->week_number];
    const char *dayWithinWeekStr = sefirot_en[ev->days_within_weeks];
    int weekNum2or6 = (ev->week_number == 2 || ev->week_number == 6);

    if (heb_locale_is_hebrew(locale)) {
        char week[32], dayW[32];
        heb_locale_lookup(weekStr, locale, week, sizeof(week));
        heb_locale_lookup(dayWithinWeekStr, locale, dayW, sizeof(dayW));
        const char *prefix = weekNum2or6
            ? "\xd7\xa9\xd6\xb6\xd7\x81\xd7\x91\xd6\xb4\xd6\xbc"  /* שֶׁבִּ */
            : "\xd7\xa9\xd6\xb6\xd7\x81\xd7\x91\xd6\xb0\xd6\xbc";  /* שֶׁבְּ */
        if (strstr(locale, "nonikud") || strstr(locale, "NoNikud") ||
            strstr(locale, "NONIKUD")) {
            char ptmp[16];
            heb_hebrew_strip_nikkud(prefix, ptmp, sizeof(ptmp));
            snprintf(buf, n, "%s %s%s", dayW, ptmp, week);
        } else {
            snprintf(buf, n, "%s %s%s", dayW, prefix, week);
        }
    } else if (locale && strcmp(locale, "translit") == 0) {
        const char *prefix = weekNum2or6 ? "shebi" : "sheb'";
        snprintf(buf, n, "%s %s%s",
                 sefirot_translit[ev->days_within_weeks], prefix,
                 sefirot_translit[ev->week_number]);
    } else {
        snprintf(buf, n, "%s within %s", dayWithinWeekStr, weekStr);
    }
}

void heb_omer_emoji(const heb_omer_t *ev, char *buf, size_t n) {
    int number = ev->omer_day;
    int32_t r;
    if (number <= 20) {
        r = 9312 + number - 1;  /* ① .. ⑳ */
    } else if (number <= 35) {
        r = 12881 + number - 21; /* ㉑ .. ㉟ */
    } else {
        r = 12977 + number - 36; /* ㊱ .. ㊾ */
    }
    /* Encode as UTF-8 */
    if (r <= 0x7F) {
        snprintf(buf, n, "%c", (char)r);
    } else if (r <= 0x7FF) {
        snprintf(buf, n, "%c%c", (char)(0xC0 | (r >> 6)), (char)(0x80 | (r & 0x3F)));
    } else {
        snprintf(buf, n, "%c%c%c",
                 (char)(0xE0 | (r >> 12)),
                 (char)(0x80 | ((r >> 6) & 0x3F)),
                 (char)(0x80 | (r & 0x3F)));
    }
}
