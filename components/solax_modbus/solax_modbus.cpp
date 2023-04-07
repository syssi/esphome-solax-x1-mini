#include "solax_modbus.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

#define BROADCAST_ADDRESS 0xFF

namespace esphome {
namespace solax_modbus {

static const char *const TAG = "solax_modbus";

void SolaxModbus::setup() {
  if (this->flow_control_pin_ != nullptr) {
    this->flow_control_pin_->setup();
  }
}

void SolaxModbus::loop() {
  const uint32_t now = millis();
  if (now - this->last_solax_modbus_byte_ > 50) {
    this->rx_buffer_.clear();
    this->last_solax_modbus_byte_ = now;
  }

  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);
    if (this->parse_solax_modbus_byte_(byte)) {
      this->last_solax_modbus_byte_ = now;
    } else {
      this->rx_buffer_.clear();
    }
  }
}

std::string hexencode_plain(const uint8_t *data, uint32_t len) {
  char buf[20];
  std::string res;
  for (size_t i = 0; i < len; i++) {
    sprintf(buf, "%02X", data[i]);
    res += buf;
  }
  return res;
}

uint16_t chksum(const uint8_t data[], const uint8_t len) {
  uint8_t i;
  uint16_t checksum = 0;
  for (i = 0; i <= len; i++) {
    checksum = checksum + data[i];
  }
  return checksum;
}

bool SolaxModbus::parse_solax_modbus_byte_(uint8_t byte) {
  size_t at = this->rx_buffer_.size();
  this->rx_buffer_.push_back(byte);
  const uint8_t *frame = &this->rx_buffer_[0];

  // Byte 0: modbus address (match all)
  if (at == 0)
    return true;

  // Byte 1: Function (msb indicates error)
  if (at == 1)
    return (byte & 0x80) != 0x80;

  if (at == 2)
    return true;

  // Byte 3: solax device address
  if (at == 3)
    return true;
  uint8_t address = frame[3];

  // Byte 9: data length
  if (at < 9)
    return true;

  uint8_t data_len = frame[8];
  // Byte 9...9+data_len-1: Data
  if (at < 9 + data_len)
    return true;

  // Byte 9+data_len: CRC_LO (over all bytes)
  if (at == 9 + data_len)
    return true;

  ESP_LOGVV(TAG, "RX <- %s", format_hex_pretty(frame, at + 1).c_str());

  if (frame[0] != 0xAA || frame[1] != 0x55) {
    ESP_LOGW(TAG, "Invalid header");
    return false;
  }

  // Byte 9+data_len+1: CRC_HI (over all bytes)
  uint16_t computed_checksum = chksum(frame, 9 + data_len - 1);
  uint16_t remote_checksum = uint16_t(frame[9 + data_len + 1]) | (uint16_t(frame[9 + data_len]) << 8);
  if (computed_checksum != remote_checksum) {
    ESP_LOGW(TAG, "Invalid checksum! 0x%02X !=  0x%02X", computed_checksum, remote_checksum);
    return false;
  }

  // data only
  std::vector<uint8_t> data(this->rx_buffer_.begin() + 9, this->rx_buffer_.begin() + 9 + data_len);

  if (address == BROADCAST_ADDRESS) {
    // check control code && function code
    if (frame[6] == 0x10 && frame[7] == 0x80 && data.size() == 14) {
      ESP_LOGI(TAG, "Inverter discovered. Serial number: %s", hexencode_plain(&data.front(), data.size()).c_str());
      this->register_address(data.data(), 0x0A);
    } else {
      ESP_LOGW(TAG, "Unknown broadcast data: %s", format_hex_pretty(&data.front(), data.size()).c_str());
    }

    // early return false to reset buffer
    return false;
  }

  bool found = false;
  for (auto *device : this->devices_) {
    if (device->address_ == address) {
      if (frame[6] == 0x11) {
        device->on_solax_modbus_data(frame[7], data);
      } else {
        ESP_LOGW(TAG, "Unhandled control code (%d) of frame for address 0x%02X: %s", frame[6], address,
                 format_hex_pretty(frame, at + 1).c_str());
      }
      found = true;
    }
  }

  if (!found) {
    ESP_LOGW(TAG, "Got solax frame from unknown device address 0x%02X!", address);
  }

  // return false to reset buffer
  return false;
}

