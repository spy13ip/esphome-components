#include "stair_lighting.h"
#include "esphome/core/log.h"

namespace esphome {
namespace stair_lighting {

static const char *const TAG = "stair_lighting";

void StairLightingComponent::setup() {}

void StairLightingComponent::update() {}

void StairLightingComponent::turn_on() {
  for (auto *step : steps_) {
    step->turn_on().set_effect("action").set_brightness(effect_brightness_).perform();
  }
}

void StairLightingComponent::turn_off() {
  for (auto *step : steps_) {
    step->turn_on().set_effect("night").set_brightness(night_brightness_).perform();
  }
}

void StairLightingComponent::turn_up() {}

void StairLightingComponent::turn_down() {}

}  // namespace stair_lighting
}  // namespace esphome
