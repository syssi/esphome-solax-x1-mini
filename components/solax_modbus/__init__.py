from esphome import pins
import esphome.codegen as cg
from esphome.components import uart
import esphome.config_validation as cv
from esphome.const import CONF_ADDRESS, CONF_FLOW_CONTROL_PIN, CONF_ID
from esphome.cpp_helpers import gpio_pin_expression

CODEOWNERS = ["@syssi"]

DEPENDENCIES = ["uart"]
MULTI_CONF = True

CONF_SOLAX_MODBUS_ID = "solax_modbus_id"
CONF_SERIAL_NUMBER = "serial_number"

solax_modbus_ns = cg.esphome_ns.namespace("solax_modbus")
SolaxModbus = solax_modbus_ns.class_("SolaxModbus", cg.Component, uart.UARTDevice)
SolaxModbusDevice = solax_modbus_ns.class_("SolaxModbusDevice")

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SolaxModbus),
            cv.Optional(CONF_FLOW_CONTROL_PIN): pins.gpio_output_pin_schema,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
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


def as_hex_array(value):
    cpp_array = [
        f"0x{part}" for part in [value[i : i + 2] for i in range(0, len(value), 2)]
    ]
    return cg.RawExpression(f"(uint8_t*)(const uint8_t[16]){{{','.join(cpp_array)}}}")


async def to_code(config):
    cg.add_global(solax_modbus_ns.using)
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    await uart.register_uart_device(var, config)

    if CONF_FLOW_CONTROL_PIN in config:
        pin = await gpio_pin_expression(config[CONF_FLOW_CONTROL_PIN])
        cg.add(var.set_flow_control_pin(pin))


def solax_modbus_device_schema(default_address, default_serial):
    schema = {
        cv.GenerateID(CONF_SOLAX_MODBUS_ID): cv.use_id(SolaxModbus),
    }
    if default_address is None:
        schema[cv.Required(CONF_ADDRESS)] = cv.hex_uint8_t
    else:
        schema[cv.Optional(CONF_ADDRESS, default=default_address)] = cv.hex_uint8_t

    if default_address is None:
        schema[cv.Required(CONF_SERIAL_NUMBER)] = validate_serial_number
    else:
        schema[
            cv.Optional(CONF_SERIAL_NUMBER, default=default_serial)
        ] = validate_serial_number

    return cv.Schema(schema)


async def register_solax_modbus_device(var, config):
    parent = await cg.get_variable(config[CONF_SOLAX_MODBUS_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(var.set_serial_number(as_hex_array(config[CONF_SERIAL_NUMBER])))
    cg.add(parent.register_device(var))
