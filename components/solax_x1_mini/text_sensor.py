import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID

from . import CONF_SOLAX_X1_MINI_COMPONENT_SCHEMA, CONF_SOLAX_X1_MINI_ID

DEPENDENCIES = ["solax_x1_mini"]

CONF_MODE_NAME = "mode_name"
CONF_ERRORS = "errors"

ICON_MODE_NAME = "mdi:heart-pulse"
ICON_ERRORS = "mdi:alert-circle-outline"

CONFIG_SCHEMA = CONF_SOLAX_X1_MINI_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_MODE_NAME): text_sensor.text_sensor_schema(
            text_sensor.TextSensor, icon=ICON_MODE_NAME
        ),
        cv.Optional(CONF_ERRORS): text_sensor.text_sensor_schema(
            text_sensor.TextSensor, icon=ICON_ERRORS
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_SOLAX_X1_MINI_ID])
    for key in [CONF_MODE_NAME, CONF_ERRORS]:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await text_sensor.register_text_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))
