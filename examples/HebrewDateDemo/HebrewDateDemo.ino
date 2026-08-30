/*
 * HebrewDateDemo - Basic Hebrew date conversion for Arduino.
 *
 * Converts between Hebrew and Gregorian dates and prints
 * holiday information for a given date.
 *
 * Compatible with AVR (Uno, Nano) and ESP32.
 *
 * Open the Serial Monitor at 9600 baud.
 */

#include <Arduino.h>
#include "hdate.h"
#include "holidays.h"

void setup() {
    Serial.begin(9600);
    while (!Serial) { ; }
    delay(1000);

    Serial.println("=== Hebcal-c Hebrew Date Demo ===\n");

    // Convert Gregorian to Hebrew: November 13, 2008
    heb_hdate_t hd = heb_hdate_from_gregorian(2008, HEB_NOVEMBER, 13);
    char buf[64];
    heb_hdate_to_string(&hd, buf, sizeof(buf));
    Serial.print("2008-11-13 = ");
    Serial.println(buf);

    // Show the weekday
    const char *days[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    Serial.print("  Weekday: ");
    Serial.println(days[heb_hdate_weekday(&hd)]);

    // Convert Hebrew to Gregorian: 15 Cheshvan 5769
    heb_hdate_create(&hd, 5769, HEB_H_CHESHVAN, 15);
    heb_gdate_t gd = heb_hdate_greg(&hd);
    Serial.print("15 Cheshvan 5769 = ");
    Serial.print(gd.year);
    Serial.print("-");
    Serial.print(gd.month);
    Serial.print("-");
    Serial.println(gd.day);

    // Check if today is a holiday
    // (using a fixed date for demo: April 22, 2024 = first day of Pesach)
    hd = heb_hdate_from_gregorian(2024, HEB_APRIL, 22);
    heb_event_t events[10];
    int n = heb_holidays_for_date(&hd, 0, events, 10);
    Serial.print("\nHolidays on 2024-04-22: ");
    if (n > 0) {
        Serial.println(events[0].desc);
    } else {
        Serial.println("(none)");
    }

    // Show some year info
    Serial.println("\nHebrew year 5784 info:");
    Serial.print("  Leap year: ");
    Serial.println(heb_hdate_is_leap_year(5784) ? "yes" : "no");
    Serial.print("  Days in year: ");
    Serial.println(heb_hdate_days_in_year(5784));
    Serial.print("  Months in year: ");
    Serial.println(heb_hdate_months_in_year(5784));

    Serial.println("\nDone!");
}

void loop() {
    // nothing to do
}
