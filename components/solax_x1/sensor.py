import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, modbus_solax
from esphome.const import (
    CONF_ID,
    CONF_MODE,
    CONF_TEMPERATURE,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_VOLTAGE,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_ENERGY,
    DEVICE_CLASS_TEMPERATURE,
    ICON_EMPTY,
    ICON_CURRENT_AC,
    ICON_COUNTER,
    ICON_TIMER,
    UNIT_CELSIUS,
    UNIT_HERTZ,
    UNIT_VOLT,
    UNIT_AMPERE,
    UNIT_WATT,
    UNIT_EMPTY,
)

AUTO_LOAD = ["modbus_solax"]
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

UNIT_HOURS = "h"
UNIT_KILO_WATT_HOURS = "kWh"

ICON_MODE = "mdi:heart-pulse"
ICON_ERROR_BITS = "mdi:alert-circle-outline"

SENSORS = [
    CONF_ENERGY_TODAY,
    CONF_ENERGY_TOTAL,
    CONF_DC1_CURRENT,
    CONF_DC1_VOLTAGE,
    CONF_DC2_CURRENT,
    CONF_DC2_VOLTAGE,
    CONF_AC_CURRENT,
    CONF_AC_VOLTAGE,
    CONF_AC_FREQUENCY,
    CONF_AC_POWER,
    CONF_RUNTIME_TOTAL,
    CONF_ERROR_BITS,
    CONF_MODE,
    CONF_TEMPERATURE,
]

solax_x1_ns = cg.esphome_ns.namespace("solax_x1")
SolaxX1 = solax_x1_ns.class_(
    "SolaxX1", cg.PollingComponent, modbus_solax.ModbusSolaxDevice
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SolaxX1),
            cv.Optional(CONF_ENERGY_TODAY): sensor.sensor_schema(
                UNIT_KILO_WATT_HOURS, ICON_COUNTER, 3, DEVICE_CLASS_ENERGY
            ),
            cv.Optional(CONF_ENERGY_TOTAL): sensor.sensor_schema(
                UNIT_KILO_WATT_HOURS, ICON_COUNTER, 3, DEVICE_CLASS_ENERGY
            ),
            cv.Optional(CONF_DC1_CURRENT): sensor.sensor_schema(
                UNIT_AMPERE, ICON_EMPTY, 1, DEVICE_CLASS_CURRENT
            ),
            cv.Optional(CONF_DC1_VOLTAGE): sensor.sensor_schema(
                UNIT_VOLT, ICON_EMPTY, 1, DEVICE_CLASS_VOLTAGE
            ),
            cv.Optional(CONF_DC2_CURRENT): sensor.sensor_schema(
                UNIT_AMPERE, ICON_EMPTY, 1, DEVICE_CLASS_CURRENT
            ),
            cv.Optional(CONF_DC2_VOLTAGE): sensor.sensor_schema(
                UNIT_VOLT, ICON_EMPTY, 1, DEVICE_CLASS_VOLTAGE
            ),
            cv.Optional(CONF_AC_CURRENT): sensor.sensor_schema(
                UNIT_AMPERE, ICON_EMPTY, 2, DEVICE_CLASS_CURRENT
            ),
            cv.Optional(CONF_AC_VOLTAGE): sensor.sensor_schema(
                UNIT_VOLT, ICON_EMPTY, 1, DEVICE_CLASS_VOLTAGE
            ),
            cv.Optional(CONF_AC_FREQUENCY): sensor.sensor_schema(
                UNIT_HERTZ, ICON_CURRENT_AC, 1, DEVICE_CLASS_EMPTY
            ),
            cv.Optional(CONF_AC_POWER): sensor.sensor_schema(
                UNIT_WATT, ICON_EMPTY, 0, DEVICE_CLASS_POWER
            ),
            cv.Optional(CONF_RUNTIME_TOTAL): sensor.sensor_schema(
                UNIT_HOURS, ICON_TIMER, 0, DEVICE_CLASS_EMPTY
            ),
            cv.Optional(CONF_ERROR_BITS): sensor.sensor_schema(
                UNIT_EMPTY, ICON_ERROR_BITS, 0, DEVICE_CLASS_EMPTY
            ),
            cv.Optional(CONF_MODE): sensor.sensor_schema(
                UNIT_EMPTY, ICON_MODE, 0, DEVICE_CLASS_EMPTY
            ),
            cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
                UNIT_CELSIUS, ICON_EMPTY, 1, DEVICE_CLASS_TEMPERATURE
            ),
        }
    )
    .extend(cv.polling_component_schema("30s"))
    .extend(modbus_solax.modbus_solax_device_schema())
)


def validate_serial_number(value):
    value = cv.string_strict(value)
    parts = [value[i : i + 2] for i in range(0, len(value), 2)]
    if len(parts) != 14:
        raise cv.Invalid("Serial number must consist of 14 hexadecimal numbers")
    parts_int = []
    if any(len(part) != 2 for part in parts):
        raise cv.Invalid("Serial number must be format XX")
    for part in parts:
        try:
            parts_int.append(int(part, 16))
        except ValueError:
            # pylint: disable=raise-missing-from
            raise cv.Invalid("Serial number must be hex values from 00 to FF")

    return "".join(f"{part:02X}" for part in parts_int)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield modbus_solax.register_modbus_solax_device(var, config)

    for key in SENSORS:
        if key in config:
            conf = config[key]
            sens = yield sensor.new_sensor(conf)
            cg.add(getattr(var, f"set_{key}_sensor")(sens))
