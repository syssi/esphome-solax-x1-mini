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
  void send_raw(const std::vector<uint8_t> &payload) override { last_raw_payload_ = payload; }

  void set_power_demand(float value) { this->power_demand_ = value; }
  void set_last_power_demand_received(uint32_t ts) { this->last_power_demand_received_ = ts; }

  uint32_t get_last_solax_request_received() const { return this->last_solax_request_received_; }
  uint32_t get_last_power_demand_received() const { return this->last_power_demand_received_; }
  uint8_t get_consecutive_handshake_count() const { return this->consecutive_handshake_count_; }
  const std::vector<uint8_t> &get_last_raw_payload() const { return last_raw_payload_; }

  void call_update() { SolaxMeterGateway::update(); }

 private:
  std::vector<uint8_t> last_raw_payload_;
};

}  // namespace esphome::solax_meter_gateway::testing
