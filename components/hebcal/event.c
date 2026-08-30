/*
 * hebcal-c - Calendar event types and flags.
 * Ported from github.com/hebcal/hebcal-go/event (Go).
 *
 * License: GPL-2.0-or-later
 */
#include "event.h"
#include "locale.h"
#include "gematriya.h"
#include "hdate.h"
#include <string.h>
#include <stdio.h>

/* flagToCategory table (mirrors Go event.go flagToCategory). */
typedef struct {
    heb_holiday_flags_t flag;
    const char *cats;
} flag_cat_t;

static const flag_cat_t flag_to_cat[] = {
    {HEB_F_MAJOR_FAST,        "fast"},
    {HEB_F_CHANUKAH_CANDLES,  "minor"},
    {HEB_F_HEBREW_DATE,       "hebdate"},
    {HEB_F_MINOR_FAST,        "fast"},
    {HEB_F_MINOR_HOLIDAY,     "minor"},
    {HEB_F_MODERN_HOLIDAY,    "modern"},
    {HEB_F_MOLAD,             "molad"},
    {HEB_F_OMER_COUNT,        "omer"},
    {HEB_F_PARSHA_HASHAVUA,   "parashat"},
    {HEB_F_ROSH_CHODESH,      "roshchodesh"},
    {HEB_F_SHABBAT_MEVARCHIM, "mevarchim"},
    {HEB_F_SPECIAL_SHABBAT,   "shabbat"},
    {HEB_F_USER_EVENT,        "user"},
};

const char *heb_flags_to_category(heb_holiday_flags_t mask) {
    for (size_t i = 0; i < sizeof(flag_to_cat)/sizeof(flag_to_cat[0]); i++) {
        if (mask & flag_to_cat[i].flag) {
            return flag_to_cat[i].cats;
        }
    }
    return "unknown";
}

/* minorHolidays set (mirrors Go event/holiday.go). */
static int is_minor_holiday(const char *desc) {
    static const char *const minor[] = {
        "Lag BaOmer", "Leil Selichot", "Pesach Sheni",
        "Erev Purim", "Purim Katan", "Shushan Purim",
        "Tu B'Av", "Tu BiShvat", "Rosh Hashana LaBehemot",
        NULL
    };
    for (int i = 0; minor[i]; i++) {
        if (strcmp(desc, minor[i]) == 0) return 1;
    }
    return 0;
}

const char *heb_event_category(const heb_event_t *ev) {
    if (ev->chol_hamoed_day != 0) return "cholhamoed";
    const char *cat = heb_flags_to_category(ev->flags);
    if (strcmp(cat, "unknown") != 0) return cat;
    if (is_minor_holiday(ev->desc)) return "minor";
    return "major";
}

