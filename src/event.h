/*
 * hebcal-c - Calendar event types and flags.
 * Ported from github.com/hebcal/hebcal-go/event (Go).
 *
 * License: GPL-2.0-or-later
 */
#ifndef HEB_EVENT_H
#define HEB_EVENT_H

#include "hebcal_types.h"
#include <stdint.h>
#include <stddef.h>

/* Holiday flag bitmask (mirrors Go's event.HolidayFlags). */
typedef uint32_t heb_holiday_flags_t;

#define HEB_F_CHAG               (1u << 0)
#define HEB_F_LIGHT_CANDLES      (1u << 1)
#define HEB_F_YOM_TOV_ENDS       (1u << 2)
#define HEB_F_CHUL_ONLY          (1u << 3)
#define HEB_F_IL_ONLY            (1u << 4)
#define HEB_F_LIGHT_CANDLES_TZEIS (1u << 5)
#define HEB_F_CHANUKAH_CANDLES   (1u << 6)
#define HEB_F_ROSH_CHODESH       (1u << 7)
#define HEB_F_MINOR_FAST         (1u << 8)
#define HEB_F_SPECIAL_SHABBAT    (1u << 9)
#define HEB_F_PARSHA_HASHAVUA    (1u << 10)
#define HEB_F_DAF_YOMI           (1u << 11)
#define HEB_F_OMER_COUNT         (1u << 12)
#define HEB_F_MODERN_HOLIDAY     (1u << 13)
#define HEB_F_MAJOR_FAST         (1u << 14)
#define HEB_F_SHABBAT_MEVARCHIM  (1u << 15)
#define HEB_F_MOLAD              (1u << 16)
#define HEB_F_USER_EVENT         (1u << 17)
#define HEB_F_HEBREW_DATE        (1u << 18)
#define HEB_F_MINOR_HOLIDAY      (1u << 19)
#define HEB_F_EREV               (1u << 20)
#define HEB_F_CHOL_HAMOED        (1u << 21)
#define HEB_F_MISHNA_YOMI        (1u << 22)
#define HEB_F_YOM_KIPPUR_KATAN   (1u << 23)
#define HEB_F_ZMANIM             (1u << 24)
#define HEB_F_YERUSHALMI_YOMI    (1u << 25)
#define HEB_F_NACH_YOMI          (1u << 26)
#define HEB_F_DAILY_LEARNING     (1u << 27)

/* A calendar event (holiday, parsha, omer, etc.). */
typedef struct {
    heb_hdate_t date;             /* Date of occurrence */
    const char *desc;             /* Description (e.g. "Pesach III (CH''M)") */
    heb_holiday_flags_t flags;    /* Flag bitmask */
    const char *emoji;            /* Holiday emoji (may be NULL) */
    int chol_hamoed_day;          /* CH''M day number (0 = not chol hamoed) */
    int chanukah_day;             /* Chanukah day number (0 = not chanukah) */
} heb_event_t;

/* Maximum number of events that can be stored in a result array. */
#define HEB_MAX_EVENTS 100

/* Renders an event description in the given locale. */
void heb_event_render(const heb_event_t *ev, const char *locale,
                      char *buf, size_t n);

/* Returns the basename (simplified untranslated description). */
void heb_event_basename(const heb_event_t *ev, char *buf, size_t n);

/* Returns the primary category string for the event. */
const char *heb_event_category(const heb_event_t *ev);

/* Maps a flag bitmask to its primary category. */
const char *heb_flags_to_category(heb_holiday_flags_t mask);

#endif /* HEB_EVENT_H */
