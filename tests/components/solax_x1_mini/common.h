#pragma once
#include "esphome/components/solax_x1_mini/solax_x1_mini.h"

namespace esphome::solax_x1_mini::testing {

class TestableSolaxX1Mini : public SolaxX1Mini {
 public:
  void update() override {}
};

}  // namespace esphome::solax_x1_mini::testing
