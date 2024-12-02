#include "stair_lighting.h"
#include "stair_lighting_effect.h"

namespace esphome {
namespace stair_lighting {

void StairLightingComponent::add_effect(StairLightingEffect *effect) {
  if (effects_.count(effect)) {
    return;
  }
  effects_.insert(effect);
}

void StairLightingComponent::remove_effect(StairLightingEffect *effect) {
  if (!effects_.count(effect)) {
    return;
  }
  effects_.erase(effect);
}

void StairLightingComponent::setup() {}

void StairLightingComponent::loop() {
  uint32_t time = millis();
  uint32_t timeout = time - state_.start_time;
  bool lower_state = lower_sensor_->state;
  bool upper_state = upper_sensor_->state;
  bool lower_trigger_on = !state_.last_lower_state && lower_state;
  bool upper_trigger_on = !state_.last_upper_state && upper_state;
  state_.last_lower_state = lower_state;
  state_.last_upper_state = upper_state;

  if (state_.operation == OFF) {
    if (lower_trigger_on) {
      run_action(time, ON, UP);
    } else if (upper_trigger_on) {
      run_action(time, ON, DOWN);
    }
  } else if (state_.operation == ON) {
    if (timeout > off_timeout_) {
      run_action(time, OFF, state_.category);
    } else if (state_.category == DOWN && lower_trigger_on) {
      if (timeout < passage_timeout_) {
        run_action(time, ON, FULL);
      } else {
        run_action(time, OFF, DOWN);
      }
    } else if (state_.category == UP && upper_trigger_on) {
      if (timeout < passage_timeout_) {
        run_action(time, ON, FULL);
      } else {
        run_action(time, OFF, UP);
      }
    } else if (lower_state && (state_.category == FULL || state_.category == UP)) {
      reset_start_time(time);
    } else if (upper_state && (state_.category == FULL || state_.category == DOWN)) {
      reset_start_time(time);
    }
  }
}

void StairLightingComponent::turn_up() { run_action(millis(), ON, UP); }
void StairLightingComponent::turn_down() { run_action(millis(), ON, DOWN); }
void StairLightingComponent::turn_full() { run_action(millis(), ON, FULL); }
void StairLightingComponent::turn_off() { run_action(millis(), OFF, FULL); }

void StairLightingComponent::run_action(uint32_t time, ActionOperation operation, ActionCategory category) {
  state_.start_time = time;
  state_.category = category;
  state_.operation = operation;
  for (auto effect : effects_) {
    effect->run_action(category, operation);
  }
}
void StairLightingComponent::reset_start_time(uint32_t time) { state_.start_time = time; }

}  // namespace stair_lighting
}  // namespace esphome
