/*
 * hebcal-c - GeoName ID lookup table.
 *
 * License: GPL-2.0-or-later
 */
#include "geonames.h"
#include <string.h>

/* Built-in table of common Jewish community cities (93 entries).
 * Sorted by geoname_id for binary search.
 * Coordinates from geonames.org database. */
static const heb_geoname_t geoname_table[] = {
    { 232351, "Petah Tikva", 32.09, 34.89, 50, "Asia/Jerusalem" },
    { 232448, "Netanya", 32.33, 34.57, 28, "Asia/Jerusalem" },
    { 232802, "Rishon LeZiyyon", 31.97, 34.77, 60, "Asia/Jerusalem" },
    { 233619, "Rehovot", 31.89, 34.81, 50, "Asia/Jerusalem" },
    { 234672, "Tzfat", 32.96, 35.5, 850, "Asia/Jerusalem" },
    { 235729, "Tveria", 32.79, 35.53, -200, "Asia/Jerusalem" },
    { 280086, "Haifa", 32.82, 34.99, 50, "Asia/Jerusalem" },
    { 281184, "Jerusalem", 31.78, 35.22, 757, "Asia/Jerusalem" },
    { 290557, "Dubai", 25.07, 55.3, 3, "Asia/Dubai" },
    { 292223, "Beer Sheva", 31.25, 34.79, 285, "Asia/Jerusalem" },
    { 292553, "Hadera", 32.43, 34.95, 50, "Asia/Jerusalem" },
    { 293068, "Bnei Brak", 32.08, 34.84, 40, "Asia/Jerusalem" },
    { 293397, "Tel Aviv", 32.08, 34.78, 15, "Asia/Jerusalem" },
    { 294751, "Ramat Gan", 32.08, 34.8, 70, "Asia/Jerusalem" },
    { 294801, "Modiin", 31.9, 35.01, 180, "Asia/Jerusalem" },
    { 524901, "Moscow", 55.75, 37.62, 144, "Europe/Moscow" },
    { 658225, "Helsinki", 60.17, 24.94, 26, "Europe/Helsinki" },
    { 683506, "Bucharest", 44.43, 26.1, 77, "Europe/Bucharest" },
    { 727011, "Ljubljana", 46.06, 14.51, 295, "Europe/Ljubljana" },
    { 756135, "Warsaw", 52.23, 21.01, 113, "Europe/Warsaw" },
    { 1151254, "Phuket", 7.88, 98.39, 12, "Asia/Bangkok" },
    { 1185241, "Dhaka", 23.73, 90.4, 8, "Asia/Dhaka" },
    { 1261481, "Delhi", 28.67, 77.21, 239, "Asia/Kolkata" },
    { 1274733, "Bangalore", 12.97, 77.56, 920, "Asia/Kolkata" },
    { 1275339, "Mumbai", 19.07, 72.87, 14, "Asia/Kolkata" },
    { 1609350, "Bangkok", 13.75, 100.5, 4, "Asia/Bangkok" },
    { 1699395, "Manila", 14.6, 120.98, 8, "Asia/Manila" },
    { 1796236, "Shanghai", 31.22, 121.46, 7, "Asia/Shanghai" },
    { 1816670, "Beijing", 39.91, 116.4, 49, "Asia/Shanghai" },
    { 1819729, "Hong Kong", 22.28, 114.16, 8, "Asia/Hong_Kong" },
    { 1850147, "Tokyo", 35.69, 139.69, 40, "Asia/Tokyo" },
    { 1880252, "Singapore", 1.29, 103.85, 16, "Asia/Singapore" },
    { 2063523, "Perth", -31.95, 115.86, 25, "Australia/Perth" },
    { 2147714, "Sydney", -33.87, 151.21, 58, "Australia/Sydney" },
    { 2158177, "Melbourne", -37.81, 144.96, 25, "Australia/Melbourne" },
    { 2172517, "Brisbane", -27.47, 153.03, 28, "Australia/Brisbane" },
    { 2173220, "Adelaide", -34.93, 138.6, 59, "Australia/Adelaide" },
    { 2186231, "Auckland", -36.85, 174.76, 49, "Pacific/Auckland" },
    { 2618425, "Copenhagen", 55.68, 12.57, 5, "Europe/Copenhagen" },
    { 2643339, "Manchester", 53.48, -2.24, 38, "Europe/Manchester" },
    { 2643743, "London", 51.51, -0.13, 25, "Europe/London" },
    { 2673730, "Stockholm", 59.33, 18.07, 26, "Europe/Stockholm" },
    { 2759794, "Amsterdam", 52.37, 4.89, 2, "Europe/Amsterdam" },
    { 2761369, "Vienna", 48.21, 16.37, 171, "Europe/Vienna" },
    { 2925533, "Frankfurt", 50.12, 8.68, 112, "Europe/Berlin" },
    { 2950159, "Berlin", 52.52, 13.4, 74, "Europe/Berlin" },
    { 2988507, "Paris", 48.86, 2.35, 35, "Europe/Paris" },
    { 3067696, "Prague", 50.09, 14.42, 377, "Europe/Prague" },
    { 3110179, "Oslo", 59.91, 10.75, 23, "Europe/Oslo" },
    { 3117735, "Madrid", 40.42, -3.7, 667, "Europe/Madrid" },
    { 3128760, "Barcelona", 41.39, 2.16, 26, "Europe/Madrid" },
    { 3169070, "Rome", 41.89, 12.48, 20, "Europe/Rome" },
    { 3433932, "Montevideo", -34.83, -56.19, 32, "America/Montevideo" },
    { 3435910, "Buenos Aires", -34.61, -58.42, 31, "America/Argentina/Buenos_Aires" },
    { 3448439, "Sao Paulo", -23.55, -46.64, 760, "America/Sao_Paulo" },
    { 3674962, "Lima", -12.05, -77.04, 154, "America/Lima" },
    { 3688685, "Bogota", 4.71, -74.07, 2582, "America/Bogota" },
    { 3871336, "Santiago", -33.46, -70.66, 556, "America/Santiago" },
    { 4140963, "Washington DC", 38.89, -77.04, 7, "America/New_York" },
    { 4155966, "Miami", 25.77, -80.19, 10, "America/New_York" },
    { 4180439, "Atlanta", 33.75, -84.39, 336, "America/New_York" },
    { 4509177, "Columbus", 39.96, -82.99, 242, "America/New_York" },
    { 4560349, "Philadelphia", 39.95, -75.17, 12, "America/New_York" },
    { 4671654, "Austin", 30.27, -97.74, 149, "America/Chicago" },
    { 4736286, "Houston", 29.76, -95.37, 12, "America/Chicago" },
    { 4744709, "Dallas", 32.78, -96.8, 139, "America/Chicago" },
    { 4806824, "Denver", 39.74, -104.99, 1636, "America/Denver" },
    { 4862182, "Minneapolis", 44.98, -93.27, 262, "America/Chicago" },
    { 4887158, "Detroit", 42.33, -83.05, 189, "America/Detroit" },
    { 4887398, "Chicago", 41.85, -87.65, 180, "America/Chicago" },
    { 4926724, "Baltimore", 39.29, -76.61, 10, "America/New_York" },
    { 4930956, "Boston", 42.36, -71.06, 43, "America/New_York" },
    { 4990729, "Cleveland", 41.5, -81.7, 198, "America/New_York" },
    { 4994358, "St. Louis", 38.63, -90.2, 153, "America/Chicago" },
    { 5128581, "New York", 40.71, -74.01, 10, "America/New_York" },
    { 5317158, "Las Vegas", 36.17, -115.14, 614, "America/Los_Angeles" },
    { 5368361, "Los Angeles", 34.05, -118.24, 96, "America/Los_Angeles" },
    { 5391959, "San Francisco", 37.77, -122.42, 16, "America/Los_Angeles" },
    { 5392171, "San Diego", 32.72, -117.16, 20, "America/Los_Angeles" },
    { 5446220, "Phoenix", 33.45, -112.07, 331, "America/Phoenix" },
    { 5506956, "Salt Lake City", 40.75, -111.89, 1300, "America/Denver" },
    { 5516233, "Las Vegas NV", 36.17, -115.14, 614, "America/Los_Angeles" },
    { 5641729, "Portland", 45.52, -122.68, 15, "America/Los_Angeles" },
    { 5809844, "Seattle", 47.61, -122.33, 63, "America/Los_Angeles" },
    { 5913490, "Calgary", 51.05, -114.09, 1050, "America/Edmonton" },
    { 5946768, "Edmonton", 53.55, -113.47, 664, "America/Edmonton" },
    { 6077243, "Montreal", 45.5, -73.57, 57, "America/Toronto" },
    { 6119109, "Ottawa", 45.42, -75.7, 79, "America/Toronto" },
    { 6141242, "Winnipeg", 49.88, -97.17, 229, "America/Winnipeg" },
    { 6167865, "Toronto", 43.7, -79.42, 167, "America/Toronto" },
    { 6173331, "Vancouver", 49.25, -123.12, 70, "America/Vancouver" },
    { 6695072, "Hamburg", 53.55, 9.99, 6, "Europe/Berlin" },
    { 7280737, "Budapest", 47.5, 19.08, 154, "Europe/Budapest" },
};

