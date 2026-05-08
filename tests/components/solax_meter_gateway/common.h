#pragma once
#include "esphome/components/solax_meter_gateway/solax_meter_gateway.h"
#include "esphome/components/switch/switch.h"

namespace esphome::solax_meter_gateway::testing {

class TestSwitch : public switch_::Switch {
 protected:
  void write_state(bool state) override { this->publish_state(state); }
};

class TestableSolaxMeterGateway : public SolaxMeterGateway {
 public:
  void update() override {}
  void send(int16_t power) override {}
  void send(float power) override {}
  void send_raw(const std::vector<uint8_t> &payload) override {}

  void set_power_demand(float value) { this->power_demand_ = value; }
};

}  // namespace esphome::solax_meter_gateway::testing
