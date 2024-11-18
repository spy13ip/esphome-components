#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
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
  void set_effect_brightness(float effect_brightness) { effect_brightness_ = effect_brightness; }
  void set_night_brightness(float night_brightness) { night_brightness_ = night_brightness; }

  void setup() override;
  void update() override;

 protected:
  vector<StairLightingStep *> steps_;
  // sensor::Sensor *upper_sensor_ = nullptr;
  // sensor::Sensor *lower_sensor_ = nullptr;
  StairLightingEffect *effect_ = nullptr;

  float effect_brightness_ = 0;
  float night_brightness_ = 0;
};

template<typename... Ts> class StairLightingTurnOnAction : public Action<Ts...> {
 public:
  StairLightingTurnOnAction(StairLightingComponent *stair_lighting) : stair_lighting_(stair_lighting) {}

  TEMPLATABLE_VALUE(float, effect_brightness)
  TEMPLATABLE_VALUE(float, night_brightness)

  void play(Ts... x) override {
    optional<float> effect_brightness = this->effect_brightness_.optional_value(x...);
    if (effect_brightness.has_value()) {
      stair_lighting_->set_effect_brightness(*effect_brightness);
    }
    optional<float> night_brightness = this->night_brightness_.optional_value(x...);
    if (night_brightness.has_value()) {
      stair_lighting_->set_night_brightness(*night_brightness);
    }
  }

 protected:
  StairLightingComponent *stair_lighting_;
};

template<typename... Ts> class StairLightingTurnOffAction : public Action<Ts...> {
 public:
  StairLightingTurnOffAction(StairLightingComponent *stair_lighting) : stair_lighting_(stair_lighting) {}

  void play(Ts... x) override {
    stair_lighting_->set_effect_brightness(0);
    stair_lighting_->set_night_brightness(0);
  }

 protected:
  StairLightingComponent *stair_lighting_;
};

}  // namespace stair_lighting
}  // namespace esphome
