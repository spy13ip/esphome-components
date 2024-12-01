#pragma once

#include <vector>
#include <set>

#include "esphome/core/component.h"

using std::vector;
using std::set;

namespace esphome {
namespace stair_lighting {

class StairLightingEffect;

struct StairLightingStepConfig {
  int32_t offset = 0;
  int32_t size = 0;
  bool reversed = false;
};

class StairLightingComponent : public Component {
 public:
  void set_steps_config(const vector<StairLightingStepConfig> &steps_config) { steps_config_ = steps_config; }

  vector<StairLightingStepConfig> &get_steps_config() { return steps_config_; }

  void add_effect(StairLightingEffect *effect);
  void remove_effect(StairLightingEffect *effect);

 protected:
  vector<StairLightingStepConfig> steps_config_;
  set<StairLightingEffect *> effects_;
};

}  // namespace stair_lighting
}  // namespace esphome