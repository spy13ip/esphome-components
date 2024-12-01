#include "stair_lighting.h"
#include "stair_lighting_effect.h"

namespace esphome {
namespace stair_lighting {

void StairLightingComponent::add_effect(StairLightingEffect *effect) {
  if (effects_.count(effect)) {
    return;
  }
  effects_.insert(effect);
}

void StairLightingComponent::remove_effect(StairLightingEffect *effect) {
  if (!effects_.count(effect)) {
    return;
  }
  effects_.erase(effect);
}
}  // namespace stair_lighting
}  // namespace esphome