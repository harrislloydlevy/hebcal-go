import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor, sensor, time
from esphome.const import (
    CONF_ID,
    CONF_TIME_ID,
    CONF_LATITUDE,
    CONF_LONGITUDE,
    CONF_ELEVATION,
    CONF_UPDATE_INTERVAL,
)
from esphome.cpp_generator import RawExpression

CODEOWNERS = ["@harrislloydlevy"]
DEPENDENCIES = ["time"]

CONF_ISRAEL = "israel"
CONF_LOCALE = "locale"
CONF_CANDLE_LIGHTING = "candle_lighting"
CONF_CANDLE_LIGHTING_MINS = "candle_lighting_mins"
CONF_HAVDALAH_MINS = "havdalah_mins"
CONF_HAVDALAH_DEG = "havdalah_deg"
CONF_USE_ELEVATION = "use_elevation"
CONF_OMER = "omer"
CONF_SEDROT = "sedrot"
CONF_MOLAD = "molad"
CONF_NO_ROSH_CHODESH = "no_rosh_chodesh"
CONF_NO_MODERN = "no_modern"
CONF_NO_MINOR_FAST = "no_minor_fast"
CONF_NO_SPECIAL_SHABBAT = "no_special_shabbat"

# Text sensors exposed by the component
CONF_HEBREW_DATE = "hebrew_date"
CONF_PARASHA = "parasha"
CONF_HOLIDAY = "holiday"
CONF_CANDLES = "candles"
CONF_HAVDALAH = "havdalah"
CONF_OMER_TEXT = "omer_text"
CONF_DOW = "day_of_week"

# Numeric sensors
CONF_SUNRISE = "sunrise"
CONF_SUNSET = "sunset"
CONF_CHATZOT = "chatzot"