void SolaxModbus::dump_config() {
  ESP_LOGCONFIG(TAG, "SolaxModbus:");
  LOG_PIN("  Flow Control Pin: ", this->flow_control_pin_);

  this->check_uart_settings(9600);
}

float SolaxModbus::get_setup_priority() const {
  // After UART bus
  return setup_priority::BUS - 1.0f;
}

void SolaxModbus::query_status_report(uint8_t address) {
  static SolaxMessageT tx_message;

  tx_message.Source[0] = 0x01;
  tx_message.Source[1] = 0x00;
  tx_message.Destination[0] = 0x00;
  tx_message.Destination[1] = address;
  tx_message.ControlCode = 0x11;
  tx_message.FunctionCode = 0x02;
  tx_message.DataLength = 0x00;

  this->send(&tx_message);
}

void SolaxModbus::query_device_info(uint8_t address) {
  static SolaxMessageT tx_message;

  tx_message.Source[0] = 0x01;
  tx_message.Source[1] = 0x00;
  tx_message.Destination[0] = 0x00;
  tx_message.Destination[1] = address;
  tx_message.ControlCode = 0x11;
  tx_message.FunctionCode = 0x03;
  tx_message.DataLength = 0x00;

  this->send(&tx_message);
}

void SolaxModbus::query_config_settings(uint8_t address) {
  static SolaxMessageT tx_message;

  tx_message.Source[0] = 0x01;
  tx_message.Source[1] = 0x00;
  tx_message.Destination[0] = 0x00;
  tx_message.Destination[1] = address;
  tx_message.ControlCode = 0x11;
  tx_message.FunctionCode = 0x04;
  tx_message.DataLength = 0x00;

  this->send(&tx_message);
}

void SolaxModbus::register_address(uint8_t serial_number[14], uint8_t address) {
  static SolaxMessageT tx_message;

  tx_message.Source[0] = 0x00;
  tx_message.Source[1] = 0x00;
  tx_message.Destination[0] = 0x00;
  tx_message.Destination[1] = 0x00;
  tx_message.ControlCode = 0x10;
  tx_message.FunctionCode = 0x01;
  tx_message.DataLength = 0x0F;
  memcpy(tx_message.Data, serial_number, 14);
  tx_message.Data[14] = address;

  this->send(&tx_message);
}

void SolaxModbus::discover_devices() {
  static SolaxMessageT tx_message;

  tx_message.Source[0] = 0x01;
  tx_message.Source[1] = 0x00;
  tx_message.Destination[0] = 0x00;
  tx_message.Destination[1] = 0x00;
  tx_message.ControlCode = 0x10;
  tx_message.FunctionCode = 0x00;
  tx_message.DataLength = 0x00;

  this->send(&tx_message);
}

void SolaxModbus::send(SolaxMessageT *tx_message) {
  uint8_t msg_len;

  tx_message->Header[0] = 0xAA;
  tx_message->Header[1] = 0x55;

  msg_len = tx_message->DataLength + 9;
  auto checksum = chksum((const uint8_t *) tx_message, msg_len - 1);

  tx_message->Data[tx_message->DataLength + 0] = checksum >> 8;
  tx_message->Data[tx_message->DataLength + 1] = checksum >> 0;
  msg_len += 2;

  ESP_LOGVV(TAG, "TX -> %s", format_hex_pretty((const uint8_t *) tx_message, msg_len).c_str());

  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(true);

  this->write_array((const uint8_t *) tx_message, msg_len);
  this->flush();

  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(false);
}

}  // namespace solax_modbus
}  // namespace esphome
