import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_ID, CONF_RESTORE_MODE

from .. import CONF_SOLAX_METER_GATEWAY_ID, SolaxMeterGateway, solax_meter_gateway_ns

DEPENDENCIES = ["solax_meter_gateway"]

CODEOWNERS = ["@syssi"]

CONF_MANUAL_MODE = "manual_mode"
CONF_EMERGENCY_POWER_OFF = "emergency_power_off"

ICON_MANUAL_MODE = "mdi:auto-fix"
ICON_EMERGENCY_POWER_OFF = "mdi:power"

SWITCHES = [
    CONF_MANUAL_MODE,
    CONF_EMERGENCY_POWER_OFF,
]

SolaxSwitch = solax_meter_gateway_ns.class_("SolaxSwitch", switch.Switch, cg.Component)
SolaxSwitchRestoreMode = solax_meter_gateway_ns.enum("SolaxSwitchRestoreMode")

RESTORE_MODES = {
    "RESTORE_DEFAULT_OFF": SolaxSwitchRestoreMode.SOLAX_SWITCH_RESTORE_DEFAULT_OFF,
    "RESTORE_DEFAULT_ON": SolaxSwitchRestoreMode.SOLAX_SWITCH_RESTORE_DEFAULT_ON,
    "ALWAYS_OFF": SolaxSwitchRestoreMode.SOLAX_SWITCH_ALWAYS_OFF,
    "ALWAYS_ON": SolaxSwitchRestoreMode.SOLAX_SWITCH_ALWAYS_ON,
}

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SOLAX_METER_GATEWAY_ID): cv.use_id(SolaxMeterGateway),
        cv.Optional(CONF_MANUAL_MODE): switch.SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(SolaxSwitch),
                cv.Optional(CONF_ICON, default=ICON_MANUAL_MODE): cv.icon,
                cv.Optional(CONF_RESTORE_MODE, default="RESTORE_DEFAULT_OFF"): cv.enum(
                    RESTORE_MODES, upper=True, space="_"
                ),
            }
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_EMERGENCY_POWER_OFF): switch.SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(SolaxSwitch),
                cv.Optional(CONF_ICON, default=ICON_EMERGENCY_POWER_OFF): cv.icon,
                cv.Optional(CONF_RESTORE_MODE, default="RESTORE_DEFAULT_OFF"): cv.enum(
                    RESTORE_MODES, upper=True, space="_"
                ),
            }
        ).extend(cv.COMPONENT_SCHEMA),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_SOLAX_METER_GATEWAY_ID])
    for key in SWITCHES:
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await switch.register_switch(var, conf)
            cg.add(getattr(hub, f"set_{key}_switch")(var))
            cg.add(var.set_parent(hub))
            cg.add(var.set_restore_mode(conf[CONF_RESTORE_MODE]))
