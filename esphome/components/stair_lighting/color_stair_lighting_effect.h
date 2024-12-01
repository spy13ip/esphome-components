#pragma once

#include <utility>
#include <vector>
#include <algorithm>

#include "stair_lighting_effect.h"

using std::vector;
using namespace esphome::light;

namespace esphome {
namespace stair_lighting {

class ColorStairLightingEffect : public StairLightingEffect {
 public:
  explicit ColorStairLightingEffect(const std::string &name) : StairLightingEffect(name) {}

 protected:
  bool apply(Step &step, const Color &current_color) override {
    float effect = calculate_brightness(step.effect_data());
    float night = calculate_brightness(step.night_data());
    float result = max(effect, night);
    Color result_color = current_color * (uint8_t) (255.0f * result);
    for (int i = 0; i < step.size(); i++) {
      step[i] = result_color;
    }
    return true;
  }

  static float calculate_brightness(const ProgressData &data) {
    const auto &operation = data.last_operation();
    const float &increase = data.get(INCREASE);
    const float &decrease = data.get(DECREASE);
    if (isnan(increase) && isnan(decrease))
      return 0;
    switch (operation) {
      case INCREASE:
        return isnan(decrease) ? increase : max(increase, 1.0f - decrease);
      case DECREASE:
        return isnan(increase) ? 1.0f - decrease : min(increase, 1.0f - decrease);
    }
  }
};

}  // namespace stair_lighting
}  // namespace esphome