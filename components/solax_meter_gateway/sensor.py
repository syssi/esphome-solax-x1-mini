import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    DEVICE_CLASS_POWER,
    ICON_EMPTY,
    STATE_CLASS_MEASUREMENT,
    UNIT_WATT,
)

from . import CONF_SOLAX_METER_GATEWAY_COMPONENT_SCHEMA, CONF_SOLAX_METER_GATEWAY_ID

DEPENDENCIES = ["solax_meter_gateway"]
CODEOWNERS = ["@syssi"]

CONF_POWER_DEMAND = "power_demand"

SENSOR_DEFS = {
    CONF_POWER_DEMAND: {
        "unit_of_measurement": UNIT_WATT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_POWER,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
}

CONFIG_SCHEMA = CONF_SOLAX_METER_GATEWAY_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(key): sensor.sensor_schema(**kwargs)
        for key, kwargs in SENSOR_DEFS.items()
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_SOLAX_METER_GATEWAY_ID])
    for key in SENSOR_DEFS:
        if key in config:
            sens = await sensor.new_sensor(config[key])
            cg.add(getattr(hub, f"set_{key}_sensor")(sens))
