#pragma once

#include "../solax_meter_gateway.h"
#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace solax_meter_gateway {

enum SolaxSwitchRestoreMode {
  SOLAX_SWITCH_RESTORE_DEFAULT_OFF,
  SOLAX_SWITCH_RESTORE_DEFAULT_ON,
  SOLAX_SWITCH_ALWAYS_OFF,
  SOLAX_SWITCH_ALWAYS_ON,
};

class SolaxMeterGateway;

class SolaxSwitch : public switch_::Switch, public Component {
 public:
  void set_parent(SolaxMeterGateway *parent) { this->parent_ = parent; };
  void set_restore_mode(SolaxSwitchRestoreMode restore_mode) { this->restore_mode_ = restore_mode; }

  void setup() override;
  void dump_config() override;

 protected:
  void write_state(bool state) override;
  SolaxMeterGateway *parent_;
  SolaxSwitchRestoreMode restore_mode_{SOLAX_SWITCH_RESTORE_DEFAULT_OFF};
};

}  // namespace solax_meter_gateway
}  // namespace esphome
