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
  void apply(const vector<Step *> &steps, const Color &current_color) override {
    for (auto *step : steps) {
      Color result_color = current_color * (uint8_t) (255.0f * step->effect_data().value);
      for (int i = 0; i < step->size(); i++) {
        (*step)[i] = result_color;
      }
    }
  }
};

}  // namespace stair_lighting
}  // namespace esphome
