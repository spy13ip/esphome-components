#include "stair_lighting.h"

namespace esphome {
namespace stair_lighting {

void StairLightingComponent::setup() {
  auto *effect = new RainbowStairLightingEffect();
  effect->init(steps_);
  set_effect(effect);
}

void StairLightingComponent::update() { effect_->apply(); }

}  // namespace stair_lighting
}  // namespace esphome
