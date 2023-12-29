#pragma once

#include "esphome/core/component.h"
#include "esphome/components/number/number.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/solax_meter_modbus/solax_meter_modbus.h"

namespace esphome {
namespace solax_meter_gateway {

class SolaxMeterGateway : public PollingComponent, public solax_meter_modbus::SolaxMeterModbusDevice {
 public:
  void set_manual_power_demand_number(number::Number *manual_power_demand_number) {
    manual_power_demand_number_ = manual_power_demand_number;
  }

  void set_power_sensor(sensor::Sensor *power_sensor) { power_sensor_ = power_sensor; }
  void set_power_demand_sensor(sensor::Sensor *power_demand_sensor) { power_demand_sensor_ = power_demand_sensor; }
  void set_power_sensor_inactivity_timeout(uint16_t power_sensor_inactivity_timeout_s) {
    this->power_sensor_inactivity_timeout_s_ = power_sensor_inactivity_timeout_s;
  }

  void set_manual_mode_switch(switch_::Switch *manual_mode_switch) { manual_mode_switch_ = manual_mode_switch; }
  void set_emergency_power_off_switch(switch_::Switch *emergency_power_off_switch) {
    emergency_power_off_switch_ = emergency_power_off_switch;
  }

  void set_operation_mode_text_sensor(text_sensor::TextSensor *operation_mode_text_sensor) {
    operation_mode_text_sensor_ = operation_mode_text_sensor;
  }

  void setup() override;

  void on_solax_meter_modbus_data(const std::vector<uint8_t> &data) override;

  void dump_config() override;

  void update() override;

  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  number::Number *manual_power_demand_number_;

  sensor::Sensor *power_sensor_;
  sensor::Sensor *power_demand_sensor_;

  switch_::Switch *manual_mode_switch_;
  switch_::Switch *emergency_power_off_switch_;

  text_sensor::TextSensor *operation_mode_text_sensor_;

  float power_demand_;
  uint16_t power_sensor_inactivity_timeout_s_{0};
  uint32_t last_power_demand_received_{0};

  void publish_state_(sensor::Sensor *sensor, float value);
  void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);
  bool inactivity_timeout_();
};

}  // namespace solax_meter_gateway
}  // namespace esphome
