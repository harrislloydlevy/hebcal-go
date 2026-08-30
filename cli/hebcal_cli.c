/*
 * hebcal-c CLI - Command-line interface for testing the hebcal-c library.
 *
 * Usage:
 *   hebcal-c 2g 15 Cheshvan 5769        Convert Hebrew → Gregorian
 *   hebcal-c 2h 2008-11-13              Convert Gregorian → Hebrew
 *   hebcal-c -y 2026                    List holidays for Gregorian year
 *   hebcal-c -y 5784 --hebrew-year      List holidays for Hebrew year
 *   hebcal-c --yahrzeit 2017-01-13      Show yahrzeit dates
 *   hebcal-c --birthday 1948-03-11      Show birthday dates
 *   hebcal-c -z 40.097,-74.222,-300 2026  Zmanim for location
 *
 * License: GPL-2.0-or-later
 */
#include "hdate.h"
#include "greg.h"
#include "anniversary.h"
#include "holidays.h"
#include "sedra.h"
#include "omer.h"
#include "molad.h"
#include "zmanim.h"
#include "hebcal.h"
#include "event.h"
#include "locale.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage: %s [options] [args]\n"
        "\n"
        "Date conversion:\n"
        "  %s 2g <day> <month> <year>     Hebrew → Gregorian\n"
        "  %s 2h <YYYY-MM-DD>              Gregorian → Hebrew\n"
        "\n"
        "Calendar:\n"
        "  %s -y <year>                    Holidays for Gregorian year\n"
        "  %s -y <year> --hebrew-year       Holidays for Hebrew year\n"
        "  %s -y <year> -i                  Israel schedule\n"
        "  %s -y <year> -o                  Include Omer\n"
        "  %s -y <year> -s                  Include Sedra (Torah readings)\n"
        "  %s -y <year> -m                  Include Molad\n"
        "  %s -y <year> --he               Hebrew output\n"
        "\n"
        "Anniversaries:\n"
        "  %s --yahrzeit <YYYY-MM-DD>      Yahrzeit dates (25 years)\n"
        "  %s --birthday <YYYY-MM-DD>      Birthday dates (21 years)\n"
        "\n"
        "Zmanim:\n"
        "  %s -z <lat,lon,offset> <year>   Halachic times for location\n"
        "\n"
        "Options:\n"
        "  -i         Israel schedule\n"
        "  -o         Include Counting of the Omer\n"
        "  -s         Include Parashat HaShavua (Sedra)\n"
        "  -m         Include Molad\n"
        "  --he       Hebrew language output\n"
        "  --hebrew-year  Treat year as Hebrew year\n"
        "  --no-rc    Suppress Rosh Chodesh\n"
        "  --no-mf    Suppress minor fasts\n"
        "  --no-mod   Suppress modern holidays\n"
        "  --no-ss    Suppress special Shabbatot\n",
        prog, prog, prog, prog, prog, prog, prog, prog, prog, prog, prog, prog, prog);
}

/* Parse a Gregorian date in YYYY-MM-DD format */
static int parse_greg_date(const char *s, int *y, int *m, int *d) {
    return sscanf(s, "%d-%d-%d", y, m, d) == 3;
}

/* Parse "lat,lon,offset" for zmanim */
static int parse_location(const char *s, double *lat, double *lon, int *offset) {
    return sscanf(s, "%lf,%lf,%d", lat, lon, offset) == 3;
}

/* Print a Gregorian date */
static void print_greg(const heb_gdate_t *gd) {
    printf("%04d-%02d-%02d\n", gd->year, gd->month, gd->day);
}

/* Print a Hebrew date in English */
static void print_hebrew(const heb_hdate_t *hd, const char *locale) {
    char buf[64];
    if (strcmp(locale, "en") == 0) {
        heb_hdate_to_string(hd, buf, sizeof(buf));
    } else {
        char mname[32];
        heb_hdate_month_name(hd, locale, mname, sizeof(mname));
        snprintf(buf, sizeof(buf), "%d %s %d", hd->day, mname, hd->year);
    }
    printf("%s\n", buf);
}

/* Print a weekday name */
static const char *weekday_name(heb_weekday_t dow) {
    const char *names[] = {"Sunday", "Monday", "Tuesday", "Wednesday",
                           "Thursday", "Friday", "Saturday"};
    if (dow >= 0 && dow <= 6) return names[dow];
    return "?";
}

