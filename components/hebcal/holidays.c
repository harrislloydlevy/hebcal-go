/*
 * hebcal-c - Holiday definitions and lookup.
 * Ported from github.com/hebcal/hebcal-go/hebcal/holidays.go (Go).
 *
 * License: GPL-2.0-or-later
 */
#include "holidays.h"
#include "sedra.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* Emoji constants (as macros for use in static initializers) */
#define chanukahEmoji "\xf0\x9f\x95\x8a" /* 🕎 */
#define sukkotEmoji   "\xf0\x9f\x8c\xbf\xf0\x9f\x8d\x8b" /* 🌿🍋 */

/* --- Static holiday definitions (fixed Hebrew dates) --- */
typedef struct {
    heb_hmonth_t month;
    int day;
    const char *desc;
    heb_holiday_flags_t flags;
    const char *emoji;
    int chm_day;
} holiday_def_t;

static const holiday_def_t static_holidays[] = {
    {HEB_H_TISHREI, 2, "Rosh Hashana II",
     HEB_F_CHAG|HEB_F_YOM_TOV_ENDS, "\xf0\x9f\x8d\x8f\xf0\x9f\x8d\x86", 0},
    {HEB_H_TISHREI, 9, "Erev Yom Kippur",
     HEB_F_EREV|HEB_F_LIGHT_CANDLES, NULL, 0},
    {HEB_H_TISHREI, 10, "Yom Kippur",
     HEB_F_CHAG|HEB_F_MAJOR_FAST|HEB_F_YOM_TOV_ENDS, NULL, 0},
    /* Erev Sukkot (Diaspora) */
    {HEB_H_TISHREI, 14, "Erev Sukkot",
     HEB_F_CHUL_ONLY|HEB_F_EREV|HEB_F_LIGHT_CANDLES, sukkotEmoji, 0},
    /* Erev Sukkot (Israel) */
    {HEB_H_TISHREI, 14, "Erev Sukkot",
     HEB_F_IL_ONLY|HEB_F_EREV|HEB_F_LIGHT_CANDLES, sukkotEmoji, 0},
    /* Sukkot (Diaspora) */
    {HEB_H_TISHREI, 15, "Sukkot I",
     HEB_F_CHUL_ONLY|HEB_F_CHAG|HEB_F_LIGHT_CANDLES_TZEIS, sukkotEmoji, 0},
    {HEB_H_TISHREI, 16, "Sukkot II",
     HEB_F_CHUL_ONLY|HEB_F_CHAG|HEB_F_YOM_TOV_ENDS, sukkotEmoji, 0},
    {HEB_H_TISHREI, 17, "Sukkot III (CH''M)",
     HEB_F_CHUL_ONLY|HEB_F_CHOL_HAMOED, sukkotEmoji, 1},
    {HEB_H_TISHREI, 18, "Sukkot IV (CH''M)",
     HEB_F_CHUL_ONLY|HEB_F_CHOL_HAMOED, sukkotEmoji, 2},
    {HEB_H_TISHREI, 19, "Sukkot V (CH''M)",
     HEB_F_CHUL_ONLY|HEB_F_CHOL_HAMOED, sukkotEmoji, 3},
    {HEB_H_TISHREI, 20, "Sukkot VI (CH''M)",
     HEB_F_CHUL_ONLY|HEB_F_CHOL_HAMOED, sukkotEmoji, 4},
    {HEB_H_TISHREI, 22, "Shmini Atzeret",
     HEB_F_CHUL_ONLY|HEB_F_CHAG|HEB_F_LIGHT_CANDLES_TZEIS, NULL, 0},
    {HEB_H_TISHREI, 23, "Simchat Torah",
     HEB_F_CHUL_ONLY|HEB_F_CHAG|HEB_F_YOM_TOV_ENDS, NULL, 0},
    /* Sukkot (Israel) */
    {HEB_H_TISHREI, 15, "Sukkot I",
     HEB_F_IL_ONLY|HEB_F_CHAG|HEB_F_YOM_TOV_ENDS, sukkotEmoji, 0},
    {HEB_H_TISHREI, 16, "Sukkot II (CH''M)",
     HEB_F_IL_ONLY|HEB_F_CHOL_HAMOED, sukkotEmoji, 1},
    {HEB_H_TISHREI, 17, "Sukkot III (CH''M)",
     HEB_F_IL_ONLY|HEB_F_CHOL_HAMOED, sukkotEmoji, 2},
    {HEB_H_TISHREI, 18, "Sukkot IV (CH''M)",
     HEB_F_IL_ONLY|HEB_F_CHOL_HAMOED, sukkotEmoji, 3},
    {HEB_H_TISHREI, 19, "Sukkot V (CH''M)",
     HEB_F_IL_ONLY|HEB_F_CHOL_HAMOED, sukkotEmoji, 4},
    {HEB_H_TISHREI, 20, "Sukkot VI (CH''M)",
     HEB_F_IL_ONLY|HEB_F_CHOL_HAMOED, sukkotEmoji, 5},
    {HEB_H_TISHREI, 22, "Shmini Atzeret",
     HEB_F_IL_ONLY|HEB_F_CHAG|HEB_F_YOM_TOV_ENDS, NULL, 0},
    {HEB_H_TISHREI, 21, "Sukkot VII (Hoshana Raba)",
     HEB_F_LIGHT_CANDLES|HEB_F_CHOL_HAMOED, sukkotEmoji, -1},
    /* Chanukah */
    {HEB_H_KISLEV, 24, "Chanukah: 1 Candle",
     HEB_F_EREV|HEB_F_MINOR_HOLIDAY|HEB_F_CHANUKAH_CANDLES, chanukahEmoji, 0},
    {HEB_H_TEVET, 10, "Asara B'Tevet",
     HEB_F_MINOR_FAST, NULL, 0},
    {HEB_H_SHVAT, 15, "Tu BiShvat",
     HEB_F_MINOR_HOLIDAY, "\xf0\x9f\x8c\xb3", 0},
    /* Purim */
    {HEB_H_ADAR2, 13, "Erev Purim",
     HEB_F_EREV|HEB_F_MINOR_HOLIDAY, NULL, 0},
    {HEB_H_ADAR2, 14, "Purim",
     HEB_F_MINOR_HOLIDAY, NULL, 0},
    {HEB_H_ADAR2, 15, "Shushan Purim",
     HEB_F_MINOR_HOLIDAY, NULL, 0},
    /* Pesach (Diaspora) */
    {HEB_H_NISAN, 14, "Erev Pesach",
     HEB_F_CHUL_ONLY|HEB_F_EREV|HEB_F_LIGHT_CANDLES, NULL, 0},
    {HEB_H_NISAN, 14, "Erev Pesach",
     HEB_F_IL_ONLY|HEB_F_EREV|HEB_F_LIGHT_CANDLES, NULL, 0},
    {HEB_H_NISAN, 15, "Pesach I",
     HEB_F_IL_ONLY|HEB_F_CHAG|HEB_F_YOM_TOV_ENDS, NULL, 0},
    {HEB_H_NISAN, 16, "Pesach II (CH''M)",
     HEB_F_IL_ONLY|HEB_F_CHOL_HAMOED, NULL, 1},
    {HEB_H_NISAN, 17, "Pesach III (CH''M)",
     HEB_F_IL_ONLY|HEB_F_CHOL_HAMOED, NULL, 2},
    {HEB_H_NISAN, 18, "Pesach IV (CH''M)",
     HEB_F_IL_ONLY|HEB_F_CHOL_HAMOED, NULL, 3},
    {HEB_H_NISAN, 19, "Pesach V (CH''M)",
     HEB_F_IL_ONLY|HEB_F_CHOL_HAMOED, NULL, 4},
    {HEB_H_NISAN, 20, "Pesach VI (CH''M)",
     HEB_F_IL_ONLY|HEB_F_CHOL_HAMOED|HEB_F_LIGHT_CANDLES, NULL, 5},
    {HEB_H_NISAN, 21, "Pesach VII",
     HEB_F_IL_ONLY|HEB_F_CHAG|HEB_F_YOM_TOV_ENDS, NULL, 0},
    {HEB_H_NISAN, 15, "Pesach I",
     HEB_F_CHUL_ONLY|HEB_F_CHAG|HEB_F_LIGHT_CANDLES_TZEIS, NULL, 0},
    {HEB_H_NISAN, 16, "Pesach II",
     HEB_F_CHUL_ONLY|HEB_F_CHAG|HEB_F_YOM_TOV_ENDS, NULL, 0},
    {HEB_H_NISAN, 17, "Pesach III (CH''M)",
     HEB_F_CHUL_ONLY|HEB_F_CHOL_HAMOED, NULL, 1},
    {HEB_H_NISAN, 18, "Pesach IV (CH''M)",
     HEB_F_CHUL_ONLY|HEB_F_CHOL_HAMOED, NULL, 2},
    {HEB_H_NISAN, 19, "Pesach V (CH''M)",
     HEB_F_CHUL_ONLY|HEB_F_CHOL_HAMOED, NULL, 3},
    {HEB_H_NISAN, 20, "Pesach VI (CH''M)",
     HEB_F_CHUL_ONLY|HEB_F_CHOL_HAMOED|HEB_F_LIGHT_CANDLES, NULL, 4},
    {HEB_H_NISAN, 21, "Pesach VII",
     HEB_F_CHUL_ONLY|HEB_F_CHAG|HEB_F_LIGHT_CANDLES_TZEIS, NULL, 0},
    {HEB_H_NISAN, 22, "Pesach VIII",
     HEB_F_CHUL_ONLY|HEB_F_CHAG|HEB_F_YOM_TOV_ENDS, NULL, 0},
    /* Other holidays */
    {HEB_H_IYYAR, 14, "Pesach Sheni",
     HEB_F_MINOR_HOLIDAY, NULL, 0},
    {HEB_H_IYYAR, 18, "Lag BaOmer",
     HEB_F_MINOR_HOLIDAY, "\xf0\x9f\x94\xa5", 0},
    {HEB_H_SIVAN, 5, "Erev Shavuot",
     HEB_F_EREV|HEB_F_LIGHT_CANDLES, NULL, 0},
    {HEB_H_SIVAN, 6, "Shavuot",
     HEB_F_IL_ONLY|HEB_F_CHAG|HEB_F_YOM_TOV_ENDS, NULL, 0},
    {HEB_H_SIVAN, 6, "Shavuot I",
     HEB_F_CHUL_ONLY|HEB_F_CHAG|HEB_F_LIGHT_CANDLES_TZEIS, NULL, 0},
    {HEB_H_SIVAN, 7, "Shavuot II",
     HEB_F_CHUL_ONLY|HEB_F_CHAG|HEB_F_YOM_TOV_ENDS, NULL, 0},
    {HEB_H_AV, 15, "Tu B'Av",
     HEB_F_MINOR_HOLIDAY, "\xe2\x9d\xa4\xef\xb8\x8f", 0},
    {HEB_H_ELUL, 1, "Rosh Hashana LaBehemot",
     HEB_F_MINOR_HOLIDAY, "\xf0\x9f\x90\x91", 0},
    {HEB_H_ELUL, 29, "Erev Rosh Hashana",
     HEB_F_EREV|HEB_F_LIGHT_CANDLES, "\xf0\x9f\x8d\x8f\xf0\x9f\x8d\x86", 0},
};

