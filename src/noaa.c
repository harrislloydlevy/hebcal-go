/*
 * hebcal-c - NOAA solar position calculator.
 * Ported from github.com/hebcal/noaa-go (Go).
 *
 * License: LGPL-2.1-or-later
 */
#include "noaa.h"
#include "greg.h"
#include "arduino_compat.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef NAN
#define NAN (0.0/0.0)
#endif

/* === Solar radius table (365 entries, by day of year) === */
static const double solar_radius_table[365] PROGMEM = {
    0.27108024, 0.27108486, 0.27108790, 0.27108930, 0.27108899,
    0.27108695, 0.27108316, 0.27107762, 0.27107033, 0.27106133,
    0.27105062, 0.27103826, 0.27102427, 0.27100873, 0.27099168,
    0.27097320, 0.27095337, 0.27093228, 0.27091002, 0.27088667,
    0.27086231, 0.27083701, 0.27081079, 0.27078369, 0.27075569,
    0.27072676, 0.27069684, 0.27066588, 0.27063378, 0.27060048,
    0.27056589, 0.27052995, 0.27049261, 0.27045383, 0.27041359,
    0.27037186, 0.27032864, 0.27028396, 0.27023782, 0.27019025,
    0.27014129, 0.27009098, 0.27003938, 0.26998658, 0.26993264,
    0.26987767, 0.26982177, 0.26976506, 0.26970763, 0.26964958,
    0.26959099, 0.26953191, 0.26947239, 0.26941242, 0.26935200,
    0.26929108, 0.26922962, 0.26916755, 0.26910482, 0.26904136,
    0.26897712, 0.26891206, 0.26884614, 0.26877935, 0.26871165,
    0.26864306, 0.26857358, 0.26850321, 0.26843197, 0.26835989,
    0.26828703, 0.26821343, 0.26813918, 0.26806437, 0.26798910,
    0.26791348, 0.26783763, 0.26776167, 0.26768569, 0.26760979,
    0.26753404, 0.26745846, 0.26738308, 0.26730790, 0.26723289,
    0.26715800, 0.26708320, 0.26700842, 0.26693363, 0.26685877,
    0.26678380, 0.26670870, 0.26663342, 0.26655796, 0.26648229,
    0.26640640, 0.26633030, 0.26625399, 0.26617748, 0.26610082,
    0.26602406, 0.26594728, 0.26587055, 0.26579398, 0.26571769,
    0.26564180, 0.26556641, 0.26549164, 0.26541756, 0.26534425,
    0.26527174, 0.26520006, 0.26512920, 0.26505915, 0.26498987,
    0.26492132, 0.26485345, 0.26478622, 0.26471959, 0.26465351,
    0.26458794, 0.26452285, 0.26445820, 0.26439396, 0.26433010,
    0.26426661, 0.26420348, 0.26414070, 0.26407832, 0.26401636,
    0.26395489, 0.26389400, 0.26383378, 0.26377435, 0.26371580,
    0.26365825, 0.26360179, 0.26354651, 0.26349247, 0.26343971,
    0.26338825, 0.26333807, 0.26328918, 0.26324153, 0.26319510,
    0.26314983, 0.26310568, 0.26306261, 0.26302057, 0.26297951,
    0.26293938, 0.26290014, 0.26286173, 0.26282411, 0.26278725,
    0.26275111, 0.26271570, 0.26268102, 0.26264710, 0.26261399,
    0.26258177, 0.26255053, 0.26252037, 0.26249137, 0.26246366,
    0.26243731, 0.26241239, 0.26238897, 0.26236707, 0.26234671,
    0.26232790, 0.26231061, 0.26229481, 0.26228048, 0.26226756,
    0.26225602, 0.26224581, 0.26223687, 0.26222914, 0.26222257,
    0.26221708, 0.26221262, 0.26220912, 0.26220653, 0.26220480,
    0.26220392, 0.26220388, 0.26220470, 0.26220642, 0.26220910,
    0.26221282, 0.26221768, 0.26222375, 0.26223114, 0.26223991,
    0.26225014, 0.26226187, 0.26227512, 0.26228992, 0.26230626,
    0.26232413, 0.26234349, 0.26236433, 0.26238659, 0.26241024,
    0.26243521, 0.26246145, 0.26248890, 0.26251746, 0.26254708,
    0.26257766, 0.26260913, 0.26264141, 0.26267446, 0.26270823,
    0.26274270, 0.26277789, 0.26281382, 0.26285054, 0.26288813,
    0.26292666, 0.26296622, 0.26300686, 0.26304868, 0.26309171,
    0.26313601, 0.26318159, 0.26322848, 0.26327666, 0.26332612,
    0.26337685, 0.26342881, 0.26348197, 0.26353627, 0.26359167,
    0.26364809, 0.26370545, 0.26376368, 0.26382267, 0.26388233,
    0.26394256, 0.26400328, 0.26406442, 0.26412593, 0.26418777,
    0.26424995, 0.26431249, 0.26437542, 0.26443880, 0.26450270,
    0.26456718, 0.26463231, 0.26469815, 0.26476474, 0.26483213,
    0.26490034, 0.26496937, 0.26503922, 0.26510990, 0.26518138,
    0.26525364, 0.26532664, 0.26540034, 0.26547467, 0.26554957,
    0.26562495, 0.26570072, 0.26577676, 0.26585296, 0.26592922,
    0.26600544, 0.26608154, 0.26615745, 0.26623314, 0.26630859,
    0.26638382, 0.26645884, 0.26653372, 0.26660850, 0.26668323,
    0.26675798, 0.26683280, 0.26690773, 0.26698280, 0.26705803,
    0.26713345, 0.26720905, 0.26728485, 0.26736083, 0.26743698,
    0.26751326, 0.26758964, 0.26766606, 0.26774245, 0.26781872,
    0.26789476, 0.26797045, 0.26804569, 0.26812035, 0.26819433,
    0.26826755, 0.26833992, 0.26841141, 0.26848200, 0.26855170,
    0.26862051, 0.26868849, 0.26875567, 0.26882211, 0.26888786,
    0.26895296, 0.26901746, 0.26908139, 0.26914478, 0.26920767,
    0.26927006, 0.26933199, 0.26939345, 0.26945445, 0.26951496,
    0.26957497, 0.26963442, 0.26969325, 0.26975136, 0.26980865,
    0.26986502, 0.26992034, 0.26997450, 0.27002740, 0.27007895,
    0.27012907, 0.27017773, 0.27022491, 0.27027060, 0.27031483,
    0.27035764, 0.27039906, 0.27043914, 0.27047794, 0.27051549,
    0.27055186, 0.27058709, 0.27062122, 0.27065430, 0.27068636,
    0.27071746, 0.27074761, 0.27077684, 0.27080516, 0.27083256,
    0.27085899, 0.27088442, 0.27090875, 0.27093191, 0.27095379,
    0.27097427, 0.27099326, 0.27101067, 0.27102640, 0.27104041,
    0.27105266, 0.27106312, 0.27107182, 0.27107876, 0.27108399,
};