hebcal_ns = cg.esphome_ns.namespace("hebcal")
HebcalComponent = hebcal_ns.class_("HebcalComponent", cg.PollingComponent)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(HebcalComponent),
        cv.Required(CONF_TIME_ID): cv.use_id(time.RealTime),
        cv.Optional(CONF_LATITUDE): cv.float_,
        cv.Optional(CONF_LONGITUDE): cv.float_,
        cv.Optional(CONF_ELEVATION): cv.float_,
        cv.Optional(CONF_ISRAEL, default=False): cv.boolean,
        cv.Optional(CONF_LOCALE, default="en"): cv.string,
        cv.Optional(CONF_CANDLE_LIGHTING, default=False): cv.boolean,
        cv.Optional(CONF_CANDLE_LIGHTING_MINS, default=18): cv.int_,
        cv.Optional(CONF_HAVDALAH_MINS, default=0): cv.int_,
        cv.Optional(CONF_HAVDALAH_DEG, default=8.5): cv.float_,
        cv.Optional(CONF_USE_ELEVATION, default=False): cv.boolean,
        cv.Optional(CONF_OMER, default=False): cv.boolean,
        cv.Optional(CONF_SEDROT, default=False): cv.boolean,
        cv.Optional(CONF_MOLAD, default=False): cv.boolean,
        cv.Optional(CONF_NO_ROSH_CHODESH, default=False): cv.boolean,
        cv.Optional(CONF_NO_MODERN, default=False): cv.boolean,
        cv.Optional(CONF_NO_MINOR_FAST, default=False): cv.boolean,
        cv.Optional(CONF_NO_SPECIAL_SHABBAT, default=False): cv.boolean,
        cv.Optional(CONF_UPDATE_INTERVAL, default="1h"): cv.update_interval,

        # Text sensors
        cv.Optional(CONF_HEBREW_DATE): text_sensor.text_sensor_schema(
            HebcalComponent, icon="mdi:calendar-star"
        ),
        cv.Optional(CONF_PARASHA): text_sensor.text_sensor_schema(
            HebcalComponent, icon="mdi:book-open-variant"
        ),
        cv.Optional(CONF_HOLIDAY): text_sensor.text_sensor_schema(
            HebcalComponent, icon="mdi:calendar-check"
        ),
        cv.Optional(CONF_CANDLES): text_sensor.text_sensor_schema(
            HebcalComponent, icon="mdi:candle"
        ),
        cv.Optional(CONF_HAVDALAH): text_sensor.text_sensor_schema(
            HebcalComponent, icon="mdi:star-three-points"
        ),
        cv.Optional(CONF_OMER_TEXT): text_sensor.text_sensor_schema(
            HebcalComponent, icon="mdi:counter"
        ),
        cv.Optional(CONF_DOW): text_sensor.text_sensor_schema(
            HebcalComponent, icon="mdi:calendar-today"
        ),

        # Numeric sensors (epoch seconds for the event time)
        cv.Optional(CONF_SUNRISE): sensor.sensor_schema(
            HebcalComponent, unit_of_measurement="s", accuracy_decimals=0, icon="mdi:weather-sunny"
        ),
        cv.Optional(CONF_SUNSET): sensor.sensor_schema(
            HebcalComponent, unit_of_measurement="s", accuracy_decimals=0, icon="mdi:weather-night"
        ),
        cv.Optional(CONF_CHATZOT): sensor.sensor_schema(
            HebcalComponent, unit_of_measurement="s", accuracy_decimals=0, icon="mdi:clock-outline"
        ),
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_polling_component(var, config)

    time_var = await cg.get_variable(config[CONF_TIME_ID])
    cg.add(var.set_time(time_var))

    if CONF_LATITUDE in config:
        cg.add(var.set_latitude(config[CONF_LATITUDE]))
    if CONF_LONGITUDE in config:
        cg.add(var.set_longitude(config[CONF_LONGITUDE]))
    if CONF_ELEVATION in config:
        cg.add(var.set_elevation(config[CONF_ELEVATION]))

    cg.add(var.set_israel(config[CONF_ISRAEL]))
    cg.add(var.set_locale(config[CONF_LOCALE]))
    cg.add(var.set_candle_lighting(config[CONF_CANDLE_LIGHTING]))
    cg.add(var.set_candle_lighting_mins(config[CONF_CANDLE_LIGHTING_MINS]))
    cg.add(var.set_havdalah_mins(config[CONF_HAVDALAH_MINS]))
    cg.add(var.set_havdalah_deg(config[CONF_HAVDALAH_DEG]))
    cg.add(var.set_use_elevation(config[CONF_USE_ELEVATION]))
    cg.add(var.set_omer(config[CONF_OMER]))
    cg.add(var.set_sedrot(config[CONF_SEDROT]))
    cg.add(var.set_molad(config[CONF_MOLAD]))
    cg.add(var.set_no_rosh_chodesh(config[CONF_NO_ROSH_CHODESH]))
    cg.add(var.set_no_modern(config[CONF_NO_MODERN]))
    cg.add(var.set_no_minor_fast(config[CONF_NO_MINOR_FAST]))
    cg.add(var.set_no_special_shabbat(config[CONF_NO_SPECIAL_SHABBAT]))

    # Wire up text sensors
    for conf_key, setter in [
        (CONF_HEBREW_DATE, "set_hebrew_date"),
        (CONF_PARASHA, "set_parasha"),
        (CONF_HOLIDAY, "set_holiday"),
        (CONF_CANDLES, "set_candles"),
        (CONF_HAVDALAH, "set_havdalah"),
        (CONF_OMER_TEXT, "set_omer_text"),
        (CONF_DOW, "set_dow"),
    ]:
        if conf_key in config:
            sens = await text_sensor.new_text_sensor(config[conf_key])
            cg.add(getattr(var, setter)(sens))

    # Wire up numeric sensors
    for conf_key, setter in [
        (CONF_SUNRISE, "set_sunrise_sensor"),
        (CONF_SUNSET, "set_sunset_sensor"),
        (CONF_CHATZOT, "set_chatzot_sensor"),
    ]:
        if conf_key in config:
            sens = await sensor.new_sensor(config[conf_key])
            cg.add(getattr(var, setter)(sens))
