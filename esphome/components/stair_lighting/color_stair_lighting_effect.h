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
  bool apply(StairLightingStep &step, const Color &current_color) override {
    float result = max(step.effect_data().get(), step.night_data().get());
    Color result_color = current_color * (uint8_t) (255.0f * result);
    for (int i = 0; i < step.size(); i++) {
      step[i] = result_color;
    }
    return true;
  }
};

}  // namespace stair_lighting
}  // namespace esphome