/* === Trigonometric helpers (degrees) === */
static double to_rad(double deg) { return deg * M_PI / 180.0; }
static double to_deg(double rad) { return rad * 180.0 / M_PI; }
static double sin_d(double a) { return sin(to_rad(a)); }
static double cos_d(double a) { return cos(to_rad(a)); }
static double tan_d(double a) { return tan(to_rad(a)); }
static double acos_d(double x) { return to_deg(acos(x)); }
static double asin_d(double x) { return to_deg(asin(x)); }

static double mod_pos(double x, double y) {
    double m = fmod(x, y);
    if (m < 0) m += y;
    return m;
}

/* === Julian day calculations === */
double heb_julian_day(int year, int month, int day) {
    if (month <= 2) {
        year -= 1;
        month += 12;
    }
    int a = year / 100;
    int b = 2 - a + a / 4;
    return floor(365.25 * (double)(year + 4716)) +
           floor(30.6001 * (double)(month + 1)) +
           (double)day + (double)b - 1524.5;
}

double heb_julian_centuries(double jd) {
    return (jd - HEB_JD_J2000) / HEB_JD_PER_CENTURY;
}

/* === Solar position functions === */
static double sun_geom_mean_long(double jc) {
    double lon = 280.46646 + jc * (36000.76983 + 0.0003032 * jc);
    return mod_pos(mod_pos(lon, 360), 360);
}

