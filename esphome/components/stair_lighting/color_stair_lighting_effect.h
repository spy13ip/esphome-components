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
  bool show(const Color &current_color, float brightness, float night_brightness) override {
    bool changed = false;
    auto first_step = steps_.begin();
    auto last_step = steps_.end() - 1;
    for (auto it = steps_.begin(); it != steps_.end(); it++) {
      auto &step = (**it);
      float effect_data = step.effect_data().value * brightness;
      float night_data = (it == first_step || it == last_step) ? night_brightness : 0.0f;
      float result_data = std::max(effect_data, night_data);
      Color result_color = current_color * (uint8_t) (255.0f * result_data);
      for (int i = 0; i < step.size(); i++) {
        if (step[i].get() != result_color) {
          step[i].set(result_color);
          changed = true;
        }
      }
    }
    return changed;
  }
};

}  // namespace stair_lighting
}  // namespace esphome