#define NUM_STATIC_HOLIDAYS (sizeof(static_holidays)/sizeof(static_holidays[0]))

/* --- Modern holiday definitions --- */
typedef struct {
    int first_year;
    heb_hmonth_t month;
    int day;
    const char *desc;
    int chul;
    int suppress_emoji;
    int sat_postpone_to_sun;
    int fri_postpone_to_sun;
    int fri_sat_move_to_thu;
} modern_holiday_def_t;

static const modern_holiday_def_t static_modern_holidays[] = {
    {5727, HEB_H_IYYAR, 28, "Yom Yerushalayim", 1, 0, 0, 0, 0},
    {5737, HEB_H_KISLEV, 6, "Ben-Gurion Day", 0, 0, 1, 1, 0},
    {5750, HEB_H_SHVAT, 30, "Family Day", 0, 1, 0, 0, 0},
    {5758, HEB_H_CHESHVAN, 12, "Yitzhak Rabin Memorial Day", 0, 0, 0, 0, 1},
    {5764, HEB_H_IYYAR, 10, "Herzl Day", 0, 0, 1, 0, 0},
    {5765, HEB_H_TAMUZ, 29, "Jabotinsky Day", 0, 0, 1, 0, 0},
    {5769, HEB_H_CHESHVAN, 29, "Sigd", 1, 1, 0, 0, 1},
    {5777, HEB_H_NISAN, 10, "Yom HaAliyah", 1, 0, 0, 0, 0},
    {5777, HEB_H_CHESHVAN, 7, "Yom HaAliyah School Observance", 0, 0, 0, 0, 0},
    {5773, HEB_H_TEVET, 21, "Hebrew Language Day", 0, 0, 0, 0, 1},
};

