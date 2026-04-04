"""Schema structure tests for solax ESPHome component modules."""

import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

import components.solax_meter_gateway as hub_gateway  # noqa: E402
from components.solax_meter_gateway import (  # noqa: E402
    number as gateway_number,
    switch as gateway_switch,
    text_sensor as gateway_text_sensor,
)
import components.solax_x1_mini as hub  # noqa: E402
from components.solax_x1_mini import sensor, text_sensor  # noqa: E402


class TestHubConstants:
    def test_conf_ids_defined(self):
        assert hub.CONF_SOLAX_X1_MINI_ID == "solax_x1_mini_id"
        assert hub_gateway.CONF_SOLAX_METER_GATEWAY_ID == "solax_meter_gateway_id"


class TestSolaxX1MiniSensorLists:
    def test_sensor_defs_completeness(self):
        assert "energy_today" in sensor.SENSOR_DEFS
        assert "dc1_voltage" in sensor.SENSOR_DEFS
        assert len(sensor.SENSOR_DEFS) == 21


class TestSolaxX1MiniTextSensorConstants:
    def test_text_sensor_consts_defined(self):
        assert text_sensor.CONF_MODE_NAME == "mode_name"
        assert text_sensor.CONF_ERRORS == "errors"


class TestSolaxMeterGatewayTextSensorConstants:
    def test_text_sensors_list(self):
        assert (
            gateway_text_sensor.CONF_OPERATION_MODE in gateway_text_sensor.TEXT_SENSORS
        )
        assert len(gateway_text_sensor.TEXT_SENSORS) == 1


class TestSolaxMeterGatewaySwitchConstants:
    def test_switches_list(self):
        assert gateway_switch.CONF_MANUAL_MODE in gateway_switch.SWITCHES
        assert gateway_switch.CONF_EMERGENCY_POWER_OFF in gateway_switch.SWITCHES
        assert len(gateway_switch.SWITCHES) == 2


class TestSolaxMeterGatewayNumberConstants:
    def test_numbers_dict(self):
        assert gateway_number.CONF_MANUAL_POWER_DEMAND in gateway_number.NUMBERS
        assert len(gateway_number.NUMBERS) == 1

    def test_number_addresses_are_unique(self):
        addresses = list(gateway_number.NUMBERS.values())
        assert len(addresses) == len(set(addresses))
