#include "esphome/core/automation.h"

#include "stair_lighting.h"

using std::vector;

namespace esphome {
namespace stair_lighting {

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

template<typename... Ts> class StairLightingTurnFullAction : public Action<Ts...> {
 public:
  explicit StairLightingTurnFullAction(StairLightingComponent *stair_lighting) : stair_lighting_(stair_lighting) {}

  void play(Ts... x) override { stair_lighting_->turn_full(); }

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

}  // namespace stair_lighting
}  // namespace esphome
