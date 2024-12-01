#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/core/color.h"
#include "esphome/components/light/addressable_light.h"

#include <vector>

using std::vector;

namespace esphome {
namespace stair_lighting {

class StairLightingComponent : public PollingComponent {
 public:
  void add_steps(const vector<light::LightState *> &steps) { steps_ = steps; }
  void set_effect_brightness(float effect_brightness) { effect_brightness_ = effect_brightness; }
  void set_night_brightness(float night_brightness) { night_brightness_ = night_brightness; }

  void turn_on();
  void turn_off();
  void turn_up();
  void turn_down();

  void setup() override;
  void update() override;

 protected:
  vector<light::LightState *> steps_;

  float effect_brightness_ = 0;
  float night_brightness_ = 0;

  void update_state();
};

template<typename... Ts> class StairLightingControlAction : public Action<Ts...> {
 public:
  explicit StairLightingControlAction(StairLightingComponent *stair_lighting) : stair_lighting_(stair_lighting) {}

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

template<typename... Ts> class StairLightingTurnOnAction : public Action<Ts...> {
 public:
  explicit StairLightingTurnOnAction(StairLightingComponent *stair_lighting) : stair_lighting_(stair_lighting) {}

  void play(Ts... x) override { stair_lighting_->turn_on(); }

 protected:
  StairLightingComponent *stair_lighting_;
};

template<typename... Ts> class StairLightingTurnOffAction : public Action<Ts...> {
 public:
  explicit StairLightingTurnOffAction(StairLightingComponent *stair_lighting) : stair_lighting_(stair_lighting) {}

  void play(Ts... x) override { stair_lighting_->turn_off(); }

 protected:
  StairLightingComponent *stair_lighting_;
};

template<typename... Ts> class StairLightingTurnUpAction : public Action<Ts...> {
 public:
  explicit StairLightingTurnUpAction(StairLightingComponent *stair_lighting) : stair_lighting_(stair_lighting) {}

  void play(Ts... x) override { stair_lighting_->turn_up(); }

 protected:
  StairLightingComponent *stair_lighting_;
};

template<typename... Ts> class StairLightingTurnDownAction : public Action<Ts...> {
 public:
  explicit StairLightingTurnDownAction(StairLightingComponent *stair_lighting) : stair_lighting_(stair_lighting) {}

  void play(Ts... x) override { stair_lighting_->turn_down(); }

 protected:
  StairLightingComponent *stair_lighting_;
};

}  // namespace stair_lighting_old
}  // namespace esphome
