/*
 * ZmanimESP32 - Halachic times (zmanim) for a given location on ESP32.
 *
 * Uses SNTP to get the current date, then calculates and prints
 * halachic times (sunrise, sunset, chatzot, alot hashachar, tzeit).
 *
 * ESP32 only (requires WiFi and time.h).
 *
 * Open the Serial Monitor at 115200 baud.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include "zmanim.h"
#include "hdate.h"

// --- Configuration: set these to your values ---
const char *WIFI_SSID = "your-ssid";
const char *WIFI_PASS = "your-password";

// Location: change to your city
const double LATITUDE  = 40.09789;   // Lakewood, NJ
const double LONGITUDE = -74.21764;
const double ELEVATION = 0.0;
const int UTC_OFFSET  = -300;         // minutes from UTC (-5 = EST, -240 = EDT)

void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }

    // Connect to WiFi
    Serial.print("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" connected!");

    // Get current date via NTP
    configTime(UTC_OFFSET * 60, 0, "pool.ntp.org", "time.nist.gov");
    Serial.print("Getting time...");
    struct tm timeinfo;
    while (!getLocalTime(&timeinfo)) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println(" got it!");

    int year = timeinfo.tm_year + 1900;
    int month = timeinfo.tm_mon + 1;
    int day = timeinfo.tm_mday;

    Serial.print("\n=== Zmanim for ");
    Serial.print(year);
    Serial.print("-");
    Serial.print(month);
    Serial.print("-");
    Serial.print(day);
    Serial.println(" ===\n");

    // Calculate zmanim
    heb_zmanim_t z = heb_zmanim_new(LATITUDE, LONGITUDE, ELEVATION,
                                     year, month, day, UTC_OFFSET);

    double sunrise = heb_zmanim_sunrise(&z);
    double sunset  = heb_zmanim_sunset(&z);
    double chatzot = heb_zmanim_chatzot(&z);
    double alot    = heb_zmanim_alot_hashachar(&z);
    double tzeit   = heb_zmanim_tzeit(&z, 8.5);

    // Print times
    char buf[16];

    heb_zmanim_format_time(alot, UTC_OFFSET, buf, sizeof(buf));
    Serial.print("Alot HaShachar:  "); Serial.println(buf);

    heb_zmanim_format_time(sunrise, UTC_OFFSET, buf, sizeof(buf));
    Serial.print("Sunrise:         "); Serial.println(buf);

    heb_zmanim_format_time(chatzot, UTC_OFFSET, buf, sizeof(buf));
    Serial.print("Chatzot:         "); Serial.println(buf);

    heb_zmanim_format_time(sunset, UTC_OFFSET, buf, sizeof(buf));
    Serial.print("Sunset:          "); Serial.println(buf);

    heb_zmanim_format_time(tzeit, UTC_OFFSET, buf, sizeof(buf));
    Serial.print("Tzeit 8.5 deg:   "); Serial.println(buf);

    // Also show the Hebrew date
    heb_hdate_t hd = heb_hdate_from_gregorian(year, (heb_month_greg_t)month, day);
    char hdBuf[64];
    heb_hdate_to_string(&hd, hdBuf, sizeof(hdBuf));
    Serial.print("\nHebrew date: "); Serial.println(hdBuf);

    Serial.println("\nDone!");
    WiFi.disconnect(true);
}

void loop() {
    // nothing to do
}
