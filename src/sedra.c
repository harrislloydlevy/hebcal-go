/*
 * hebcal-c - Weekly Torah reading (Parashat HaShavua).
 * Ported from github.com/hebcal/hebcal-go/sedra (Go).
 *
 * License: GPL-2.0-or-later
 */
#include "sedra.h"
#include "locale.h"
#include "arduino_compat.h"
#include <string.h>
#include <stdio.h>

/* The 54 parshiyot in reading order (English transliteration). */
static const char *const parshiot[HEB_NUM_PARSHIOT] = {
    "Bereshit", "Noach", "Lech-Lecha", "Vayera", "Chayei Sara",
    "Toldot", "Vayetzei", "Vayishlach", "Vayeshev", "Miketz",
    "Vayigash", "Vayechi", "Shemot", "Vaera", "Bo",
    "Beshalach", "Yitro", "Mishpatim", "Terumah", "Tetzaveh",
    "Ki Tisa", "Vayakhel", "Pekudei", "Vayikra", "Tzav",
    "Shmini", "Tazria", "Metzora", "Achrei Mot", "Kedoshim",
    "Emor", "Behar", "Bechukotai", "Bamidbar", "Nasso",
    "Beha'alotcha", "Sh'lach", "Korach", "Chukat", "Balak",
    "Pinchas", "Matot", "Masei", "Devarim", "Vaetchanan",
    "Eikev", "Re'eh", "Shoftim", "Ki Teitzei", "Ki Tavo",
    "Nitzavim", "Vayeilech", "Ha'azinu"
};

const char *const *heb_parshiot(void) {
    return parshiot;
}

/* Doubled parsha indices: -21, -26, -28, -31, -38, -41, -50.
 * Negative means doubled; undoubled(n) = -n. */
static int is_valid_double(int n) {
    switch (n) {
    case -21: case -26: case -28: case -31: case -38: case -41: case -50:
        return 1;
    default:
        return 0;
    }
}

/* --- Sedra arrays (from Go sedra.go) --- */
/* -1 = no parsha (holiday), 0..53 = single, negative = doubled */

static const int sat_short[] = {
    -1, 52, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, -21, 23, 24, -1, 25, -26, -28, 30, -31, 33, 34, 35, 36, 37, 38, 39, 40, -41, 43, 44, 45, 46, 47,
    48, 49, 50
};

static const int sat_long[] = {
    -1, 52, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, -21, 23, 24, -1, 25, -26, -28, 30, -31, 33, 34, 35, 36, 37, 38, 39, 40, -41, 43, 44, 45, 46, 47,
    48, 49, -50
};

static const int mon_short[] = {
    51, 52, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
    18, 19, 20, -21, 23, 24, -1, 25, -26, -28, 30, -31, 33, 34, 35, 36, 37, 38, 39, 40, -41, 43, 44, 45, 46, 47, 48,
    49, -50
};

static const int mon_long[] = {
    51, 52, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, -21, 23, 24, -1, 25, -26, -28,
    30, -31, 33, -1, 34, 35, 36, 37, -38, 40, -41, 43, 44, 45, 46, 47, 48, 49, -50
};

static const int thu_normal[] = {
    52, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
    18, 19, 20, -21, 23, 24, -1, -1, 25, -26, -28, 30, -31, 33, 34, 35, 36, 37, 38, 39, 40, -41, 43, 44, 45, 46, 47,
    48, 49, 50
};

static const int thu_normal_israel[] = {
    52, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, -21, 23, 24, -1, 25, -26, -28, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, -41, 43, 44, 45,
    46, 47, 48, 49, 50
};

static const int thu_long[] = {
    52, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
    18, 19, 20, 21, 22, 23, 24, -1, 25, -26, -28, 30, -31, 33, 34, 35, 36, 37, 38, 39, 40, -41, 43, 44, 45, 46, 47,
    48, 49, 50
};

static const int sat_short_leap[] = {
    -1, 52, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, -1, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, -41,
    43, 44, 45, 46, 47, 48, 49, -50
};

