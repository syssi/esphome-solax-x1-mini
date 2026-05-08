#include <gtest/gtest.h>
#include "common.h"
#include "frames.h"

namespace esphome::solax_x1_mini::testing {

// ── G2 status frame ───────────────────────────────────────────────────────────

TEST(SolaxX1MiniStatusTest, Temperature) {
  TestableSolaxX1Mini bms;
  sensor::Sensor temp;
  bms.set_temperature_sensor(&temp);

  bms.on_solax_modbus_data(FUNCTION_STATUS_REPORT, G2_STATUS_FRAME);

  EXPECT_FLOAT_EQ(temp.state, 33.0f);
}

TEST(SolaxX1MiniStatusTest, EnergyAndDcVoltage) {
  TestableSolaxX1Mini bms;
  sensor::Sensor energy_today, dc1v, dc2v, dc1a, dc2a;
  bms.set_energy_today_sensor(&energy_today);
  bms.set_dc1_voltage_sensor(&dc1v);
  bms.set_dc2_voltage_sensor(&dc2v);
  bms.set_dc1_current_sensor(&dc1a);
  bms.set_dc2_current_sensor(&dc2a);

  bms.on_solax_modbus_data(FUNCTION_STATUS_REPORT, G2_STATUS_FRAME);

  EXPECT_NEAR(energy_today.state, 0.2f, 0.01f);
  EXPECT_NEAR(dc1v.state, 202.8f, 0.1f);
  EXPECT_NEAR(dc2v.state, 0.0f, 0.01f);
  EXPECT_NEAR(dc1a.state, 2.9f, 0.1f);
  EXPECT_NEAR(dc2a.state, 0.0f, 0.01f);
}

TEST(SolaxX1MiniStatusTest, AcMeasurements) {
  TestableSolaxX1Mini bms;
  sensor::Sensor ac_curr, ac_volt, ac_freq, ac_power;
  bms.set_ac_current_sensor(&ac_curr);
  bms.set_ac_voltage_sensor(&ac_volt);
  bms.set_ac_frequency_sensor(&ac_freq);
  bms.set_ac_power_sensor(&ac_power);

  bms.on_solax_modbus_data(FUNCTION_STATUS_REPORT, G2_STATUS_FRAME);

  EXPECT_NEAR(ac_curr.state, 2.4f, 0.1f);
  EXPECT_NEAR(ac_volt.state, 238.9f, 0.1f);
  EXPECT_NEAR(ac_freq.state, 49.92f, 0.01f);
  EXPECT_NEAR(ac_power.state, 555.0f, 1.0f);
}

TEST(SolaxX1MiniStatusTest, EnergyTotal) {
  TestableSolaxX1Mini bms;
  sensor::Sensor energy_total;
  bms.set_energy_total_sensor(&energy_total);

  bms.on_solax_modbus_data(FUNCTION_STATUS_REPORT, G2_STATUS_FRAME);

  EXPECT_NEAR(energy_total.state, 2398.3f, 0.1f);
}

TEST(SolaxX1MiniStatusTest, RuntimeTotal) {
  TestableSolaxX1Mini bms;
  sensor::Sensor runtime;
  bms.set_runtime_total_sensor(&runtime);

  bms.on_solax_modbus_data(FUNCTION_STATUS_REPORT, G2_STATUS_FRAME);

  EXPECT_FLOAT_EQ(runtime.state, 4176.0f);
}

TEST(SolaxX1MiniStatusTest, Mode) {
  TestableSolaxX1Mini bms;
  sensor::Sensor mode;
  bms.set_mode_sensor(&mode);

  bms.on_solax_modbus_data(FUNCTION_STATUS_REPORT, G2_STATUS_FRAME);

  EXPECT_FLOAT_EQ(mode.state, 2.0f);
}

TEST(SolaxX1MiniStatusTest, ModeText) {
  TestableSolaxX1Mini bms;
  text_sensor::TextSensor mode_name;
  bms.set_mode_name_text_sensor(&mode_name);

  bms.on_solax_modbus_data(FUNCTION_STATUS_REPORT, G2_STATUS_FRAME);

  EXPECT_EQ(mode_name.state, "Normal");
}

TEST(SolaxX1MiniStatusTest, FaultSensors) {
  TestableSolaxX1Mini bms;
  sensor::Sensor gv_fault, gf_fault, dci_fault, temp_fault, pv1_fault, pv2_fault, gfc_fault;
  bms.set_grid_voltage_fault_sensor(&gv_fault);
  bms.set_grid_frequency_fault_sensor(&gf_fault);
  bms.set_dc_injection_fault_sensor(&dci_fault);
  bms.set_temperature_fault_sensor(&temp_fault);
  bms.set_pv1_voltage_fault_sensor(&pv1_fault);
  bms.set_pv2_voltage_fault_sensor(&pv2_fault);
  bms.set_gfc_fault_sensor(&gfc_fault);

  bms.on_solax_modbus_data(FUNCTION_STATUS_REPORT, G2_STATUS_FRAME);

  EXPECT_NEAR(gv_fault.state, 0.0f, 0.01f);
  EXPECT_NEAR(gf_fault.state, 0.0f, 0.01f);
  EXPECT_NEAR(dci_fault.state, 0.0f, 0.01f);
  EXPECT_NEAR(temp_fault.state, 0.0f, 0.01f);
  EXPECT_NEAR(pv1_fault.state, 0.0f, 0.01f);
  EXPECT_NEAR(pv2_fault.state, 0.0f, 0.01f);
  EXPECT_NEAR(gfc_fault.state, 0.0f, 0.01f);
}

TEST(SolaxX1MiniStatusTest, ErrorBitsClean) {
  TestableSolaxX1Mini bms;
  sensor::Sensor error_bits;
  text_sensor::TextSensor errors;
  bms.set_error_bits_sensor(&error_bits);
  bms.set_errors_text_sensor(&errors);

  bms.on_solax_modbus_data(FUNCTION_STATUS_REPORT, G2_STATUS_FRAME);

  EXPECT_FLOAT_EQ(error_bits.state, 0.0f);
  EXPECT_EQ(errors.state, "");
}

// ── Null sensors do not crash ─────────────────────────────────────────────────

TEST(SolaxX1MiniSafetyTest, NullSensorsDoNotCrash) {
  TestableSolaxX1Mini bms;

  EXPECT_NO_FATAL_FAILURE(bms.on_solax_modbus_data(FUNCTION_STATUS_REPORT, G2_STATUS_FRAME));
}

}  // namespace esphome::solax_x1_mini::testing
