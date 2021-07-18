import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import CONF_ID, CONF_ICON
from . import SolaxX1, CONF_SOLAX_X1_ID

DEPENDENCIES = ["solax_x1"]

CONF_MODE_NAME = "mode_name"
CONF_ERRORS = "errors"

ICON_MODE_NAME = "mdi:heart-pulse"
ICON_ERRORS = "mdi:alert-circle-outline"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SOLAX_X1_ID): cv.use_id(SolaxX1),
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


def to_code(config):
    hub = yield cg.get_variable(config[CONF_SOLAX_X1_ID])
    for key in [CONF_MODE_NAME, CONF_ERRORS]:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            yield text_sensor.register_text_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))
