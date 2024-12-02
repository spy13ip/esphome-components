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
  Color last_color_ = Color::BLACK;

  bool show(const Color &current_color, float brightness, float night_brightness) override {
    bool changed = false;
    if (last_color_ != current_color) {
      changed = true;
    }
    last_color_ = current_color;

    auto first_step = steps_.begin();
    auto last_step = steps_.end() - 1;
    for (auto it = steps_.begin(); it != steps_.end(); it++) {
      auto &step = (**it);
      float effect_data_f = step.effect_data().value * brightness;
      if (it == first_step || it == last_step) {
        effect_data_f = std::max(effect_data_f, night_brightness);
      }
      uint8_t effect_data = (uint8_t) (255.0f * effect_data_f);
      Color result_color = current_color * effect_data;
      for (int i = 0; i < step.size(); i++) {
        if (effect_data != step[i].get_effect_data()) {
          step[i].set_effect_data(effect_data);
          changed = true;
        }
        step[i].set(result_color);
      }
    }
    return changed;
  }
};

}  // namespace stair_lighting
}  // namespace esphome