#define NUM_MODERN_HOLIDAYS (sizeof(static_modern_holidays)/sizeof(static_modern_holidays[0]))

/* --- Helper functions for variable holidays --- */

static heb_hdate_t tzom_gedaliah_date(heb_hdate_t rh) {
    int offset = 0;
    if (heb_hdate_weekday(&rh) == HEB_THURSDAY) offset = 1;
    heb_hdate_t hd;
    heb_hdate_create(&hd, rh.year, HEB_H_TISHREI, 3 + offset);
    return hd;
}

static heb_hdate_t taanit_esther_date(heb_hdate_t pesach) {
    int64_t offset = 31;
    if (heb_hdate_weekday(&pesach) == HEB_TUESDAY) offset = 33;
    return heb_hdate_from_rd(pesach.abs - offset);
}

static heb_hdate_t taanit_bechorot_date(heb_hdate_t pesach) {
    heb_hdate_t prev = heb_hdate_prev(&pesach);
    if (heb_hdate_weekday(&prev) == HEB_SATURDAY) {
        return heb_hdate_from_rd(heb_hdate_day_on_or_before(HEB_THURSDAY, pesach.abs));
    }
    heb_hdate_t hd;
    heb_hdate_create(&hd, pesach.year, HEB_H_NISAN, 14);
    return hd;
}

