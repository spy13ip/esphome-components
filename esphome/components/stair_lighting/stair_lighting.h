#pragma once

#include "esphome/core/component.h"
#include "esphome/core/color.h"
// #include "esphome/components/sensor/sensor.h"
#include "stair_lighting_step.h"
#include "stair_lighting_effect.h"

#include <vector>

using std::vector;

namespace esphome {
namespace stair_lighting {

class StairLightingComponent : public PollingComponent {
 public:
  void add_steps(const vector<StairLightingStep *> &steps) { steps_ = steps; }
  // void set_lower_sensor(sensor::Sensor *lower_sensor) { lower_sensor_ = lower_sensor; }
  // void set_upper_sensor(sensor::Sensor *upper_sensor) { upper_sensor_ = upper_sensor; }
  void set_effect(StairLightingEffect *effect) { effect_ = effect; }
  void set_night_brightness(float night_brightness) { night_brightness_ = night_brightness; }
  void set_effect_brightness(float effect_brightness) { effect_brightness_ = effect_brightness; }

  void setup() override;
  void update() override;

 protected:
  vector<StairLightingStep *> steps_;
  // sensor::Sensor *upper_sensor_ = nullptr;
  // sensor::Sensor *lower_sensor_ = nullptr;
  StairLightingEffect *effect_ = nullptr;

  float night_brightness_ = 0;
  float effect_brightness_ = 0;
};

}  // namespace stair_lighting
}  // namespace esphome