static double sun_geom_mean_anomaly(double jc) {
    return 357.52911 + jc * (35999.05029 - 0.0001537 * jc);
}

static double earth_orbit_eccentricity(double jc) {
    return 0.016708634 - jc * (0.000042037 + 0.0000001267 * jc);
}

static double sun_equation_of_center(double jc) {
    double m = sun_geom_mean_anomaly(jc);
    double sinm = sin_d(m);
    double sin2m = sin_d(m + m);
    double sin3m = sin_d(m + m + m);
    return sinm * (1.914602 - jc * (0.004817 + 0.000014 * jc)) +
           sin2m * (0.019993 - 0.000101 * jc) + sin3m * 0.000289;
}

static double sun_true_longitude(double jc) {
    return sun_geom_mean_long(jc) + sun_equation_of_center(jc);
}

static double sun_apparent_longitude(double jc) {
    double trueLon = sun_true_longitude(jc);
    double omega = 125.04 - 1934.136 * jc;
    return trueLon - 0.00569 - 0.00478 * sin_d(omega);
}

static double mean_obliquity_of_ecliptic(double jc) {
    double seconds = 21.448 - jc * (46.8150 + jc * (0.00059 - jc * 0.001813));
    return 23.0 + (26.0 + (seconds / 60.0)) / 60.0;
}

static double obliquity_correction(double jc) {
    double obl = mean_obliquity_of_ecliptic(jc);
    double omega = 125.04 - 1934.136 * jc;
    return obl + 0.00256 * cos_d(omega);
}

static double sun_declination(double jc) {
    double obl = obliquity_correction(jc);
    double lambda = sun_apparent_longitude(jc);
    double sint = sin_d(obl) * sin_d(lambda);
    return asin_d(sint);
}

static double equation_of_time(double jc) {
    double epsilon = obliquity_correction(jc);
    double geomMeanLong = sun_geom_mean_long(jc);
    double ecc = earth_orbit_eccentricity(jc);
    double geomMeanAnom = sun_geom_mean_anomaly(jc);
    double y = tan_d(epsilon / 2.0);
    y *= y;
    double sin2l0 = sin_d(2.0 * geomMeanLong);
    double sinm = sin_d(geomMeanAnom);
    double cos2l0 = cos_d(2.0 * geomMeanLong);
    double sin4l0 = sin_d(4.0 * geomMeanLong);
    double sin2m = sin_d(2.0 * geomMeanAnom);
    double eot = y * sin2l0 - 2.0 * ecc * sinm +
                 4.0 * ecc * y * sinm * cos2l0 -
                 0.5 * y * y * sin4l0 -
                 1.25 * ecc * ecc * sin2m;
    return to_deg(eot) * 4.0;
}

/* === Sun hour angle === */
typedef enum { EVENT_SUNRISE, EVENT_SUNSET, EVENT_NOON, EVENT_MIDNIGHT } solar_event_t;

static double sun_hour_angle(double latitude, double declination,
                             double zenith, solar_event_t event) {
    double ratio = cos_d(zenith) / (cos_d(latitude) * cos_d(declination)) -
                   tan_d(latitude) * tan_d(declination);
    if (ratio < -1.0 || ratio > 1.0) return NAN;
    double ha = acos(ratio);
    if (event == EVENT_SUNSET) ha = -ha;
    return ha;
}

/* === Solar noon / midnight UTC === */
static double solar_noon_midnight_utc(double jd, double longitude,
                                      solar_event_t event) {
    double tnoon = heb_julian_centuries(jd + longitude / 360.0);
    double eot = equation_of_time(tnoon);
    double solNoonUTC = (longitude * 4) - eot;

    for (int i = 0; i < 2; i++) {
        double newt = heb_julian_centuries(jd + solNoonUTC / 1440.0);
        eot = equation_of_time(newt);
        double base = 720.0;
        if (event != EVENT_NOON) base = 1440.0;
        solNoonUTC = base + (longitude * 4) - eot;
    }
    return solNoonUTC;
}

