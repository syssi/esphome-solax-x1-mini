import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_ID

from . import CONF_SOLAX_X1_MINI_ID, SolaxX1Mini

DEPENDENCIES = ["solax_x1_mini"]

CONF_MODE_NAME = "mode_name"
CONF_ERRORS = "errors"

ICON_MODE_NAME = "mdi:heart-pulse"
ICON_ERRORS = "mdi:alert-circle-outline"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SOLAX_X1_MINI_ID): cv.use_id(SolaxX1Mini),
        cv.Optional(CONF_MODE_NAME): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_MODE_NAME): cv.icon,
            }
        ),
        cv.Optional(CONF_ERRORS): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_ERRORS): cv.icon,
            }
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
