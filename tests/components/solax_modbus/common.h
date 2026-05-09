#pragma once
#include <cstdint>
#include <vector>
#include "esphome/components/solax_modbus/solax_modbus.h"

namespace esphome::solax_modbus::testing {

// Reimplements solax_modbus.cpp static chksum (sums bytes 0..len inclusive)
static uint16_t solax_chksum(const uint8_t *data, uint8_t len) {
  uint16_t s = 0;
  for (uint8_t i = 0; i <= len; i++)
    s += data[i];
  return s;
}

// Frame: [0xAA,0x55,src0,src1(=address),dst0,dst1,cc,fc,data_len,data...,crc_hi,crc_lo]
static std::vector<uint8_t> make_solax_frame(uint8_t address, uint8_t cc, uint8_t fc,
                                             const std::vector<uint8_t> &data) {
  std::vector<uint8_t> frame = {0xAA, 0x55, 0x00, address, 0x01, 0x00, cc, fc, static_cast<uint8_t>(data.size())};
  frame.insert(frame.end(), data.begin(), data.end());
  // chksum over frame[0..8+data_len-1]: length arg = 9 + data.size() - 1
  uint16_t crc = solax_chksum(frame.data(), static_cast<uint8_t>(9 + data.size() - 1));
  frame.push_back(crc >> 8);
  frame.push_back(crc & 0xFF);
  return frame;
}

// Status response from address=0x0A, control_code=0x11, function=0x02, no data
static const std::vector<uint8_t> STATUS_FRAME = make_solax_frame(0x0A, 0x11, 0x02, {});

// Status response from address=0x01
static const std::vector<uint8_t> STATUS_FRAME_ADDR01 = make_solax_frame(0x01, 0x11, 0x02, {});

// Frame with non-dispatch control code 0x10 from address=0x0A
static const std::vector<uint8_t> WRONG_CC_FRAME = make_solax_frame(0x0A, 0x10, 0x02, {});

class MockSolaxModbusDevice : public SolaxModbusDevice {
 public:
  uint8_t last_function{0};
  std::vector<uint8_t> received_data;
  int call_count{0};

  void on_solax_modbus_data(const uint8_t &function, const std::vector<uint8_t> &data) override {
    last_function = function;
    received_data = data;
    call_count++;
  }
};

class TestableSolaxModbus : public SolaxModbus {
 public:
  void loop() override {}
  using SolaxModbus::parse_solax_modbus_byte_;

  bool feed(const std::vector<uint8_t> &frame) {
    bool result = false;
    for (uint8_t byte : frame) {
      result = parse_solax_modbus_byte_(byte);
      if (!result)
        this->rx_buffer_.clear();
    }
    return result;
  }
};

}  // namespace esphome::solax_modbus::testing
