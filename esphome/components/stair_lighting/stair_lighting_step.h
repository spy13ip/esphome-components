#pragma once

#include "esphome/core/color.h"
#include "esphome/components/light/addressable_light.h"

#include <vector>

using std::vector;

namespace esphome {
namespace stair_lighting {

class StairLightingStep {
 public:
  void set_light(light::LightState *state) {
    light_state_ = state;
    light_ = static_cast<light::AddressableLight *>(state->get_output());
  }
  void set_start(uint32_t start) { start_ = start; }
  void set_end(uint32_t end) { end_ = end; }

  void init();

  uint32_t size();
  light::ESPColorView operator[](int32_t index) const;
  void schedule_show();

 protected:
  light::LightState *light_state_;
  light::AddressableLight *light_;
  uint32_t start_;
  uint32_t end_;

  bool reverse_;
  uint32_t offset_;
  uint32_t size_;
};

}  // namespace stair_lighting
}  // namespace esphome