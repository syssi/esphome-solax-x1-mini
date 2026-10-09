#include "solax_number.h"
#include "esphome/core/log.h"

// Fallback for ESPHome < 2026.10.0
#ifndef ESPHOME_LOG_TAG
#define ESPHOME_LOG_TAG(name, tag) static const char *const name = tag
#endif

namespace esphome::solax_meter_gateway {

ESPHOME_LOG_TAG(TAG, "solax_meter_gateway.number");

void SolaxNumber::setup() {
  float value;
  if (!this->restore_value_) {
    value = this->initial_value_;
  } else {
    this->pref_ = global_preferences->make_preference<float>(this->get_object_id_hash());
    if (!this->pref_.load(&value)) {
      if (!std::isnan(this->initial_value_)) {
        value = this->initial_value_;
      } else {
        value = this->traits.get_min_value();
      }
    }
  }

  this->publish_state(value);
}

void SolaxNumber::control(float value) {
  this->publish_state(value);

  if (this->restore_value_)
    this->pref_.save(&value);
}
void SolaxNumber::dump_config() { LOG_NUMBER("", "SolaxMeterGateway Number", this); }

}  // namespace esphome::solax_meter_gateway