/* List all holidays for a year */
static int list_year(int year, int is_hebrew, int il, int omer,
                      int sedrot, int molad, const char *locale,
                      int no_rc, int no_mf, int no_mod, int no_ss) {
    heb_cal_options_t opts = heb_cal_default_options();
    opts.year = year;
    opts.is_hebrew_year = is_hebrew;
    opts.il = il;
    opts.omer = omer;
    opts.sedrot = sedrot;
    opts.molad = molad;
    opts.no_rosh_chodesh = no_rc;
    opts.no_minor_fast = no_mf;
    opts.no_modern = no_mod;
    opts.no_special_shabbat = no_ss;

    static heb_event_t events[HEB_MAX_EVENTS * 2];
    int n = heb_hebrew_calendar(&opts, events, HEB_MAX_EVENTS * 2);
    if (n < 0) {
        fprintf(stderr, "Error generating calendar\n");
        return 1;
    }

    for (int i = 0; i < n; i++) {
        heb_gdate_t gd = heb_hdate_greg(&events[i].date);
        const char *dow = weekday_name(heb_hdate_weekday(&events[i].date));

        char desc[256];
        heb_event_render(&events[i], locale, desc, sizeof(desc));

        char hdStr[64];
        heb_hdate_to_string(&events[i].date, hdStr, sizeof(hdStr));

        printf("%04d-%02d-%02d %s %s\t%s\n",
               gd.year, gd.month, gd.day, dow, hdStr, desc);
    }
    return 0;
}

/* List zmanim for a specific date */
static int list_zmanim(double lat, double lon, int utc_offset, int year, int il) {
    /* Print zmanim for a few sample dates */
    int months[] = {1, 3, 6, 9, 12};
    int days[] = {15, 15, 21, 21, 15};

    printf("Zmanim for lat=%.5f lon=%.5f UTC%+d\n\n", lat, lon, utc_offset/60);

    for (int i = 0; i < 4; i++) {
        int m = months[i];
        int d = days[i];
        heb_zmanim_t z = heb_zmanim_new(lat, lon, 0, year, m, d, utc_offset);

        double sunrise = heb_zmanim_sunrise(&z);
        double sunset = heb_zmanim_sunset(&z);
        double chatzot = heb_zmanim_chatzot(&z);
        double alot = heb_zmanim_alot_hashachar(&z);
        double tzeit = heb_zmanim_tzeit(&z, 8.5);

        char sr[16], ss[16], ch[16], al[16], tz[16];
        heb_zmanim_format_time(sunrise, utc_offset, sr, sizeof(sr));
        heb_zmanim_format_time(sunset, utc_offset, ss, sizeof(ss));
        heb_zmanim_format_time(chatzot, utc_offset, ch, sizeof(ch));
        heb_zmanim_format_time(alot, utc_offset, al, sizeof(al));
        heb_zmanim_format_time(tzeit, utc_offset, tz, sizeof(tz));

        printf("%04d-%02d-%02d:\n", year, m, d);
        printf("  Alot HaShachar:  %s\n", al);
        printf("  Sunrise:         %s\n", sr);
        printf("  Chatzot:         %s\n", ch);
        printf("  Sunset:          %s\n", ss);
        printf("  Tzeit 8.5°:      %s\n\n", tz);
    }
    return 0;
}