/* === Sunrise / sunset UTC === */
static double sun_rise_set_utc(double jd, double latitude, double longitude,
                               double zenith, solar_event_t event) {
    double noonmin = solar_noon_midnight_utc(jd, longitude, EVENT_NOON);
    double tnoon = heb_julian_centuries(jd + noonmin / 1440.0);
    double eot = equation_of_time(tnoon);
    double decl = sun_declination(tnoon);
    double ha = sun_hour_angle(latitude, decl, zenith, event);
    if (isnan(ha)) return NAN;
    double delta = longitude - to_deg(ha);
    double timeDiff = 4 * delta;
    double timeUTC = 720 + timeDiff - eot;

    /* Second pass */
    double newt = heb_julian_centuries(jd + timeUTC / 1440.0);
    eot = equation_of_time(newt);
    decl = sun_declination(newt);
    ha = sun_hour_angle(latitude, decl, zenith, event);
    if (isnan(ha)) return NAN;
    delta = longitude - to_deg(ha);
    timeDiff = 4 * delta;
    timeUTC = 720 + timeDiff - eot;
    return timeUTC;
}

/* === Calculator === */
heb_noaa_calc_t heb_noaa_new(void) {
    heb_noaa_calc_t c;
    c.refraction = 34.0 / 60.0;
    c.solar_radius = 16.0 / 60.0;
    c.use_apparent_solar_radius = 1;
    c.earth_radius = 6371.0088;
    return c;
}

double heb_noaa_elevation_adjustment(const heb_noaa_calc_t *calc,
                                     double elevation) {
    return to_deg(acos(calc->earth_radius /
                       (calc->earth_radius + (elevation / 1000.0))));
}

/* Compute day of year (1-365) */
static int day_of_year(int year, int month, int day) {
    int doy = 0;
    for (int m = 1; m < month; m++) {
        doy += heb_greg_days_in((heb_month_greg_t)m, year);
    }
    doy += day;
    return doy;
}

double heb_noaa_adjust_zenith(const heb_noaa_calc_t *calc,
                              double zenith, double elevation,
                              int doy) {
    if (zenith != HEB_GEOMETRIC_ZENITH) return zenith;
    double sr;
    if (calc->use_apparent_solar_radius && doy > 0) {
        if (doy > 365) doy = 365;
#if defined(__AVR__)
        sr = pgm_read_float(&solar_radius_table[doy - 1]);
#else
        sr = solar_radius_table[doy - 1];
#endif
    } else {
        sr = calc->solar_radius;
    }
    return zenith + sr + calc->refraction +
           heb_noaa_elevation_adjustment(calc, elevation);
}

/* === Public API === */
static double utc_rise_set(const heb_noaa_calc_t *calc,
                           const heb_geolocation_t *geo,
                           int year, int month, int day,
                           double zenith, int adjust_for_elevation,
                           solar_event_t event) {
    double elevation = adjust_for_elevation ? geo->elevation : 0.0;
    int doy = day_of_year(year, month, day);
    double adjustedZenith = heb_noaa_adjust_zenith(calc, zenith, elevation, doy);
    double jd = heb_julian_day(year, month, day);
    /* longitude is positive west in the NOAA algorithm */
    double lon = -geo->longitude;
    double riseSet = sun_rise_set_utc(jd, geo->latitude, lon, adjustedZenith, event);
    riseSet /= 60.0;
    return mod_pos(mod_pos(riseSet, 24), 24);
}

double heb_noaa_utc_sunrise(const heb_noaa_calc_t *calc,
                            const heb_geolocation_t *geo,
                            int year, int month, int day,
                            double zenith, int adjust_for_elevation) {
    return utc_rise_set(calc, geo, year, month, day, zenith,
                        adjust_for_elevation, EVENT_SUNRISE);
}

