/*
 * HolidaysOfYear - List Jewish holidays for a Gregorian year.
 *
 * Prints all holidays for the given year to the Serial Monitor.
 * Works on AVR and ESP32.
 *
 * Open the Serial Monitor at 9600 baud.
 */

#include <Arduino.h>
#include "hebcal.h"
#include "hdate.h"
#include "holidays.h"

void setup() {
    Serial.begin(9600);
    while (!Serial) { ; }
    delay(1000);

    int year = 2026;  // Change to any year

    Serial.print("=== Jewish Holidays for ");
    Serial.print(year);
    Serial.println(" ===\n");

    heb_cal_options_t opts = heb_cal_default_options();
    opts.year = year;

    // Static buffer for events (AVR has limited RAM; ESP32 has plenty)
    static heb_event_t events[150];
    int n = heb_hebrew_calendar(&opts, events, 150);

    for (int i = 0; i < n; i++) {
        heb_gdate_t gd = heb_hdate_greg(&events[i].date);
        Serial.print(gd.year);
        Serial.print("-");
        if (gd.month < 10) Serial.print("0");
        Serial.print(gd.month);
        Serial.print("-");
        if (gd.day < 10) Serial.print("0");
        Serial.print(gd.day);
        Serial.print("  ");
        Serial.println(events[i].desc);
    }

    Serial.print("\nTotal: ");
    Serial.print(n);
    Serial.println(" events");
}

void loop() {
    // nothing to do
}
