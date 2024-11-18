#pragma once

#include "esphome/core/color.h"
#include "stair_lighting_step.h"

#include <vector>

using std::vector;

namespace esphome {
namespace stair_lighting {

class StairLightingEffect {
 public:
  virtual void init(const vector<StairLightingStep *> &steps, float *brightness) {
    steps_ = steps;
    brightness_ = brightness;
  }

  virtual void start();
  virtual void stop();
  virtual void apply();

 protected:
  vector<StairLightingStep *> steps_;
  float *brightness_;
};

class RainbowStairLightingEffect : public StairLightingEffect {
 public:
  void set_speed(uint32_t speed) { this->speed_ = speed; }
  void set_width(uint16_t width) { this->width_ = width; }

  void start() override {}

  void stop() override {}

  void apply() override {
    light::ESPHSVColor hsv;
    hsv.value = 255 * *brightness_;
    hsv.saturation = 240;
    uint16_t hue = (millis() * this->speed_) % 0xFFFF;
    const uint16_t add = 0xFFFF / this->width_;
    for (auto *step : steps_) {
      for (int i = 0; i < step->size(); i++) {
        auto pixel = (*step)[i];
        hsv.hue = hue >> 8;
        pixel = hsv;
        hue += add;
      }
      step->schedule_show();
    }
  }

 protected:
  uint32_t speed_{10};
  uint16_t width_{50};
};

}  // namespace stair_lighting
}  // namespace esphome
