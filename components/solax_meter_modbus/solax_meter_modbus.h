#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

#include <vector>

namespace esphome {
namespace solax_meter_modbus {

class SolaxMeterModbusDevice;

class SolaxMeterModbus : public uart::UARTDevice, public Component {
 public:
  SolaxMeterModbus() = default;

  void setup() override;

  void loop() override;

  void dump_config() override;

  void register_device(SolaxMeterModbusDevice *device) { this->devices_.push_back(device); }

  float get_setup_priority() const override;

  void send(uint8_t address, int16_t power);
  void send(uint8_t address, float power);
  void send_raw(const std::vector<uint8_t> &payload);
  void set_flow_control_pin(GPIOPin *flow_control_pin) { this->flow_control_pin_ = flow_control_pin; }

 protected:
  GPIOPin *flow_control_pin_{nullptr};

  bool parse_solax_meter_modbus_byte_(uint8_t byte);
  std::vector<uint8_t> rx_buffer_;
  uint32_t last_solax_meter_modbus_byte_{0};
  std::vector<SolaxMeterModbusDevice *> devices_;
};

class SolaxMeterModbusDevice {
 public:
  void set_parent(SolaxMeterModbus *parent) { parent_ = parent; }
  void set_address(uint8_t address) { address_ = address; }
  virtual void on_solax_meter_modbus_data(const std::vector<uint8_t> &data) = 0;
  void send(int16_t power) { this->parent_->send(this->address_, power); }
  void send(float power) { this->parent_->send(this->address_, power); }
  void send_raw(const std::vector<uint8_t> &payload) { this->parent_->send_raw(payload); }

 protected:
  friend SolaxMeterModbus;

  SolaxMeterModbus *parent_;
  uint8_t address_;
};

}  // namespace solax_meter_modbus
}  // namespace esphome
