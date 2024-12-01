#include "stair_lighting_effect.h"
#include "stair_lighting.h"

namespace esphome {
namespace stair_lighting {

void StairLightingEffect::run_effect_action(ActionCategory category, ActionOperation operation) {
  effect_action_.reset(millis(), category, operation);
}
void StairLightingEffect::run_night_action(ActionCategory category, ActionOperation operation) {
  night_action_.reset(millis(), category, operation);
}

void StairLightingEffect::init() {
  auto *light = static_cast<AddressableLight *>(state_->get_output());
  steps_.reserve(parent_->get_steps_config().size());
  for (auto step_config : parent_->get_steps_config()) {
    steps_.push_back(new StairLightingStep(light, step_config.offset, step_config.size, step_config.reversed));
  }
}
void StairLightingEffect::start() { parent_->add_effect(this); }
void StairLightingEffect::stop() { parent_->remove_effect(this); }

void StairLightingEffect::apply(AddressableLight &it, const Color &current_color) {
  const uint32_t time = millis();
  apply_action(time, effect_action_, &StairLightingStep::effect_data);
  apply_action(time, night_action_, &StairLightingStep::night_data);

  bool schedule_show = false;
  for (auto *step : steps_) {
    schedule_show |= apply(*step, current_color);
  }
  if (schedule_show) {
    it.schedule_show();
  }
}

void StairLightingEffect::apply_action(uint32_t time, Action &action,
                                       const std::function<ProgressData &(StairLightingStep &)> &data) {
  float full_progress = (float) (time - action.get_start_time()) / (float) next_step_interval_;
  int i = 0;
  bool finish = false;
  for (auto *step : steps_) {
    float step_progress = calculate_step_progress(action, full_progress, i, finish);
    data(*step).push(action.get_operation(), step_progress);
    i++;
  }
  if (finish) {
    action.finish();
  }
}

float StairLightingEffect::calculate_step_progress(const Action &action, float progress, int32_t index,
                                                   bool &finished) {
  switch (action.get_category()) {
    case UP:
      progress -= (float) index;
      break;
    case DOWN:
      progress -= (float) (steps_.size() - index - 1);
      break;
    default:
      break;
  }
  progress = progress * ((float) next_step_interval_ / (float) progress_step_interval_);
  progress = clamp(progress, 0.0f, 1.0f);
  finished |= is_finished(action, index, progress);
  return progress;
}

bool StairLightingEffect::is_finished(const Action &action, int32_t index, float step_progress) {
  switch (action.get_category()) {
    case UP:
      return (index == steps_.size() - 1) && step_progress == 1.0f;
    case DOWN:
      return (index == 0) && step_progress == 1.0f;
    default:
      return step_progress == 1.0f;
  }
}

}  // namespace stair_lighting
}  // namespace esphome