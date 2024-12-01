#pragma once

#include <vector>

#include "esphome/core/component.h"
#include "stair_lighting_effect.h"

using std::set;

namespace esphome {
namespace stair_lighting {

class StairLightingComponent : public Component {
 public:
  void add_effect(StairLightingEffect *effect);
  void remove_effect(StairLightingEffect *effect);

 protected:
  set<StairLightingEffect *> effects_;
};

}  // namespace stair_lighting
}  // namespace esphome