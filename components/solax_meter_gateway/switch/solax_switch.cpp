#include "solax_switch.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace solax_meter_gateway {

static const char *const TAG = "solax_meter_gateway.switch";

void SolaxSwitch::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Solax Switch '%s'...", this->name_.c_str());

  bool initial_state = false;
  switch (this->restore_mode_) {
    case SOLAX_SWITCH_RESTORE_DEFAULT_OFF:
      initial_state = this->get_initial_state().value_or(false);
      break;
    case SOLAX_SWITCH_RESTORE_DEFAULT_ON:
      initial_state = this->get_initial_state().value_or(true);
      break;
    case SOLAX_SWITCH_ALWAYS_OFF:
      initial_state = false;
      break;
    case SOLAX_SWITCH_ALWAYS_ON:
      initial_state = true;
      break;
  }

  if (initial_state) {
    this->turn_on();
  } else {
    this->turn_off();
  }
}
void SolaxSwitch::dump_config() {
  LOG_SWITCH("", "SolaxMeterGateway Switch", this);
  const LogString *restore_mode = LOG_STR("");
  switch (this->restore_mode_) {
    case SOLAX_SWITCH_RESTORE_DEFAULT_OFF:
      restore_mode = LOG_STR("Restore (Defaults to OFF)");
      break;
    case SOLAX_SWITCH_RESTORE_DEFAULT_ON:
      restore_mode = LOG_STR("Restore (Defaults to ON)");
      break;
    case SOLAX_SWITCH_ALWAYS_OFF:
      restore_mode = LOG_STR("Always OFF");
      break;
    case SOLAX_SWITCH_ALWAYS_ON:
      restore_mode = LOG_STR("Always ON");
      break;
  }
  ESP_LOGCONFIG(TAG, "  Restore Mode: %s", LOG_STR_ARG(restore_mode));
}
void SolaxSwitch::write_state(bool state) { this->publish_state(state); }

}  // namespace solax_meter_gateway
}  // namespace esphome
