import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_MODE,
    CONF_TEMPERATURE,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_ENERGY,
    DEVICE_CLASS_FREQUENCY,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_VOLTAGE,
    ICON_COUNTER,
    ICON_EMPTY,
    ICON_TIMER,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_AMPERE,
    UNIT_CELSIUS,
    UNIT_EMPTY,
    UNIT_HERTZ,
    UNIT_KILOWATT_HOURS,
    UNIT_VOLT,
    UNIT_WATT,
)

from . import CONF_SOLAX_X1_MINI_COMPONENT_SCHEMA, CONF_SOLAX_X1_MINI_ID

DEPENDENCIES = ["solax_x1_mini"]
CODEOWNERS = ["@syssi"]

CONF_ENERGY_TODAY = "energy_today"
CONF_ENERGY_TOTAL = "energy_total"
CONF_DC1_CURRENT = "dc1_current"
CONF_DC1_VOLTAGE = "dc1_voltage"
CONF_DC2_CURRENT = "dc2_current"
CONF_DC2_VOLTAGE = "dc2_voltage"
CONF_AC_CURRENT = "ac_current"
CONF_AC_VOLTAGE = "ac_voltage"
CONF_AC_FREQUENCY = "ac_frequency"
CONF_AC_POWER = "ac_power"
CONF_RUNTIME_TOTAL = "runtime_total"
CONF_ERROR_BITS = "error_bits"
CONF_GRID_VOLTAGE_FAULT = "grid_voltage_fault"
CONF_GRID_FREQUENCY_FAULT = "grid_frequency_fault"
CONF_DC_INJECTION_FAULT = "dc_injection_fault"
CONF_TEMPERATURE_FAULT = "temperature_fault"
CONF_PV1_VOLTAGE_FAULT = "pv1_voltage_fault"
CONF_PV2_VOLTAGE_FAULT = "pv2_voltage_fault"
CONF_GFC_FAULT = "gfc_fault"

UNIT_HOURS = "h"

ICON_MODE = "mdi:heart-pulse"
ICON_ERROR_BITS = "mdi:alert-circle-outline"

# key: sensor_schema kwargs
SENSOR_DEFS = {
    CONF_ENERGY_TODAY: {
        "unit_of_measurement": UNIT_KILOWATT_HOURS,
        "icon": ICON_COUNTER,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_ENERGY,
        "state_class": STATE_CLASS_TOTAL_INCREASING,
    },
    CONF_ENERGY_TOTAL: {
        "unit_of_measurement": UNIT_KILOWATT_HOURS,
        "icon": ICON_COUNTER,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_ENERGY,
        "state_class": STATE_CLASS_TOTAL_INCREASING,
    },
    CONF_DC1_CURRENT: {
        "unit_of_measurement": UNIT_AMPERE,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_CURRENT,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_DC1_VOLTAGE: {
        "unit_of_measurement": UNIT_VOLT,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_DC2_CURRENT: {
        "unit_of_measurement": UNIT_AMPERE,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_CURRENT,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_DC2_VOLTAGE: {
        "unit_of_measurement": UNIT_VOLT,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_AC_CURRENT: {
        "unit_of_measurement": UNIT_AMPERE,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_CURRENT,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_AC_VOLTAGE: {
        "unit_of_measurement": UNIT_VOLT,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_AC_FREQUENCY: {
        "unit_of_measurement": UNIT_HERTZ,
        "accuracy_decimals": 2,
        "device_class": DEVICE_CLASS_FREQUENCY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_AC_POWER: {
        "unit_of_measurement": UNIT_WATT,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_POWER,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_RUNTIME_TOTAL: {
        "unit_of_measurement": UNIT_HOURS,
        "icon": ICON_TIMER,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_TOTAL_INCREASING,
    },
    CONF_ERROR_BITS: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": ICON_ERROR_BITS,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
    },
    CONF_MODE: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": ICON_MODE,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
    },
    CONF_TEMPERATURE: {
        "unit_of_measurement": UNIT_CELSIUS,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_GRID_VOLTAGE_FAULT: {
        "unit_of_measurement": UNIT_VOLT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_GRID_FREQUENCY_FAULT: {
        "unit_of_measurement": UNIT_HERTZ,
        "accuracy_decimals": 2,
        "device_class": DEVICE_CLASS_FREQUENCY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_DC_INJECTION_FAULT: {
        "unit_of_measurement": UNIT_AMPERE,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_CURRENT,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_TEMPERATURE_FAULT: {
        "unit_of_measurement": UNIT_CELSIUS,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_PV1_VOLTAGE_FAULT: {
        "unit_of_measurement": UNIT_VOLT,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_PV2_VOLTAGE_FAULT: {
        "unit_of_measurement": UNIT_VOLT,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_GFC_FAULT: {
        "unit_of_measurement": UNIT_AMPERE,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_CURRENT,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
}

SENSORS = list(SENSOR_DEFS)

CONFIG_SCHEMA = CONF_SOLAX_X1_MINI_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(key): sensor.sensor_schema(**kwargs)
        for key, kwargs in SENSOR_DEFS.items()
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_SOLAX_X1_MINI_ID])
    for key in SENSORS:
        if key in config:
            conf = config[key]
            sens = await sensor.new_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_sensor")(sens))
