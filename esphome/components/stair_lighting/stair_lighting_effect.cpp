#include "stair_lighting_effect.h"
#include "stair_lighting.h"

static const char *const TAG = "stair_lighting_effect";

namespace esphome {
namespace stair_lighting {

void StairLightingEffect::run_action(ActionCategory category, ActionOperation operation) {
  action_.reset(millis(), category, operation);
}

void StairLightingEffect::init() {
  auto *light = static_cast<AddressableLight *>(state_->get_output());
  steps_.reserve(parent_->get_steps_config().size());
  for (auto step_config : parent_->get_steps_config()) {
    steps_.push_back(new Step(light, step_config.offset, step_config.size, step_config.reversed));
  }
}
void StairLightingEffect::start() {
  LightEffect::start();
  parent_->add_effect(this);
}
void StairLightingEffect::stop() {
  parent_->remove_effect(this);
  AddressableLightEffect::stop();
}

void StairLightingEffect::apply(AddressableLight &it, const Color &current_color) {
  const uint32_t time = millis();

  float full_progress = (float) (time - action_.get_start_time()) / (float) next_step_interval_;
  int i = 0;
  for (auto *step : steps_) {
    float step_progress = calculate_step_progress(action_.get_category(), full_progress, i);
    float inverted_step_progress = calculate_step_progress(FULL, full_progress, i);
    auto &data = step->effect_data();
    float new_data;
    switch (action_.get_operation()) {
      case ON:
        new_data = max(min(data.value, 1 - inverted_step_progress), step_progress);
        break;
      case OFF:
        new_data = min(max(data.value, inverted_step_progress), 1 - step_progress);
        break;
    }
    data.operation = new_data >= data.value ? ON : OFF;
    data.value = new_data;
    i++;
  }
  if (show(current_color, parent_->get_effect_brightness(), parent_->get_night_brightness())) {
    it.schedule_show();
  }
}

float StairLightingEffect::calculate_step_progress(ActionCategory category, float full_progress, int32_t index) {
  switch (category) {
    case UP:
      full_progress -= (float) index;
      break;
    case DOWN:
      full_progress -= (float) (steps_.size() - index - 1);
      break;
    default:
      break;
  }
  full_progress = full_progress * ((float) next_step_interval_ / (float) progress_step_interval_);
  full_progress = clamp(full_progress, 0.0f, 1.0f);
  return full_progress;
}

}  // namespace stair_lighting
}  // namespace esphome