static const int sat_long_leap[] = {
    -1, 52, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, -1, 28, 29, 30, 31, 32, 33, -1, 34, 35, 36, 37, -38, 40, -41,
    43, 44, 45, 46, 47, 48, 49, -50
};

static const int mon_short_leap[] = {
    51, 52, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, -1, 28, 29, 30, 31, 32, 33, -1, 34, 35, 36, 37, -38, 40, -41, 43,
    44, 45, 46, 47, 48, 49, -50
};

static const int mon_short_leap_israel[] = {
    51, 52, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, -1, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    -41, 43, 44, 45, 46, 47, 48, 49, -50
};

static const int mon_long_leap[] = {
    51, 52, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, -1, -1, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, -41,
    43, 44, 45, 46, 47, 48, 49, 50
};

static const int mon_long_leap_israel[] = {
    51, 52, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, -1, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50
};

static const int thu_short_leap[] = {
    52, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, -1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50
};

static const int thu_long_leap[] = {
    52, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, -1, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, -50
};

/* Array sizes */
#define ARR_LEN(arr) (sizeof(arr)/sizeof((arr)[0]))

/* yearType for sedra (different from hdate yearType!) */
typedef enum {
    SEDRA_INCOMPLETE = 1,
    SEDRA_REGULAR    = 2,
    SEDRA_COMPLETE   = 3
} sedra_year_type_t;

static const int *get_sedra_array(int leap, heb_weekday_t rh_day,
                                  sedra_year_type_t ytype, int il,
                                  int *len) {
    if (leap) {
        switch (rh_day) {
        case HEB_SATURDAY:
            switch (ytype) {
            case SEDRA_INCOMPLETE: *len = ARR_LEN(sat_short_leap); return sat_short_leap;
            case SEDRA_COMPLETE:   *len = ARR_LEN(sat_long_leap);  return sat_long_leap;
            default: break;
            }
            break;
        case HEB_MONDAY:
            switch (ytype) {
            case SEDRA_INCOMPLETE:
                *len = il ? ARR_LEN(mon_short_leap_israel) : ARR_LEN(mon_short_leap);
                return il ? mon_short_leap_israel : mon_short_leap;
            case SEDRA_COMPLETE:   *len = il ? ARR_LEN(mon_long_leap_israel) : ARR_LEN(mon_long_leap);
                                   return il ? mon_long_leap_israel : mon_long_leap;
            default: break;
            }
            break;
        case HEB_TUESDAY:
            if (ytype == SEDRA_REGULAR) {
                *len = il ? ARR_LEN(mon_long_leap_israel) : ARR_LEN(mon_long_leap);
                return il ? mon_long_leap_israel : mon_long_leap;
            }
            break;
        case HEB_THURSDAY:
            switch (ytype) {
            case SEDRA_INCOMPLETE: *len = ARR_LEN(thu_short_leap); return thu_short_leap;
            case SEDRA_COMPLETE:   *len = ARR_LEN(thu_long_leap);  return thu_long_leap;
            default: break;
            }
            break;
        default: break;
        }
    } else {
        switch (rh_day) {
        case HEB_SATURDAY:
            switch (ytype) {
            case SEDRA_INCOMPLETE: *len = ARR_LEN(sat_short); return sat_short;
            case SEDRA_COMPLETE:   *len = ARR_LEN(sat_long);  return sat_long;
            default: break;
            }
            break;
        case HEB_MONDAY:
            switch (ytype) {
            case SEDRA_INCOMPLETE: *len = ARR_LEN(mon_short); return mon_short;
            case SEDRA_COMPLETE:   *len = il ? ARR_LEN(mon_short) : ARR_LEN(mon_long);
                                   return il ? mon_short : mon_long;
            default: break;
            }
            break;
        case HEB_TUESDAY:
            if (ytype == SEDRA_REGULAR) {
                *len = il ? ARR_LEN(mon_short) : ARR_LEN(mon_long);
                return il ? mon_short : mon_long;
            }
            break;
        case HEB_THURSDAY:
            switch (ytype) {
            case SEDRA_REGULAR: *len = il ? ARR_LEN(thu_normal_israel) : ARR_LEN(thu_normal);
                                return il ? thu_normal_israel : thu_normal;
            case SEDRA_COMPLETE: *len = ARR_LEN(thu_long); return thu_long;
            default: break;
            }
            break;
        default: break;
        }
    }
    /* Should not happen */
    *len = 0;
    return NULL;
}

