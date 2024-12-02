#include "stair_lighting_effect.h"
#include "stair_lighting.h"

#include "esphome/core/log.h"

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
    if (action_.get_operation() == ON) {
      float min_data = min(data.value, 1 - inverted_step_progress);
      new_data = max(min_data, step_progress);
    } else if (action_.get_operation() == OFF) {
      float max_data = max(data.value, inverted_step_progress);
      new_data = min(max_data, 1 - step_progress);
    } else {
      new_data = NAN;
    }
    data.operation = new_data >= data.value ? ON : OFF;
    data.value = new_data;
    i++;
  }
  const uint32_t calculate_time = millis() - time;

  apply(steps_, current_color);
  const uint32_t effect_time = millis() - time - calculate_time;
  it.schedule_show();
  ESP_LOGD(TAG, "calculate %dms, effect %dms", calculate_time, effect_time);
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