double heb_noaa_utc_sunset(const heb_noaa_calc_t *calc,
                           const heb_geolocation_t *geo,
                           int year, int month, int day,
                           double zenith, int adjust_for_elevation) {
    return utc_rise_set(calc, geo, year, month, day, zenith,
                        adjust_for_elevation, EVENT_SUNSET);
}

double heb_noaa_utc_noon(const heb_noaa_calc_t *calc,
                         const heb_geolocation_t *geo,
                         int year, int month, int day) {
    double jd = heb_julian_day(year, month, day);
    double lon = -geo->longitude;
    double noon = solar_noon_midnight_utc(jd, lon, EVENT_NOON);
    noon /= 60.0;
    return mod_pos(mod_pos(noon, 24), 24);
}

double heb_noaa_utc_midnight(const heb_noaa_calc_t *calc,
                             const heb_geolocation_t *geo,
                             int year, int month, int day) {
    double jd = heb_julian_day(year, month, day);
    double lon = -geo->longitude;
    double midnight = solar_noon_midnight_utc(jd, lon, EVENT_MIDNIGHT);
    midnight /= 60.0;
    return mod_pos(mod_pos(midnight, 24), 24);
}

/* === Solar elevation and azimuth === */
static double adjust_elevation_for_refraction(double elevation) {
    if (elevation > 85.0) return 0.0;
    double te = tan_d(elevation);
    double correction;
    if (elevation > 5.0) {
        correction = 58.1 / te - 0.07 / pow(te, 3) + 0.000086 / pow(te, 5);
    } else if (elevation > -0.575) {
        correction = 1735.0 + elevation * (-518.2 + elevation * (103.4 + elevation * (-12.79 + 0.711 * elevation)));
    } else {
        correction = -20.774 / te;
    }
    return correction / 3600.0;
}

static double solar_elevation_azimuth(const heb_geolocation_t *geo,
                                      int year, int month, int day,
                                      double utc_hours, int is_azimuth) {
    double lat = geo->latitude;
    double lon = geo->longitude;

    double fractionalDay = utc_hours / 24.0;
    double jd = heb_julian_day(year, month, day) + fractionalDay;
    double jc = heb_julian_centuries(jd);
    double decl = sun_declination(jc);
    double eot = equation_of_time(jc);

    double trueSolarTime = mod_pos((fractionalDay + eot / 1440.0 + lon / 360.0) + 2, 1);
    double hourAngle = trueSolarTime * 2 * M_PI - M_PI;
    double cosZenith = sin_d(lat) * sin_d(decl) + cos_d(lat) * cos_d(decl) * cos(hourAngle);
    if (cosZenith > 1.0) cosZenith = 1.0;
    if (cosZenith < -1.0) cosZenith = -1.0;
    double zenithDeg = acos_d(cosZenith);
    double elevation = (90.0 - zenithDeg) + adjust_elevation_for_refraction(90.0 - zenithDeg);

    if (is_azimuth) {
        double azimuth;
        double azDenom = cos_d(lat) * sin_d(zenithDeg);
        if (fabs(azDenom) > 0.001) {
            double az = (sin_d(lat) * cos_d(zenithDeg) - sin_d(decl)) / azDenom;
            if (az > 1.0) az = 1.0;
            if (az < -1.0) az = -1.0;
            double sign = (hourAngle > 0) ? -1.0 : 1.0;
            azimuth = 180 - acos_d(az) * sign;
        } else if (lat > 0) {
            azimuth = 180;
        } else {
            azimuth = 0;
        }
        return mod_pos(azimuth + 360, 360);
    }
    return elevation;
}

double heb_noaa_solar_elevation(const heb_geolocation_t *geo,
                                int year, int month, int day,
                                double utc_hours) {
    return solar_elevation_azimuth(geo, year, month, day, utc_hours, 0);
}

double heb_noaa_solar_azimuth(const heb_geolocation_t *geo,
                              int year, int month, int day,
                              double utc_hours) {
    return solar_elevation_azimuth(geo, year, month, day, utc_hours, 1);
}