/* Basename: strips suffixes like " (CH''M)", " (observed)", " I", etc. */
void heb_event_basename(const heb_event_t *ev, char *buf, size_t n) {
    const char *str = ev->desc;
    size_t len = strlen(str);
    if (len >= n) len = n - 1;
    memcpy(buf, str, len);
    buf[len] = '\0';

    /* Strip " YYYY$" (space + 4 digits at end) */
    {
        size_t l = strlen(buf);
        if (l >= 6 && buf[l-5] == ' ' &&
            buf[l-4] >= '0' && buf[l-4] <= '9' &&
            buf[l-3] >= '0' && buf[l-3] <= '9' &&
            buf[l-2] >= '0' && buf[l-2] <= '9' &&
            buf[l-1] >= '0' && buf[l-1] <= '9') {
            buf[l-5] = '\0';
        }
    }
    /* Strip " (CH''M)$" */
    {
        const char *suffix = " (CH''M)";
        size_t sl = strlen(suffix);
        size_t bl = strlen(buf);
        if (bl >= sl && strcmp(buf + bl - sl, suffix) == 0) {
            buf[bl - sl] = '\0';
        }
    }
    /* Strip " (observed)$" */
    {
        const char *suffix = " (observed)";
        size_t sl = strlen(suffix);
        size_t bl = strlen(buf);
        if (bl >= sl && strcmp(buf + bl - sl, suffix) == 0) {
            buf[bl - sl] = '\0';
        }
    }
    /* Strip " (Hoshana Raba)$" */
    {
        const char *suffix = " (Hoshana Raba)";
        size_t sl = strlen(suffix);
        size_t bl = strlen(buf);
        if (bl >= sl && strcmp(buf + bl - sl, suffix) == 0) {
            buf[bl - sl] = '\0';
        }
    }
    /* Strip " [IV]+$" (space + roman numerals at end) */
    {
        size_t bl = strlen(buf);
        if (bl >= 2 && buf[bl-1] >= 'A' && buf[bl-1] <= 'Z' &&
            (buf[bl-1] == 'I' || buf[bl-1] == 'V')) {
            size_t end = bl;
            while (end > 0 && (buf[end-1] == 'I' || buf[end-1] == 'V')) end--;
            if (end > 0 && buf[end-1] == ' ') {
                buf[end-1] = '\0';
            }
        }
    }
    /* Strip ": N Candles?$" (colon + space + digit + " Candle" + optional 's') */
    {
        size_t bl = strlen(buf);
        const char *candle_suffix = NULL;
        size_t suffix_len = 0;
        if (bl >= 8 && strcmp(buf + bl - 7, " Candle") == 0) {
            candle_suffix = buf + bl - 7;
            suffix_len = 7;
        } else if (bl >= 9 && strcmp(buf + bl - 8, " Candles") == 0) {
            candle_suffix = buf + bl - 8;
            suffix_len = 8;
        }
        if (candle_suffix) {
            /* Check for ": <digit>" before " Candle[s]" */
            size_t pos = bl - suffix_len; /* position of " Candle" start */
            if (pos >= 3 &&
                buf[pos - 1] >= '0' && buf[pos - 1] <= '9' &&
                buf[pos - 2] == ' ' && buf[pos - 3] == ':') {
                buf[pos - 3] = '\0';
            }
        }
    }
    /* Strip ": 8th Day$" */
    {
        const char *suffix = ": 8th Day";
        size_t sl = strlen(suffix);
        size_t bl = strlen(buf);
        if (bl >= sl && strcmp(buf + bl - sl, suffix) == 0) {
            buf[bl - sl] = '\0';
        }
    }
    /* Strip "^Erev " */
    {
        if (strncmp(buf, "Erev ", 5) == 0) {
            size_t bl = strlen(buf);
            memmove(buf, buf + 5, bl - 5 + 1);
        }
    }
}

/* Render: produces the localized event description. */
void heb_event_render(const heb_event_t *ev, const char *locale,
                      char *buf, size_t n) {
    if (n == 0) return;

    /* Rosh Chodesh: "Rosh Chodesh <Month>" */
    if (ev->flags & HEB_F_ROSH_CHODESH) {
        char rch[32], month[32];
        heb_locale_lookup("Rosh Chodesh", locale, rch, sizeof(rch));
        /* ev->desc is "Rosh Chodesh <Month>", skip 13 chars */
        const char *monthName = ev->desc + 13;
        heb_locale_lookup(monthName, locale, month, sizeof(month));
        snprintf(buf, n, "%s %s", rch, month);
        return;
    }

    /* Shabbat Mevarchim: "Shabbat Mevarchim Chodesh <Month>" */
    if (ev->flags & HEB_F_SHABBAT_MEVARCHIM) {
        char mevarchim[64], month[32];
        heb_locale_lookup("Shabbat Mevarchim Chodesh", locale,
                          mevarchim, sizeof(mevarchim));
        /* ev->desc is "Shabbat Mevarchim Chodesh <Month>" */
        const char *monthName = ev->desc + 27;
        heb_locale_lookup(monthName, locale, month, sizeof(month));
        snprintf(buf, n, "%s %s", mevarchim, month);
        return;
    }

    /* Rosh Hashana: "Rosh Hashana <year>" */
    if (ev->date.month == HEB_H_TISHREI && ev->date.day == 1) {
        char rh[32];
        heb_locale_lookup("Rosh Hashana", locale, rh, sizeof(rh));
        if (heb_locale_is_hebrew(locale)) {
            char yearbuf[16];
            heb_gematriya(ev->date.year, yearbuf, sizeof(yearbuf));
            snprintf(buf, n, "%s %s", rh, yearbuf);
        } else {
            snprintf(buf, n, "%s %d", rh, ev->date.year);
        }
        return;
    }

    /* Yom Kippur Katan: "Yom Kippur Katan <Month>" */
    if (ev->flags & HEB_F_YOM_KIPPUR_KATAN) {
        char ykk[32], month[32];
        heb_locale_lookup("Yom Kippur Katan", locale, ykk, sizeof(ykk));
        /* ev->desc is "Yom Kippur Katan <Month>", skip 17 chars */
        const char *monthName = ev->desc + 17;
        heb_locale_lookup(monthName, locale, month, sizeof(month));
        snprintf(buf, n, "%s %s", ykk, month);
        return;
    }

    /* Default: look up the description */
    heb_locale_lookup(ev->desc, locale, buf, n);
}
