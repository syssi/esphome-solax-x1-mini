#pragma once
#include <cstdint>
#include <vector>
#include "esphome/components/solax_meter_modbus/solax_meter_modbus.h"

namespace esphome::solax_meter_modbus::testing {

static uint16_t crc16_modbus(const uint8_t *data, size_t len) {
  uint16_t crc = 0xFFFF;
  for (size_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (int j = 0; j < 8; j++)
      crc = (crc & 1) ? (crc >> 1) ^ 0xA001 : (crc >> 1);
  }
  return crc;
}

// Frame: [address, payload[5], crc_lo, crc_hi]
static std::vector<uint8_t> make_meter_frame(uint8_t address, const std::vector<uint8_t> &payload) {
  std::vector<uint8_t> frame = {address};
  frame.insert(frame.end(), payload.begin(), payload.end());
  uint16_t crc = crc16_modbus(frame.data(), frame.size());
  frame.push_back(crc & 0xFF);
  frame.push_back(crc >> 8);
  return frame;
}

// Real handshake request from inverter (address=0x01): 0x01 0x03 0x00 0x0B 0x00 0x01 0xF5 0xC8
static const std::vector<uint8_t> HANDSHAKE_FRAME = {0x01, 0x03, 0x00, 0x0B, 0x00, 0x01, 0xF5, 0xC8};

// Real read-power request (address=0x01): 0x01 0x04 0x00 0x0C 0x00 0x02 0xB1 0xC8
static const std::vector<uint8_t> READ_POWER_FRAME = {0x01, 0x04, 0x00, 0x0C, 0x00, 0x02, 0xB1, 0xC8};

// Same payload as handshake but for address=0x02
static const std::vector<uint8_t> HANDSHAKE_FRAME_ADDR02 = make_meter_frame(0x02, {0x03, 0x00, 0x0B, 0x00, 0x01});

class MockSolaxMeterModbusDevice : public SolaxMeterModbusDevice {
 public:
  std::vector<uint8_t> received_data;
  int call_count{0};

  void on_solax_meter_modbus_data(const std::vector<uint8_t> &data) override {
    received_data = data;
    call_count++;
  }
  void send(int16_t) override {}
  void send(float) override {}
  void send_raw(const std::vector<uint8_t> &) override {}
};

class TestableSolaxMeterModbus : public SolaxMeterModbus {
 public:
  void loop() override {}
  using SolaxMeterModbus::parse_solax_meter_modbus_byte_;

  bool feed(const std::vector<uint8_t> &frame) {
    bool result = false;
    for (uint8_t byte : frame) {
      result = parse_solax_meter_modbus_byte_(byte);
      if (!result)
        this->rx_buffer_.clear();
    }
    return result;
  }
};

}  // namespace esphome::solax_meter_modbus::testing
