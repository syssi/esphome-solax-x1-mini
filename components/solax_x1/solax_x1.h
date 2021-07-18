#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/modbus_solax/modbus_solax.h"

namespace esphome {
namespace solax_x1 {

static const uint8_t REDISCOVERY_THRESHOLD = 5;

struct SolaxInfoT {
  char Type;
  char RatedPower[6];
  char FirmwareVersion[5];
  char ModuleName[14];
  char Manufacturer[14];
  char SerialNumber[14];
  char RatedBusVoltage[4];
};

class SolaxX1 : public PollingComponent, public modbus_solax::ModbusSolaxDevice {
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
  uint8_t get_no_response_count() { return no_response_count_; }

  void update() override;

  void on_modbus_solax_data(const std::vector<uint8_t> &data) override;
  void on_modbus_solax_info(const std::vector<uint8_t> &data) override;

  void dump_config() override;

 protected:
  void publish_device_offline_();
  std::string error_bits_to_string_(const uint32_t bitmask);

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
  text_sensor::TextSensor *mode_name_text_sensor_;
  text_sensor::TextSensor *errors_text_sensor_;
  uint8_t no_response_count_ = REDISCOVERY_THRESHOLD;
};

}  // namespace solax_x1
}  // namespace esphome
