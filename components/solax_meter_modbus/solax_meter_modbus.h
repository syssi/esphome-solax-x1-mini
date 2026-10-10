#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/components/uart/uart.h"

#include <vector>

namespace esphome::solax_meter_modbus {

static constexpr size_t MAX_HEX_DUMP_BYTES = 32;

/// Stack buffer for hex dumps in log statements. Use the temporary returned by format_hex_dump()
/// directly as log argument: format_hex_dump(data).c_str()
struct HexDump {
  char str[format_hex_pretty_size(MAX_HEX_DUMP_BYTES)];
  const char *c_str() const { return this->str; }
};

inline HexDump format_hex_dump(const uint8_t *data, size_t length) {
  HexDump dump;
  format_hex_pretty_to(dump.str, data, length, '.');
  return dump;
}
inline HexDump format_hex_dump(const std::vector<uint8_t> &data) { return format_hex_dump(data.data(), data.size()); }

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
  virtual void send(int16_t power) { this->parent_->send(this->address_, power); }
  virtual void send(float power) { this->parent_->send(this->address_, power); }
  virtual void send_raw(const std::vector<uint8_t> &payload) { this->parent_->send_raw(payload); }

 protected:
  friend SolaxMeterModbus;

  SolaxMeterModbus *parent_;
  uint8_t address_;
};

}  // namespace esphome::solax_meter_modbus
