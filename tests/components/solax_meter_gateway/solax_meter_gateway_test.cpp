#include "esphome/components/solax_meter_gateway/solax_meter_gateway.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "common.h"
#include "frames.h"
#include <gtest/gtest.h>

namespace esphome::solax_meter_gateway::testing {

// ── Auto mode: 32-bit float register ─────────────────────────────────────────

TEST(SolaxMeterGatewayTest, AutoMode32BitFloat) {
  TestableSolaxMeterGateway gw;
  sensor::Sensor power_demand;
  text_sensor::TextSensor op_mode;
  gw.set_power_demand_sensor(&power_demand);
  gw.set_operation_mode_text_sensor(&op_mode);
  gw.set_power_demand(500.0f);

  gw.on_solax_meter_modbus_data(READ_POWER_32BIT_FLOAT_REQUEST);

  EXPECT_FLOAT_EQ(power_demand.state, 500.0f);
  EXPECT_EQ(op_mode.state, "Auto");
}

// ── Auto mode: 16-bit sint register ──────────────────────────────────────────

TEST(SolaxMeterGatewayTest, AutoMode16BitSint) {
  TestableSolaxMeterGateway gw;
  sensor::Sensor power_demand;
  text_sensor::TextSensor op_mode;
  gw.set_power_demand_sensor(&power_demand);
  gw.set_operation_mode_text_sensor(&op_mode);
  gw.set_power_demand(300.0f);

  gw.on_solax_meter_modbus_data(READ_POWER_16BIT_SINT_REQUEST);

  EXPECT_FLOAT_EQ(power_demand.state, 300.0f);
  EXPECT_EQ(op_mode.state, "Auto");
}

// ── Emergency power off: demand forced to zero ────────────────────────────────

TEST(SolaxMeterGatewayTest, EmergencyPowerOff) {
  TestableSolaxMeterGateway gw;
  sensor::Sensor power_demand;
  text_sensor::TextSensor op_mode;
  TestSwitch emergency_off;
  gw.set_power_demand_sensor(&power_demand);
  gw.set_operation_mode_text_sensor(&op_mode);
  gw.set_emergency_power_off_switch(&emergency_off);
  emergency_off.publish_state(true);
  gw.set_power_demand(500.0f);

  gw.on_solax_meter_modbus_data(READ_POWER_32BIT_FLOAT_REQUEST);

  EXPECT_FLOAT_EQ(power_demand.state, 0.0f);
  EXPECT_EQ(op_mode.state, "Off");
}

// ── Manual mode: uses manual power demand (no number entity → 0W) ─────────────

TEST(SolaxMeterGatewayTest, ManualModeNoNumber) {
  TestableSolaxMeterGateway gw;
  sensor::Sensor power_demand;
  text_sensor::TextSensor op_mode;
  TestSwitch manual_mode;
  gw.set_power_demand_sensor(&power_demand);
  gw.set_operation_mode_text_sensor(&op_mode);
  gw.set_manual_mode_switch(&manual_mode);
  manual_mode.publish_state(true);
  gw.set_power_demand(500.0f);

  gw.on_solax_meter_modbus_data(READ_POWER_32BIT_FLOAT_REQUEST);

  EXPECT_EQ(op_mode.state, "Manual");
  EXPECT_FLOAT_EQ(power_demand.state, 0.0f);
}

// ── Null sensors do not crash ─────────────────────────────────────────────────

TEST(SolaxMeterGatewaySafetyTest, NullSensorsDoNotCrash) {
  TestableSolaxMeterGateway gw;
  EXPECT_NO_FATAL_FAILURE(gw.on_solax_meter_modbus_data(READ_POWER_32BIT_FLOAT_REQUEST));
  EXPECT_NO_FATAL_FAILURE(gw.on_solax_meter_modbus_data(READ_POWER_16BIT_SINT_REQUEST));
  EXPECT_NO_FATAL_FAILURE(gw.on_solax_meter_modbus_data(HANDSHAKE_REQUEST));
  EXPECT_NO_FATAL_FAILURE(gw.on_solax_meter_modbus_data(READ_TOTAL_ENERGY_IMPORT_REQUEST));
  EXPECT_NO_FATAL_FAILURE(gw.on_solax_meter_modbus_data(READ_TOTAL_ENERGY_EXPORT_REQUEST));
  EXPECT_NO_FATAL_FAILURE(gw.on_solax_meter_modbus_data(READ_TOTAL_ENERGY_REQUEST));
}

}  // namespace esphome::solax_meter_gateway::testing
