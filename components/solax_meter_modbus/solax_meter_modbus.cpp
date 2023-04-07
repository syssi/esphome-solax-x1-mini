#include "solax_meter_modbus.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace solax_meter_modbus {

static const char *const TAG = "solax_meter_modbus";

void SolaxMeterModbus::setup() {
  if (this->flow_control_pin_ != nullptr) {
    this->flow_control_pin_->setup();
  }
}
void SolaxMeterModbus::loop() {
  const uint32_t now = millis();
  if (now - this->last_solax_meter_modbus_byte_ > 50) {
    this->rx_buffer_.clear();
    this->last_solax_meter_modbus_byte_ = now;
  }

  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);
    if (this->parse_solax_meter_modbus_byte_(byte)) {
      this->last_solax_meter_modbus_byte_ = now;
    } else {
      this->rx_buffer_.clear();
    }
  }
}

bool SolaxMeterModbus::parse_solax_meter_modbus_byte_(uint8_t byte) {
  size_t at = this->rx_buffer_.size();
  this->rx_buffer_.push_back(byte);
  const uint8_t *raw = &this->rx_buffer_[0];

  // Meter requests of a Solax X1 mini 600W
  //
  // Handshake request:        0x01 0x03 0x00 0x0B 0x00 0x01 0xF5 0xC8
  // Read power request:       0x01 0x04 0x00 0x0C 0x00 0x02 0xB1 0xC8
  // Read total energy import: 0x01 0x04 0x00 0x48 0x00 0x02 0xF1 0xDD
  // Read total energy export: 0x01 0x04 0x00 0x4A 0x00 0x02 0x50 0x1D
  //                           addr func      reg       len  crc  crc

  // Meter requests of a Solax X1 mini from @strage
  //
  // Handshake request:        0x01 0x03 0x00 0x0B 0x00 0x01 0xF5 0xC8
  // Read power request:       0x01 0x03 0x00 0x0E 0x00 0x01 0xE5 0xC9
  // Read total energy:        0x01 0x03 0x00 0x08 0x00 0x04 0xC5 0xCB
  //                           addr func      reg       len  crc  crc

  if (at == 0)
    return true;
  uint8_t address = raw[0];

  if (at == 2)
    return true;

  uint8_t data_len = 5;
  uint8_t data_offset = 1;

  if (at < data_offset + data_len)
    return true;

  if (at == data_offset + data_len)
    return true;

  ESP_LOGVV(TAG, "RX <- %s", format_hex_pretty(raw, at + 1).c_str());

  uint16_t computed_crc = crc16(raw, data_offset + data_len);
  uint16_t remote_crc = uint16_t(raw[data_offset + data_len]) | (uint16_t(raw[data_offset + data_len + 1]) << 8);
  if (computed_crc != remote_crc) {
    ESP_LOGW(TAG, "CRC check failed! 0x%04X != 0x%04X", computed_crc, remote_crc);
    return false;
  }

  std::vector<uint8_t> data(this->rx_buffer_.begin() + data_offset, this->rx_buffer_.begin() + data_offset + data_len);
  bool found = false;
  for (auto *device : this->devices_) {
    if (device->address_ == address) {
      device->on_solax_meter_modbus_data(data);
      found = true;
    }
  }

  if (!found) {
    ESP_LOGW(TAG, "Got SolaxMeterModbus frame from unknown address 0x%02X! ", address);
  }

  // return false to reset buffer
  return false;
}

void SolaxMeterModbus::dump_config() {
  ESP_LOGCONFIG(TAG, "SolaxMeterModbus:");
  LOG_PIN("  Flow Control Pin: ", this->flow_control_pin_);

  this->check_uart_settings(9600);
}

float SolaxMeterModbus::get_setup_priority() const {
  // After UART bus
  return setup_priority::BUS - 1.0f;
}

void SolaxMeterModbus::send(uint8_t address, int16_t power) {
  std::vector<uint8_t> data;
  data.push_back(address);
  data.push_back(0x03);
  data.push_back(0x02);
  data.push_back(power >> 8);
  data.push_back(power >> 0);
  auto crc = crc16(data.data(), data.size());
  data.push_back(crc >> 0);
  data.push_back(crc >> 8);

  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(true);

  this->write_array(data);
  this->flush();

  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(false);

  ESP_LOGV(TAG, "SolaxMeterModbus write: %s", format_hex_pretty(data).c_str());
}

void SolaxMeterModbus::send(uint8_t address, float power) {
  union float_bytes_t {
    float f;
    uint8_t i[4];
  };

  float_bytes_t payload;
  payload.f = power;

  std::vector<uint8_t> data;
  data.push_back(address);
  data.push_back(0x04);
  data.push_back(0x04);
  data.push_back(payload.i[3]);
  data.push_back(payload.i[2]);
  data.push_back(payload.i[1]);
  data.push_back(payload.i[0]);
  auto crc = crc16(data.data(), data.size());
  data.push_back(crc >> 0);
  data.push_back(crc >> 8);

  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(true);

  this->write_array(data);
  this->flush();

  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(false);

  ESP_LOGV(TAG, "SolaxMeterModbus write: %s", format_hex_pretty(data).c_str());
}

// Helper function for lambdas
// Send raw command. Except CRC everything must be contained in payload
void SolaxMeterModbus::send_raw(const std::vector<uint8_t> &payload) {
  if (payload.empty()) {
    return;
  }

  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(true);

  auto crc = crc16(payload.data(), payload.size());
  this->write_array(payload);
  this->write_byte(crc & 0xFF);
  this->write_byte((crc >> 8) & 0xFF);
  this->flush();
  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(false);

  ESP_LOGV(TAG, "SolaxMeterModbus write raw: %s", format_hex_pretty(payload).c_str());
}

}  // namespace solax_meter_modbus
}  // namespace esphome