static heb_hdate_t next_month_start(int year, heb_hmonth_t month) {
    int monthsInYear = heb_hdate_months_in_year(year);
    heb_hmonth_t nextMonth = (heb_hmonth_t)((int)month + 1);
    if ((int)month == monthsInYear) nextMonth = HEB_H_NISAN;
    heb_hdate_t hd;
    heb_hdate_create(&hd, year, nextMonth, 1);
    return hd;
}

static void next_month_name(int year, heb_hmonth_t month,
                             char *buf, size_t n) {
    int monthsInYear = heb_hdate_months_in_year(year);
    heb_hmonth_t nextMonth = (heb_hmonth_t)((int)month + 1);
    if ((int)month == monthsInYear) nextMonth = HEB_H_NISAN;
    heb_hdate_t hd;
    heb_hdate_create(&hd, year, nextMonth, 1);
    heb_hdate_month_name(&hd, "en", buf, n);
}

/* Birkat Hachamah appears once every 28 years */
static heb_rd_t get_birkat_hachama(int year) {
    heb_hmonth_t startMonth = HEB_H_NISAN;
    int startDay = 1;
    if (heb_hdate_is_leap_year(year)) {
        startMonth = HEB_H_ADAR2;
        startDay = 20;
    }
    heb_rd_t baseRD = heb_hdate_to_rd(year, startMonth, startDay);
    for (int day = 0; day <= 40; day++) {
        heb_rd_t rataDie = baseRD + day;
        int64_t elapsed = rataDie + 1373429;
        if (elapsed % 10227 == 172) return rataDie;
    }
    return 0;
}

/* --- Event append helper --- */
static int append_event(heb_event_t *events, int *count, int max,
                         heb_hdate_t date, const char *desc,
                         heb_holiday_flags_t flags, const char *emoji,
                         int chm_day, int chanukah_day) {
    if (*count >= max) return 0;
    heb_event_t *ev = &events[*count];
    ev->date = date;
    ev->desc = desc;
    ev->flags = flags;
    ev->emoji = emoji;
    ev->chol_hamoed_day = chm_day;
    ev->chanukah_day = chanukah_day;
    (*count)++;
    return 1;
}

/* --- Comparison for sorting events by date --- */
static int compare_events(const void *a, const void *b) {
    const heb_event_t *ea = (const heb_event_t *)a;
    const heb_event_t *eb = (const heb_event_t *)b;
    if (ea->date.abs != eb->date.abs)
        return (ea->date.abs < eb->date.abs) ? -1 : 1;
    return strcmp(ea->desc, eb->desc);
}

