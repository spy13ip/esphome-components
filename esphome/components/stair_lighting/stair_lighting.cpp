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

void StairLightingComponent::setup() {}

void StairLightingComponent::loop() {}

void StairLightingComponent::turn_up() {
  for (auto effect : effects_) {
    effect->run_effect_action(ActionCategory::UP, ActionOperation::INCREASE);
  }
}
void StairLightingComponent::turn_down() {
  for (auto effect : effects_) {
    effect->run_effect_action(ActionCategory::DOWN, ActionOperation::INCREASE);
  }
}
void StairLightingComponent::turn_full() {
  for (auto effect : effects_) {
    effect->run_effect_action(ActionCategory::FULL, ActionOperation::INCREASE);
  }
}
void StairLightingComponent::turn_off() {
  for (auto effect : effects_) {
    effect->run_effect_action(ActionCategory::FULL, ActionOperation::DECREASE);
  }
}

}  // namespace stair_lighting
}  // namespace esphome