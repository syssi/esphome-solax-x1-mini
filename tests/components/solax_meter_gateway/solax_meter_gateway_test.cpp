#include "esphome/components/solax_meter_gateway/solax_meter_gateway.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/core/hal.h"
#include "common.h"
#include "frames.h"
#include <cmath>
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

// ── Inactivity tracking: inverter-side ───────────────────────────────────────
//
// last_solax_request_received_ starts at 0; millis() on the host is always
// far above any inactivity threshold, so update() fires "Standby" immediately
// when the timestamp has never been updated.  After on_solax_meter_modbus_data()
// the timestamp is current, so a back-to-back update() must stay silent.

TEST(SolaxMeterGatewayInactivityTest, UpdatePublishesStandbyWhenInverterNeverPolled) {
  TestableSolaxMeterGateway gw;
  text_sensor::TextSensor op_mode;
  gw.set_operation_mode_text_sensor(&op_mode);

  gw.call_update();

  EXPECT_EQ(op_mode.state, "Standby");
}

TEST(SolaxMeterGatewayInactivityTest, UpdateSilentImmediatelyAfterInverterPoll) {
  TestableSolaxMeterGateway gw;
  text_sensor::TextSensor op_mode;
  gw.set_operation_mode_text_sensor(&op_mode);

  gw.on_solax_meter_modbus_data(READ_POWER_32BIT_FLOAT_REQUEST);
  gw.call_update();

  EXPECT_NE(op_mode.state, "Standby");
}

TEST(SolaxMeterGatewayInactivityTest, InverterPollSetsRequestTimestamp) {
  TestableSolaxMeterGateway gw;

  EXPECT_EQ(gw.get_last_solax_request_received(), 0u);
  gw.on_solax_meter_modbus_data(READ_POWER_32BIT_FLOAT_REQUEST);
  EXPECT_GT(gw.get_last_solax_request_received(), 0u);
}

// ── Inactivity tracking: power-sensor-side ────────────────────────────────────
//
// last_power_demand_received_ must NOT be touched by on_solax_meter_modbus_data().
// When it stays at 0 and a non-zero timeout is configured, the first inverter
// poll must trigger "Meter fault".  After the power sensor timestamp is current
// the safety path must not fire.

TEST(SolaxMeterGatewayInactivityTest, InverterPollDoesNotSetPowerSensorTimestamp) {
  TestableSolaxMeterGateway gw;

  gw.on_solax_meter_modbus_data(READ_POWER_32BIT_FLOAT_REQUEST);

  EXPECT_EQ(gw.get_last_power_demand_received(), 0u);
}

TEST(SolaxMeterGatewayInactivityTest, MeterFaultWhenPowerSensorSilent) {
  TestableSolaxMeterGateway gw;
  sensor::Sensor power_demand;
  text_sensor::TextSensor op_mode;
  gw.set_power_demand_sensor(&power_demand);
  gw.set_operation_mode_text_sensor(&op_mode);
  gw.set_power_sensor_inactivity_timeout(5);

  gw.on_solax_meter_modbus_data(READ_POWER_32BIT_FLOAT_REQUEST);

  EXPECT_EQ(op_mode.state, "Meter fault");
  EXPECT_TRUE(std::isnan(power_demand.state));
}

TEST(SolaxMeterGatewayInactivityTest, NoMeterFaultWhenPowerSensorRecent) {
  TestableSolaxMeterGateway gw;
  sensor::Sensor power_demand;
  text_sensor::TextSensor op_mode;
  gw.set_power_demand_sensor(&power_demand);
  gw.set_operation_mode_text_sensor(&op_mode);
  gw.set_power_sensor_inactivity_timeout(5);
  gw.set_last_power_demand_received(millis());

  gw.on_solax_meter_modbus_data(READ_POWER_32BIT_FLOAT_REQUEST);

  EXPECT_NE(op_mode.state, "Meter fault");
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