#define TABLE_SIZE (sizeof(geoname_table) / sizeof(geoname_table[0]))

const heb_geoname_t *heb_geonames_lookup(int geoname_id) {
    int lo = 0, hi = (int)TABLE_SIZE - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (geoname_table[mid].geoname_id == geoname_id)
            return &geoname_table[mid];
        if (geoname_id < geoname_table[mid].geoname_id)
            hi = mid - 1;
        else
            lo = mid + 1;
    }
    return NULL;
}

int heb_geonames_count(void) {
    return (int)TABLE_SIZE;
}

const heb_geoname_t *heb_geonames_table(void) {
    return geoname_table;
}

/* --- Simplified DST offset calculation --- */

/* Check if a timezone string starts with a prefix */
static int starts_with(const char *s, const char *prefix) {
    size_t lp = strlen(prefix);
    return strncmp(s, prefix, lp) == 0;
}

/* Approximate UTC offset in minutes for common IANA timezones.
 * month is 1-12 (used for DST determination).
 * This covers the vast majority of Jewish communities. */
int heb_geonames_tz_offset(const char *tz, int month) {
    if (!tz) return 0;

    /* Israel: UTC+2, DST UTC+3 (~Mar 27 - Oct 30) */
    if (strcmp(tz, "Asia/Jerusalem") == 0)
        return (month >= 4 && month <= 10) ? 180 : 120;

    /* US Eastern: UTC-5, DST UTC-4 (Mar-Nov) */
    if (strcmp(tz, "America/New_York") == 0 || strcmp(tz, "America/Detroit") == 0)
        return (month >= 3 && month <= 11) ? -240 : -300;

    /* US Central: UTC-6, DST UTC-5 */
    if (strcmp(tz, "America/Chicago") == 0)
        return (month >= 3 && month <= 11) ? -300 : -360;

    /* US Mountain: UTC-7, DST UTC-6 */
    if (strcmp(tz, "America/Denver") == 0)
        return (month >= 3 && month <= 11) ? -360 : -420;

    /* US Mountain (no DST): UTC-7 */
    if (strcmp(tz, "America/Phoenix") == 0)
        return -420;

    /* US Pacific: UTC-8, DST UTC-7 */
    if (strcmp(tz, "America/Los_Angeles") == 0)
        return (month >= 3 && month <= 11) ? -420 : -480;

    /* Alaska: UTC-9, DST UTC-8 */
    if (strcmp(tz, "America/Anchorage") == 0)
        return (month >= 3 && month <= 11) ? -480 : -540;

    /* Canada Toronto: UTC-5, DST UTC-4 */
    if (strcmp(tz, "America/Toronto") == 0)
        return (month >= 3 && month <= 11) ? -240 : -300;

    /* Canada Vancouver: UTC-8, DST UTC-7 */
    if (strcmp(tz, "America/Vancouver") == 0)
        return (month >= 3 && month <= 11) ? -420 : -480;

    /* Canada Edmonton/Calgary: UTC-7, DST UTC-6 */
    if (strcmp(tz, "America/Edmonton") == 0)
        return (month >= 3 && month <= 11) ? -360 : -420;

    /* Canada Winnipeg: UTC-6, DST UTC-5 */
    if (strcmp(tz, "America/Winnipeg") == 0)
        return (month >= 3 && month <= 11) ? -300 : -360;

    /* UK: UTC+0, DST UTC+1 (Mar-Oct) */
    if (strcmp(tz, "Europe/London") == 0 || strcmp(tz, "Europe/Manchester") == 0)
        return (month >= 3 && month <= 10) ? 60 : 0;

    /* Western Europe: UTC+1, DST UTC+2 */
    if (strcmp(tz, "Europe/Paris") == 0 || strcmp(tz, "Europe/Amsterdam") == 0 ||
        strcmp(tz, "Europe/Madrid") == 0 || strcmp(tz, "Europe/Rome") == 0)
        return (month >= 3 && month <= 10) ? 120 : 60;

    /* Central Europe: UTC+1, DST UTC+2 */
    if (strcmp(tz, "Europe/Berlin") == 0 || strcmp(tz, "Europe/Vienna") == 0 ||
        strcmp(tz, "Europe/Prague") == 0 || strcmp(tz, "Europe/Warsaw") == 0 ||
        strcmp(tz, "Europe/Budapest") == 0 || strcmp(tz, "Europe/Bucharest") == 0)
        return (month >= 3 && month <= 10) ? 120 : 60;

    /* Northern Europe: UTC+2, no DST (Finland) */
    if (strcmp(tz, "Europe/Helsinki") == 0)
        return 120;

    /* Scandinavia: UTC+1, DST UTC+2 */
    if (strcmp(tz, "Europe/Stockholm") == 0 || strcmp(tz, "Europe/Oslo") == 0 ||
        strcmp(tz, "Europe/Copenhagen") == 0)
        return (month >= 3 && month <= 10) ? 120 : 60;

    /* Moscow: UTC+3, no DST */
    if (strcmp(tz, "Europe/Moscow") == 0)
        return 180;

    /* South America: Buenos Aires UTC-3, no DST */
    if (strcmp(tz, "America/Argentina/Buenos_Aires") == 0)
        return -180;

    /* Brazil: Sao Paulo UTC-3, DST UTC-2 (Oct-Feb) */
    if (strcmp(tz, "America/Sao_Paulo") == 0)
        return (month >= 11 || month <= 2) ? -120 : -180;

    /* Uruguay: UTC-3, no DST */
    if (strcmp(tz, "America/Montevideo") == 0)
        return -180;

    /* Chile: UTC-4, DST UTC-3 (Sep-Apr) */
    if (strcmp(tz, "America/Santiago") == 0)
        return (month >= 9 || month <= 4) ? -180 : -240;

    /* Colombia: UTC-5, no DST */
    if (strcmp(tz, "America/Bogota") == 0)
        return -300;

    /* Peru: UTC-5, no DST */
    if (strcmp(tz, "America/Lima") == 0)
        return -300;

    /* Australia Sydney: UTC+10, DST UTC+11 (Oct-Apr) */
    if (strcmp(tz, "Australia/Sydney") == 0)
        return (month >= 10 || month <= 4) ? 660 : 600;

    /* Australia Melbourne: same as Sydney */
    if (strcmp(tz, "Australia/Melbourne") == 0)
        return (month >= 10 || month <= 4) ? 660 : 600;

    /* Australia Brisbane: UTC+10, no DST */
    if (strcmp(tz, "Australia/Brisbane") == 0)
        return 600;

    /* Australia Adelaide: UTC+9:30, DST UTC+10:30 */
    if (strcmp(tz, "Australia/Adelaide") == 0)
        return (month >= 10 || month <= 4) ? 630 : 570;

    /* Australia Perth: UTC+8, no DST */
    if (strcmp(tz, "Australia/Perth") == 0)
        return 480;

    /* New Zealand: UTC+12, DST UTC+13 (Sep-Apr) */
    if (strcmp(tz, "Pacific/Auckland") == 0)
        return (month >= 9 || month <= 4) ? 780 : 720;

    /* Asia: Dubai UTC+4, no DST */
    if (strcmp(tz, "Asia/Dubai") == 0)
        return 240;

    /* Asia: India UTC+5:30, no DST */
    if (strcmp(tz, "Asia/Kolkata") == 0)
        return 330;

    /* Asia: Bangkok UTC+7, no DST */
    if (strcmp(tz, "Asia/Bangkok") == 0)
        return 420;

    /* Asia: Singapore UTC+8, no DST */
    if (strcmp(tz, "Asia/Singapore") == 0)
        return 480;

    /* Asia: Hong Kong UTC+8, no DST */
    if (strcmp(tz, "Asia/Hong_Kong") == 0)
        return 480;

    /* Asia: Shanghai/Beijing UTC+8, no DST */
    if (strcmp(tz, "Asia/Shanghai") == 0)
        return 480;

    /* Asia: Tokyo UTC+9, no DST */
    if (strcmp(tz, "Asia/Tokyo") == 0)
        return 540;

    /* Asia: Manila UTC+8, no DST */
    if (strcmp(tz, "Asia/Manila") == 0)
        return 480;

    /* Asia: Dhaka UTC+6, no DST */
    if (strcmp(tz, "Asia/Dhaka") == 0)
        return 360;

    /* Fallback: UTC */
    return 0;
}