/* --- Main year generation --- */
int heb_holidays_for_year(int year, int il,
                           heb_event_t *events, int max_events) {
    int count = 0;

    /* Static holidays */
    for (size_t i = 0; i < NUM_STATIC_HOLIDAYS; i++) {
        const holiday_def_t *h = &static_holidays[i];
        heb_hdate_t hd;
        heb_hdate_create(&hd, year, h->month, h->day);
        append_event(events, &count, max_events, hd, h->desc,
                     h->flags, h->emoji, h->chm_day, 0);
    }

    /* Variable holidays */
    heb_hdate_t rosh_hashana;
    heb_hdate_create(&rosh_hashana, year, HEB_H_TISHREI, 1);
    heb_hdate_t next_rh;
    heb_hdate_create(&next_rh, year + 1, HEB_H_TISHREI, 1);
    heb_hdate_t pesach;
    heb_hdate_create(&pesach, year, HEB_H_NISAN, 15);
    int64_t pesachAbs = pesach.abs;

    char rhDesc[64];
    snprintf(rhDesc, sizeof(rhDesc), "Rosh Hashana %d", year);
    append_event(events, &count, max_events, rosh_hashana, rhDesc,
                 HEB_F_CHAG|HEB_F_LIGHT_CANDLES_TZEIS,
                 "\xf0\x9f\x8d\x8f\xf0\x9f\x8d\x86", 0, 0);

    append_event(events, &count, max_events,
                 heb_hdate_from_rd(heb_hdate_day_on_or_before(HEB_SATURDAY, 7 + rosh_hashana.abs)),
                 "Shabbat Shuva", HEB_F_SPECIAL_SHABBAT, NULL, 0, 0);

    append_event(events, &count, max_events,
                 tzom_gedaliah_date(rosh_hashana),
                 "Tzom Gedaliah", HEB_F_MINOR_FAST, NULL, 0, 0);

    append_event(events, &count, max_events,
                 heb_hdate_from_rd(heb_hdate_day_on_or_before(HEB_SATURDAY, pesachAbs - 43)),
                 "Shabbat Shekalim", HEB_F_SPECIAL_SHABBAT, NULL, 0, 0);

    append_event(events, &count, max_events,
                 heb_hdate_from_rd(heb_hdate_day_on_or_before(HEB_SATURDAY, pesachAbs - 30)),
                 "Shabbat Zachor", HEB_F_SPECIAL_SHABBAT, NULL, 0, 0);

    append_event(events, &count, max_events,
                 taanit_esther_date(pesach),
                 "Ta'anit Esther", HEB_F_MINOR_FAST, NULL, 0, 0);

    append_event(events, &count, max_events,
                 heb_hdate_from_rd(heb_hdate_day_on_or_before(HEB_SATURDAY, pesachAbs - 14) - 7),
                 "Shabbat Parah", HEB_F_SPECIAL_SHABBAT, NULL, 0, 0);

    append_event(events, &count, max_events,
                 heb_hdate_from_rd(heb_hdate_day_on_or_before(HEB_SATURDAY, pesachAbs - 14)),
                 "Shabbat HaChodesh", HEB_F_SPECIAL_SHABBAT, NULL, 0, 0);

    append_event(events, &count, max_events,
                 heb_hdate_from_rd(heb_hdate_day_on_or_before(HEB_SATURDAY, pesachAbs - 1)),
                 "Shabbat HaGadol", HEB_F_SPECIAL_SHABBAT, NULL, 0, 0);

    append_event(events, &count, max_events,
                 taanit_bechorot_date(pesach),
                 "Ta'anit Bechorot", HEB_F_MINOR_FAST, NULL, 0, 0);

    append_event(events, &count, max_events,
                 heb_hdate_from_rd(heb_hdate_day_on_or_before(HEB_SATURDAY, next_rh.abs - 4)),
                 "Leil Selichot", HEB_F_MINOR_HOLIDAY,
                 "\xf0\x9f\x95\x8c", 0, 0);

    /* Purim Meshulash */
    if (heb_hdate_weekday(&pesach) == HEB_SUNDAY) {
        heb_hdate_t hd;
        heb_hdate_create(&hd, year, HEB_H_ADAR2, 16);
        append_event(events, &count, max_events, hd,
                      "Purim Meshulash", HEB_F_MINOR_HOLIDAY, NULL, 0, 0);
    }

    /* Purim Katan (leap years only) */
    if (heb_hdate_is_leap_year(year)) {
        heb_hdate_t hd1, hd2;
        heb_hdate_create(&hd1, year, HEB_H_ADAR1, 14);
        heb_hdate_create(&hd2, year, HEB_H_ADAR1, 15);
        append_event(events, &count, max_events, hd1,
                      "Purim Katan", HEB_F_MINOR_HOLIDAY, NULL, 0, 0);
        append_event(events, &count, max_events, hd2,
                      "Shushan Purim Katan", HEB_F_MINOR_HOLIDAY, NULL, 0, 0);
    }

    /* Chanukah candles 2-6 */
    for (int candles = 2; candles <= 6; candles++) {
        heb_hdate_t hd;
        heb_hdate_create(&hd, year, HEB_H_KISLEV, 23 + candles);
        char desc[32];
        snprintf(desc, sizeof(desc), "Chanukah: %d Candles", candles);
        append_event(events, &count, max_events, hd, desc,
                      HEB_F_MINOR_HOLIDAY|HEB_F_CHANUKAH_CANDLES,
                      chanukahEmoji, 0, candles - 1);
    }

    /* Chanukah 7, 8, and 8th Day */
    heb_hdate_t chanukah7;
    if (heb_hdate_short_kislev(year)) {
        heb_hdate_create(&chanukah7, year, HEB_H_TEVET, 1);
    } else {
        heb_hdate_create(&chanukah7, year, HEB_H_KISLEV, 30);
    }
    heb_hdate_t chanukah8 = heb_hdate_next(&chanukah7);

    append_event(events, &count, max_events, chanukah7,
                 "Chanukah: 7 Candles",
                 HEB_F_MINOR_HOLIDAY|HEB_F_CHANUKAH_CANDLES, chanukahEmoji, 0, 6);
    append_event(events, &count, max_events, chanukah8,
                 "Chanukah: 8 Candles",
                 HEB_F_MINOR_HOLIDAY|HEB_F_CHANUKAH_CANDLES, chanukahEmoji, 0, 7);
    append_event(events, &count, max_events, heb_hdate_next(&chanukah8),
                 "Chanukah: 8th Day",
                 HEB_F_MINOR_HOLIDAY, chanukahEmoji, 0, 8);

    /* Chag HaBanot on Chanukah 7 */
    append_event(events, &count, max_events, chanukah7,
                 "Chag HaBanot", HEB_F_MINOR_HOLIDAY, NULL, 0, 0);

    /* Tzom Tammuz */
    heb_hdate_t tamuz17;
    heb_hdate_create(&tamuz17, year, HEB_H_TAMUZ, 17);
    if (heb_hdate_weekday(&tamuz17) == HEB_SATURDAY)
        tamuz17 = heb_hdate_next(&tamuz17);
    append_event(events, &count, max_events, tamuz17,
                 "Tzom Tammuz", HEB_F_MINOR_FAST, NULL, 0, 0);

    /* Tisha B'Av */
    heb_hdate_t av9dt;
    heb_hdate_create(&av9dt, year, HEB_H_AV, 9);
    const char *av9title = "Tish'a B'Av";
    if (heb_hdate_weekday(&av9dt) == HEB_SATURDAY) {
        av9dt = heb_hdate_next(&av9dt);
        av9title = "Tish'a B'Av (observed)";
    }
    int64_t av9abs = av9dt.abs;
    append_event(events, &count, max_events,
                 heb_hdate_from_rd(heb_hdate_day_on_or_before(HEB_SATURDAY, av9abs)),
                 "Shabbat Chazon", HEB_F_SPECIAL_SHABBAT, NULL, 0, 0);
    append_event(events, &count, max_events, heb_hdate_prev(&av9dt),
                 "Erev Tish'a B'Av", HEB_F_EREV|HEB_F_MAJOR_FAST, NULL, 0, 0);
    append_event(events, &count, max_events, av9dt, av9title,
                 HEB_F_MAJOR_FAST, NULL, 0, 0);
    append_event(events, &count, max_events,
                 heb_hdate_from_rd(heb_hdate_day_on_or_before(HEB_SATURDAY, av9abs + 7)),
                 "Shabbat Nachamu", HEB_F_SPECIAL_SHABBAT, NULL, 0, 0);

    /* Modern holidays: Yom HaAtzma'ut and Yom HaZikaron */
    if (year >= 5708) {
        int day;
        heb_weekday_t pesachDow = heb_hdate_weekday(&pesach);
        if (pesachDow == HEB_SUNDAY) day = 2;
        else if (pesachDow == HEB_SATURDAY) day = 3;
        else if (year < 5764) day = 4;
        else if (pesachDow == HEB_TUESDAY) day = 5;
        else day = 4;

        heb_hdate_t tmpDate;
        heb_hdate_create(&tmpDate, year, HEB_H_IYYAR, day);
        append_event(events, &count, max_events, tmpDate,
                     "Yom HaZikaron", HEB_F_MODERN_HOLIDAY,
                     "\xf0\x9f\x87\xae\xf0\x9f\x87\xb1", 0, 0);
        append_event(events, &count, max_events, heb_hdate_next(&tmpDate),
                     "Yom HaAtzma'ut", HEB_F_MODERN_HOLIDAY,
                     "\xf0\x9f\x87\xae\xf0\x9f\x87\xb1", 0, 0);
    }

    /* Yom HaShoah */
    if (year >= 5711) {
        heb_hdate_t nisan27dt;
        heb_hdate_create(&nisan27dt, year, HEB_H_NISAN, 27);
        heb_weekday_t dow = heb_hdate_weekday(&nisan27dt);
        if (dow == HEB_FRIDAY)
            nisan27dt = heb_hdate_prev(&nisan27dt);
        else if (dow == HEB_SUNDAY)
            nisan27dt = heb_hdate_next(&nisan27dt);
        append_event(events, &count, max_events, nisan27dt,
                     "Yom HaShoah", HEB_F_MODERN_HOLIDAY, NULL, 0, 0);
    }

    /* Static modern holidays */
    for (size_t i = 0; i < NUM_MODERN_HOLIDAYS; i++) {
        const modern_holiday_def_t *h = &static_modern_holidays[i];
        if (year >= h->first_year) {
            const char *emoji = h->suppress_emoji ? NULL : "\xf0\x9f\x87\xae\xf0\x9f\x87\xb1";
            heb_hdate_t hd;
            heb_hdate_create(&hd, year, h->month, h->day);
            heb_weekday_t dow = heb_hdate_weekday(&hd);
            if (h->fri_sat_move_to_thu && (dow == HEB_FRIDAY || dow == HEB_SATURDAY)) {
                hd = heb_hdate_on_or_before(&hd, HEB_THURSDAY);
            } else if (h->fri_postpone_to_sun && dow == HEB_FRIDAY) {
                hd = heb_hdate_next(&hd);
                hd = heb_hdate_next(&hd);
            } else if (h->sat_postpone_to_sun && dow == HEB_SATURDAY) {
                hd = heb_hdate_next(&hd);
            }
            heb_holiday_flags_t flags = HEB_F_MODERN_HOLIDAY;
            if (!h->chul) flags |= HEB_F_IL_ONLY;
            append_event(events, &count, max_events, hd, h->desc,
                         flags, emoji, 0, 0);
        }
    }

    /* Rosh Chodesh */
    int monthsInYear = heb_hdate_months_in_year(year);
    for (int i = 1; i <= monthsInYear; i++) {
        int isNisan = (i == 1);
        int prevMonthNum = i - 1;
        int prevMonthYear = year;
        if (isNisan) {
            prevMonthYear = year - 1;
            prevMonthNum = heb_hdate_months_in_year(prevMonthYear);
        }
        heb_hmonth_t prevMonth = (heb_hmonth_t)prevMonthNum;
        int prevMonthNumDays = heb_hdate_days_in_month(prevMonth, prevMonthYear);
        heb_hmonth_t month = (heb_hmonth_t)i;
        char monthName[32];
        {
            heb_hdate_t tmp;
            heb_hdate_create(&tmp, year, month, 1);
            heb_hdate_month_name(&tmp, "en", monthName, sizeof(monthName));
        }
        char desc[64];
        snprintf(desc, sizeof(desc), "Rosh Chodesh %s", monthName);

        if (prevMonthNumDays == 30) {
            heb_hdate_t hd1, hd2;
            heb_hdate_create(&hd1, year, prevMonth, 30);
            heb_hdate_create(&hd2, year, month, 1);
            append_event(events, &count, max_events, hd1, desc,
                         HEB_F_ROSH_CHODESH, NULL, 0, 0);
            append_event(events, &count, max_events, hd2, desc,
                         HEB_F_ROSH_CHODESH, NULL, 0, 0);
        } else if (month != HEB_H_TISHREI) {
            heb_hdate_t hd;
            heb_hdate_create(&hd, year, month, 1);
            append_event(events, &count, max_events, hd, desc,
                         HEB_F_ROSH_CHODESH, NULL, 0, 0);
        }

        /* Shabbat Mevarchim Chodesh */
        if (month == HEB_H_ELUL) continue;
        char nextName[32];
        next_month_name(year, month, nextName, sizeof(nextName));
        char smDesc[96];
        snprintf(smDesc, sizeof(smDesc), "Shabbat Mevarchim Chodesh %s", nextName);
        heb_hdate_t hd29;
        heb_hdate_create(&hd29, year, month, 29);
        heb_hdate_t smDate = heb_hdate_on_or_before(&hd29, HEB_SATURDAY);
        append_event(events, &count, max_events, smDate, smDesc,
                     HEB_F_SHABBAT_MEVARCHIM, NULL, 0, 0);
    }

    /* Yom Kippur Katan */
    for (int m = HEB_H_IYYAR; m <= monthsInYear; m++) {
        char nextName[32];
        heb_hmonth_t nextM;
        {
            int monthsInYr = heb_hdate_months_in_year(year);
            nextM = (heb_hmonth_t)(m + 1);
            if (m == monthsInYr) nextM = HEB_H_NISAN;
            heb_hdate_t tmp;
            heb_hdate_create(&tmp, year, nextM, 1);
            heb_hdate_month_name(&tmp, "en", nextName, sizeof(nextName));
        }
        if (nextM == HEB_H_TISHREI || nextM == HEB_H_CHESHVAN || nextM == HEB_H_TEVET)
            continue;
        heb_hdate_t ykk;
        heb_hdate_create(&ykk, year, (heb_hmonth_t)m, 29);
        heb_weekday_t dow = heb_hdate_weekday(&ykk);
        if (dow == HEB_FRIDAY || dow == HEB_SATURDAY)
            ykk = heb_hdate_on_or_before(&ykk, HEB_THURSDAY);
        char ykkDesc[96];
        snprintf(ykkDesc, sizeof(ykkDesc), "Yom Kippur Katan %s", nextName);
        append_event(events, &count, max_events, ykk, ykkDesc,
                     HEB_F_MINOR_FAST|HEB_F_YOM_KIPPUR_KATAN, NULL, 0, 0);
    }

    /* Shabbat Shirah */
    {
        heb_sedra_t sedra = heb_sedra_new(year, 0);
        heb_hdate_t beshalachHd;
        if (heb_sedra_find_parsha_num(&sedra, 16, &beshalachHd) == 0) {
            append_event(events, &count, max_events, beshalachHd,
                         "Shabbat Shirah", HEB_F_SPECIAL_SHABBAT, NULL, 0, 0);
        }
    }

    /* Birkat Hachamah */
    {
        heb_rd_t birkatRD = get_birkat_hachama(year);
        if (birkatRD != 0) {
            append_event(events, &count, max_events,
                         heb_hdate_from_rd(birkatRD),
                         "Birkat Hachamah", HEB_F_MINOR_HOLIDAY,
                         "\xe2\x98\x80\xef\xb8\x8f", 0, 0);
        }
    }

    /* Sort by date */
    qsort(events, count, sizeof(heb_event_t), compare_events);

    /* Filter by il/diaspora */
    int filtered = 0;
    for (int i = 0; i < count; i++) {
        if ((il && !(events[i].flags & HEB_F_CHUL_ONLY)) ||
            (!il && !(events[i].flags & HEB_F_IL_ONLY))) {
            if (filtered != i) events[filtered] = events[i];
            filtered++;
        }
    }

    return filtered;
}

/* --- Lookup for a specific date --- */
int heb_holidays_for_date(const heb_hdate_t *hd, int il,
                           heb_event_t *events, int max_events) {
    /* Get all holidays for the year and filter by date */
    static heb_event_t yearEvents[HEB_MAX_EVENTS];
    int n = heb_holidays_for_year(hd->year, il, yearEvents, HEB_MAX_EVENTS);

    int count = 0;
    for (int i = 0; i < n && count < max_events; i++) {
        if (yearEvents[i].date.abs == hd->abs) {
            events[count] = yearEvents[i];
            count++;
        }
    }
    return count;
}