/* Yahrzeit or birthday listing */
static int list_anniversary(const char *dateStr, int is_yahrzeit) {
    int y, m, d;
    if (!parse_greg_date(dateStr, &y, &m, &d)) {
        fprintf(stderr, "Invalid date format: %s\n", dateStr);
        return 1;
    }
    heb_hdate_t hd = heb_hdate_from_gregorian(y, (heb_month_greg_t)m, d);

    int startYear = is_yahrzeit ? 5778 : 5753;
    int numYears = is_yahrzeit ? 25 : 21;

    printf("%s dates for %s (Hebrew date: ", 
           is_yahrzeit ? "Yahrzeit" : "Birthday",
           dateStr);
    char hdStr[64];
    heb_hdate_to_string(&hd, hdStr, sizeof(hdStr));
    printf("%s)\n\n", hdStr);

    for (int i = 0; i < numYears; i++) {
        int hyear = startYear + i;
        heb_hdate_t result;
        int rc;
        if (is_yahrzeit) {
            rc = heb_anniversary_yahrzeit(hyear, &hd, &result);
        } else {
            rc = heb_anniversary_birthday(hyear, &hd, &result);
        }
        if (rc == 0) {
            heb_gdate_t gd = heb_hdate_greg(&result);
            printf("  %d/%d/%d\n", gd.month, gd.day, gd.year);
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    const char *locale = "en";
    int il = 0, omer = 0, sedrot = 0, molad = 0;
    int is_hebrew = 0, no_rc = 0, no_mf = 0, no_mod = 0, no_ss = 0;

    /* Parse options */
    int first_arg = 1;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "2g") == 0 && i + 3 < argc) {
            /* Hebrew → Gregorian */
            int day = atoi(argv[i+1]);
            heb_hmonth_t month;
            if (heb_hdate_month_from_name(argv[i+2], &month) == 0) {
                int year = atoi(argv[i+3]);
                heb_hdate_t hd;
                heb_hdate_create(&hd, year, month, day);
                heb_gdate_t gd = heb_hdate_greg(&hd);
                printf("%04d-%02d-%02d (%s)\n", gd.year, gd.month, gd.day,
                       weekday_name(heb_hdate_weekday(&hd)));
                return 0;
            }
            fprintf(stderr, "Invalid month name: %s\n", argv[i+2]);
            return 1;
        }
        if (strcmp(argv[i], "2h") == 0 && i + 1 < argc) {
            /* Gregorian → Hebrew */
            int y, m, d;
            if (parse_greg_date(argv[i+1], &y, &m, &d)) {
                heb_hdate_t hd = heb_hdate_from_gregorian(y, (heb_month_greg_t)m, d);
                print_hebrew(&hd, locale);
                printf("  Weekday: %s\n", weekday_name(heb_hdate_weekday(&hd)));
                return 0;
            }
            fprintf(stderr, "Invalid date: %s\n", argv[i+1]);
            return 1;
        }
        if (strcmp(argv[i], "-y") == 0 && i + 1 < argc) {
            int year = atoi(argv[i+1]);
            first_arg = i + 2;
            /* Parse remaining options */
            for (int j = i + 2; j < argc; j++) {
                if (strcmp(argv[j], "-i") == 0) il = 1;
                else if (strcmp(argv[j], "-o") == 0) omer = 1;
                else if (strcmp(argv[j], "-s") == 0) sedrot = 1;
                else if (strcmp(argv[j], "-m") == 0) molad = 1;
                else if (strcmp(argv[j], "--he") == 0) locale = "he";
                else if (strcmp(argv[j], "--hebrew-year") == 0) is_hebrew = 1;
                else if (strcmp(argv[j], "--no-rc") == 0) no_rc = 1;
                else if (strcmp(argv[j], "--no-mf") == 0) no_mf = 1;
                else if (strcmp(argv[j], "--no-mod") == 0) no_mod = 1;
                else if (strcmp(argv[j], "--no-ss") == 0) no_ss = 1;
                else if (strcmp(argv[j], "-z") == 0 && j + 1 < argc) {
                    double lat, lon;
                    int offset;
                    if (parse_location(argv[j+1], &lat, &lon, &offset)) {
                        j++;
                        int zyear = (j + 1 < argc) ? atoi(argv[j+1]) : 2026;
                        return list_zmanim(lat, lon, offset, zyear, il);
                    }
                }
            }
            return list_year(year, is_hebrew, il, omer, sedrot, molad,
                             locale, no_rc, no_mf, no_mod, no_ss);
        }
        if (strcmp(argv[i], "--yahrzeit") == 0 && i + 1 < argc) {
            return list_anniversary(argv[i+1], 1);
        }
        if (strcmp(argv[i], "--birthday") == 0 && i + 1 < argc) {
            return list_anniversary(argv[i+1], 0);
        }
        if (strcmp(argv[i], "-z") == 0 && i + 2 < argc) {
            double lat, lon;
            int offset;
            if (parse_location(argv[i+1], &lat, &lon, &offset)) {
                int zyear = atoi(argv[i+2]);
                return list_zmanim(lat, lon, offset, zyear, il);
            }
            fprintf(stderr, "Invalid location format: %s\n", argv[i+1]);
            return 1;
        }
        if (strcmp(argv[i], "help") == 0 || strcmp(argv[i], "--help") == 0) {
            usage(argv[0]);
            return 0;
        }
    }

    usage(argv[0]);
    return 1;
}
