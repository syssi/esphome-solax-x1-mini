#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace modbus_solax {

struct SolaxMessageT {
  uint8_t Header[2];
  uint8_t Source[2];
  uint8_t Destination[2];
  uint8_t ControlCode;
  uint8_t FunctionCode;
  uint8_t DataLength;
  uint8_t Data[100];
};

class ModbusSolaxDevice;

class ModbusSolax : public uart::UARTDevice, public Component {
 public:
  ModbusSolax() = default;

  void setup() override;
  void loop() override;

  void dump_config() override;

  void register_device(ModbusSolaxDevice *device) { this->devices_.push_back(device); }
  void set_flow_control_pin(GPIOPin *flow_control_pin) { this->flow_control_pin_ = flow_control_pin; }

  float get_setup_priority() const override;

  void send(SolaxMessageT *tx_message);
  void query_status_report(uint8_t address);
  void query_device_info(uint8_t address);
  void query_config_settings(uint8_t address);
  void discover_devices();
  void register_address(uint8_t serial_number[14], uint8_t address);

 protected:
  bool parse_modbus_solax_byte_(uint8_t byte);
  GPIOPin *flow_control_pin_{nullptr};

  std::vector<uint8_t> rx_buffer_;
  uint32_t last_modbus_solax_byte_{0};
  std::vector<ModbusSolaxDevice *> devices_;
};

class ModbusSolaxDevice {
 public:
  void set_parent(ModbusSolax *parent) { parent_ = parent; }
  void set_address(uint8_t address) { address_ = address; }
  void set_serial_number(uint8_t *serial_number) { serial_number_ = serial_number; }
  virtual void on_modbus_solax_data(const uint8_t &function, const std::vector<uint8_t> &data) = 0;

  void query_status_report(uint8_t address) { this->parent_->query_status_report(address); }
  void query_device_info(uint8_t address) { this->parent_->query_device_info(address); }
  void query_config_settings(uint8_t address) { this->parent_->query_config_settings(address); }
  void discover_devices() { this->parent_->discover_devices(); }

 protected:
  friend ModbusSolax;

  ModbusSolax *parent_;
  uint8_t address_;
  uint8_t *serial_number_;
};

}  // namespace modbus_solax
}  // namespace esphome