heb_sedra_t heb_sedra_new(int year, int il) {
    heb_sedra_t s;
    s.year = year;
    s.il = il;

    int longC = heb_hdate_long_cheshvan(year);
    int shortK = heb_hdate_short_kislev(year);
    sedra_year_type_t ytype;
    if (longC && !shortK) {
        ytype = SEDRA_COMPLETE;
    } else if (!longC && shortK) {
        ytype = SEDRA_INCOMPLETE;
    } else {
        ytype = SEDRA_REGULAR;
    }

    heb_hdate_t rh;
    heb_hdate_create(&rh, year, HEB_H_TISHREI, 1);
    heb_weekday_t rhDay = heb_hdate_weekday(&rh);
    int leap = heb_hdate_is_leap_year(year);

    s.first_saturday = heb_hdate_day_on_or_before(HEB_SATURDAY, rh.abs + 6);

    int arr_len = 0;
    const int *arr = get_sedra_array(leap, rhDay, ytype, il, &arr_len);
    s.array = arr;
    s.array_len = arr_len;
    return s;
}

heb_parsha_t heb_sedra_lookup_rd(const heb_sedra_t *s, heb_rd_t rataDie) {
    heb_parsha_t p;
    memset(&p, 0, sizeof(p));

    heb_rd_t abs = heb_hdate_day_on_or_before(HEB_SATURDAY, rataDie + 6);
    int idx;

    int weekNum = (int)((abs - s->first_saturday) / 7);
    if (weekNum >= s->array_len) {
        int indexLast = s->array[s->array_len - 1];
        if (indexLast < 0) {
            idx = (-indexLast) + 2;
        } else {
            idx = indexLast + 1;
        }
    } else {
        idx = s->array[weekNum];
    }

    if (idx >= 0) {
        p.name1 = parshiot[idx];
        p.num1 = idx + 1;
        p.chag = 0;
    } else if (idx == -1) {
        p.chag = 1;
    } else {
        int p1 = -idx;
        int p2 = p1 + 1;
        p.name1 = parshiot[p1];
        p.name2 = parshiot[p2];
        p.num1 = p1 + 1;
        p.num2 = p2 + 1;
        p.chag = 0;
    }
    return p;
}

heb_parsha_t heb_sedra_lookup(const heb_sedra_t *s, const heb_hdate_t *hd) {
    return heb_sedra_lookup_rd(s, hd->abs);
}

int heb_sedra_find_parsha_num(const heb_sedra_t *s, int num,
                              heb_hdate_t *result) {
    int parshaNum = num - 1;
    if (parshaNum > 53 || (parshaNum < 0 && !is_valid_double(parshaNum))) {
        return 1; /* invalid parsha number */
    }
    for (int idx = 0; idx < s->array_len; idx++) {
        if (s->array[idx] == parshaNum) {
            *result = heb_hdate_from_rd(s->first_saturday + (heb_rd_t)idx * 7);
            return 0;
        }
    }
    return 1; /* not read in this year */
}

void heb_parsha_render(const heb_parsha_t *p, const char *locale,
                       char *buf, size_t n) {
    if (n == 0) return;
    if (p->chag) {
        buf[0] = '\0';
        return;
    }
    char name1[64];
    heb_locale_lookup(p->name1, locale, name1, sizeof(name1));
    if (p->name2) {
        char name2[64];
        heb_locale_lookup(p->name2, locale, name2, sizeof(name2));
        if (heb_locale_is_hebrew(locale)) {
            /* Use maqaf (־ U+05BE) for Hebrew */
            snprintf(buf, n, "%s\xd6\xbe%s", name1, name2);
        } else {
            snprintf(buf, n, "%s-%s", name1, name2);
        }
    } else {
        size_t len = strlen(name1);
        if (len >= n) len = n - 1;
        memcpy(buf, name1, len);
        buf[len] = '\0';
    }
}
