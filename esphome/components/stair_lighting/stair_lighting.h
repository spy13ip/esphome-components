#pragma once

#include <set>

#include "esphome/core/component.h"

using std::set;

namespace esphome {
namespace stair_lighting {

class StairLightingEffect;

class StairLightingComponent : public Component {
 public:
  void add_effect(StairLightingEffect *effect);
  void remove_effect(StairLightingEffect *effect);

 protected:
  set<StairLightingEffect *> effects_;
};

}  // namespace stair_lighting
}  // namespace esphome