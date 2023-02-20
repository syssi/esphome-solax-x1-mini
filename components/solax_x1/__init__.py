import esphome.codegen as cg
from esphome.components import solax_modbus
import esphome.config_validation as cv
from esphome.const import CONF_ID

AUTO_LOAD = ["solax_modbus", "sensor", "text_sensor"]
CODEOWNERS = ["@syssi"]
MULTI_CONF = True

CONF_SOLAX_X1_ID = "solax_x1_id"

solax_x1_ns = cg.esphome_ns.namespace("solax_x1")
SolaxX1 = solax_x1_ns.class_(
    "SolaxX1", cg.PollingComponent, solax_modbus.SolaxModbusDevice
)

CONFIG_SCHEMA = (
    cv.Schema({cv.GenerateID(): cv.declare_id(SolaxX1)})
    .extend(cv.polling_component_schema("30s"))
    .extend(
        solax_modbus.solax_modbus_device_schema(0x0A, "3132333435363737363534333231")
    )
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await solax_modbus.register_solax_modbus_device(var, config)
