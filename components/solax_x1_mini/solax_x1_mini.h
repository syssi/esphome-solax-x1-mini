#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/solax_modbus/solax_modbus.h"

namespace esphome {
namespace solax_x1_mini {

static const uint8_t REDISCOVERY_THRESHOLD = 5;

class SolaxX1Mini : public PollingComponent, public solax_modbus::SolaxModbusDevice {
 public:
  void set_energy_today_sensor(sensor::Sensor *energy_today_sensor) { energy_today_sensor_ = energy_today_sensor; }
  void set_energy_total_sensor(sensor::Sensor *energy_total_sensor) { energy_total_sensor_ = energy_total_sensor; }
  void set_dc1_current_sensor(sensor::Sensor *dc1_current_sensor) { dc1_current_sensor_ = dc1_current_sensor; }
  void set_dc2_current_sensor(sensor::Sensor *dc2_current_sensor) { dc2_current_sensor_ = dc2_current_sensor; }
  void set_dc1_voltage_sensor(sensor::Sensor *dc1_voltage_sensor) { dc1_voltage_sensor_ = dc1_voltage_sensor; }
  void set_dc2_voltage_sensor(sensor::Sensor *dc2_voltage_sensor) { dc2_voltage_sensor_ = dc2_voltage_sensor; }
  void set_ac_current_sensor(sensor::Sensor *ac_current_sensor) { ac_current_sensor_ = ac_current_sensor; }
  void set_ac_frequency_sensor(sensor::Sensor *ac_frequency_sensor) { ac_frequency_sensor_ = ac_frequency_sensor; }
  void set_ac_power_sensor(sensor::Sensor *ac_power_sensor) { ac_power_sensor_ = ac_power_sensor; }
  void set_ac_voltage_sensor(sensor::Sensor *ac_voltage_sensor) { ac_voltage_sensor_ = ac_voltage_sensor; }
  void set_temperature_sensor(sensor::Sensor *temperature_sensor) { temperature_sensor_ = temperature_sensor; }
  void set_mode_sensor(sensor::Sensor *mode_sensor) { mode_sensor_ = mode_sensor; }
  void set_mode_name_text_sensor(text_sensor::TextSensor *sensor) { this->mode_name_text_sensor_ = sensor; }
  void set_error_bits_sensor(sensor::Sensor *error_bits_sensor) { error_bits_sensor_ = error_bits_sensor; }
  void set_errors_text_sensor(text_sensor::TextSensor *sensor) { this->errors_text_sensor_ = sensor; }
  void set_runtime_total_sensor(sensor::Sensor *runtime_total_sensor) { runtime_total_sensor_ = runtime_total_sensor; }
  void set_grid_voltage_fault_sensor(sensor::Sensor *grid_voltage_fault_sensor) {
    grid_voltage_fault_sensor_ = grid_voltage_fault_sensor;
  }
  void set_grid_frequency_fault_sensor(sensor::Sensor *grid_frequency_fault_sensor) {
    grid_frequency_fault_sensor_ = grid_frequency_fault_sensor;
  }
  void set_dc_injection_fault_sensor(sensor::Sensor *dc_injection_fault_sensor) {
    dc_injection_fault_sensor_ = dc_injection_fault_sensor;
  }
  void set_temperature_fault_sensor(sensor::Sensor *temperature_fault_sensor) {
    temperature_fault_sensor_ = temperature_fault_sensor;
  }
  void set_pv1_voltage_fault_sensor(sensor::Sensor *pv1_voltage_fault_sensor) {
    pv1_voltage_fault_sensor_ = pv1_voltage_fault_sensor;
  }
  void set_pv2_voltage_fault_sensor(sensor::Sensor *pv2_voltage_fault_sensor) {
    pv2_voltage_fault_sensor_ = pv2_voltage_fault_sensor;
  }
  void set_gfc_fault_sensor(sensor::Sensor *gfc_fault_sensor) { gfc_fault_sensor_ = gfc_fault_sensor; }

  uint8_t get_no_response_count() { return no_response_count_; }

  void update() override;
  void on_solax_modbus_data(const uint8_t &function, const std::vector<uint8_t> &data) override;
  void dump_config() override;

 protected:
  sensor::Sensor *energy_today_sensor_;
  sensor::Sensor *energy_total_sensor_;
  sensor::Sensor *dc1_current_sensor_;
  sensor::Sensor *dc2_current_sensor_;
  sensor::Sensor *dc1_voltage_sensor_;
  sensor::Sensor *dc2_voltage_sensor_;
  sensor::Sensor *ac_current_sensor_;
  sensor::Sensor *ac_frequency_sensor_;
  sensor::Sensor *ac_power_sensor_;
  sensor::Sensor *ac_voltage_sensor_;
  sensor::Sensor *temperature_sensor_;
  sensor::Sensor *mode_sensor_;
  sensor::Sensor *error_bits_sensor_;
  sensor::Sensor *runtime_total_sensor_;
  sensor::Sensor *grid_voltage_fault_sensor_;
  sensor::Sensor *grid_frequency_fault_sensor_;
  sensor::Sensor *dc_injection_fault_sensor_;
  sensor::Sensor *temperature_fault_sensor_;
  sensor::Sensor *pv1_voltage_fault_sensor_;
  sensor::Sensor *pv2_voltage_fault_sensor_;
  sensor::Sensor *gfc_fault_sensor_;

  text_sensor::TextSensor *mode_name_text_sensor_;
  text_sensor::TextSensor *errors_text_sensor_;
  uint8_t no_response_count_ = REDISCOVERY_THRESHOLD;

  void decode_device_info_(const std::vector<uint8_t> &data);
  void decode_status_report_(const std::vector<uint8_t> &data);
  void decode_config_settings_(const std::vector<uint8_t> &data);
  void publish_state_(sensor::Sensor *sensor, float value);
  void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);
  void publish_device_offline_();
  std::string error_bits_to_string_(uint32_t bitmask);
};

}  // namespace solax_x1_mini
}  // namespace esphome
