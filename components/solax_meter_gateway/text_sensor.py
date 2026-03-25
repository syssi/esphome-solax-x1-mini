import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID

from . import CONF_SOLAX_METER_GATEWAY_COMPONENT_SCHEMA, CONF_SOLAX_METER_GATEWAY_ID

DEPENDENCIES = ["solax_meter_gateway"]

CODEOWNERS = ["@syssi"]

CONF_OPERATION_MODE = "operation_mode"

ICON_OPERATION_MODE = "mdi:heart-pulse"

TEXT_SENSORS = [
    CONF_OPERATION_MODE,
]

CONFIG_SCHEMA = CONF_SOLAX_METER_GATEWAY_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_OPERATION_MODE): text_sensor.text_sensor_schema(
            text_sensor.TextSensor,
            icon=ICON_OPERATION_MODE,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_SOLAX_METER_GATEWAY_ID])
    for key in TEXT_SENSORS:
        if key in config:
            conf = config[key]
            sens = await text_sensor.new_text_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))
