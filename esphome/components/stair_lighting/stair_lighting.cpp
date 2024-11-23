#include "stair_lighting.h"
#include "esphome/components/light/addressable_light_effect.h"

namespace esphome {
namespace stair_lighting {

light::LightEffect *build_effect() {
  auto *effect = new light::AddressableRainbowLightEffect("rainbow");
  effect->set_speed(10);
  effect->set_width(50);
  return effect;
}

void StairLightingComponent::setup() {
  for (auto *step : steps_) {
    auto effect = build_effect();
    effect->init_internal(step);
    step->add_effects({effect});
  }
  for (auto *step : steps_) {
    auto call = step->make_call();
    call.set_effect("rainbow");
  }
}

void StairLightingComponent::update() {}

void StairLightingComponent::update_state() {
  for (auto *step : steps_) {
    auto call = step->make_call();
    call.set_brightness(effect_brightness_);
  }
}

}  // namespace stair_lighting
}  // namespace esphome
