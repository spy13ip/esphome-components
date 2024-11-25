#include "stair_lighting.h"
#include "esphome/core/log.h"

namespace esphome {
namespace stair_lighting {

static const char *const TAG = "stair_lighting";

void StairLightingComponent::setup() {}

void StairLightingComponent::update() {}

void StairLightingComponent::turn_on() {
  ESP_LOGD(TAG, "Effect brightness - %f", this->effect_brightness_);
  for (auto *step : steps_) {
    ESP_LOGD(TAG, "Step processing - %s", step->get_name().c_str());
    step->make_call().set_effect("action").set_brightness(effect_brightness_).perform();
  }
}

void StairLightingComponent::turn_off() {
  for (auto *step : steps_) {

    step->make_call().set_effect("night").set_brightness(night_brightness_).perform();
  }
}

void StairLightingComponent::turn_up() {}

void StairLightingComponent::turn_down() {}

}  // namespace stair_lighting
}  // namespace esphome
