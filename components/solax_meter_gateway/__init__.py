import esphome.codegen as cg
from esphome.components import sensor, solax_meter_modbus
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@syssi"]

DEPENDENCIES = ["solax_meter_modbus"]
AUTO_LOAD = ["number", "sensor", "switch", "text_sensor"]
MULTI_CONF = True

CONF_SOLAX_METER_GATEWAY_ID = "solax_meter_gateway_id"
CONF_POWER_ID = "power_id"
CONF_POWER_SENSOR_INACTIVITY_TIMEOUT = "power_sensor_inactivity_timeout"
CONF_OPERATION_MODE_ID = "operation_mode_id"

DEFAULT_MIN_POWER_DEMAND = 0
DEFAULT_MAX_POWER_DEMAND = 600

solax_meter_gateway_ns = cg.esphome_ns.namespace("solax_meter_gateway")
SolaxMeterGateway = solax_meter_gateway_ns.class_(
    "SolaxMeterGateway",
    cg.PollingComponent,
    solax_meter_modbus.SolaxMeterModbusDevice,
)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SolaxMeterGateway),
            cv.Required(CONF_POWER_ID): cv.use_id(sensor.Sensor),
            cv.Optional(
                CONF_POWER_SENSOR_INACTIVITY_TIMEOUT, default="5s"
            ): cv.positive_time_period_seconds,
        }
    )
    .extend(solax_meter_modbus.solax_meter_modbus_device_schema(0x01))
    .extend(cv.polling_component_schema("never")),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await solax_meter_modbus.register_solax_meter_modbus_device(var, config)

    power_sensor = await cg.get_variable(config[CONF_POWER_ID])

    cg.add(var.set_power_sensor(power_sensor))
    cg.add(
        var.set_power_sensor_inactivity_timeout(
            config[CONF_POWER_SENSOR_INACTIVITY_TIMEOUT]
        )
    )
