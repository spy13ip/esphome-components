#pragma once

#include <utility>
#include <vector>

#include "esphome/components/light/addressable_light_effect.h"

using std::vector;
using namespace esphome::light;

namespace esphome {
namespace stair_lighting {

struct StepConfig {
  int32_t size;
  bool reversed;
};

enum ActionCategory { FULL, UP, DOWN };

enum ActionOperation { INCREASE, DECREASE };

class Action {
 public:
  Action(uint32_t start_time, ActionCategory category, ActionOperation operation)
      : start_time_(start_time), category_(category), operation_(operation) {}

  uint32_t get_start_time() const { return start_time_; }
  ActionCategory get_category() const { return category_; }
  ActionOperation get_operation() const { return operation_; }
  bool is_finished() const { return finished_; }

  void finish() { finished_ = true; }

 protected:
  uint32_t start_time_;
  ActionCategory category_;
  ActionOperation operation_;

  bool finished_{false};
};

class ProgressData {
 public:
  float get(ActionOperation operation) const { return data_[operation]; }
  ActionOperation last_operation() const { return lastOperation_; }

  void reset() {
    data_[INCREASE] = NAN;
    data_[DECREASE] = NAN;
  }

  void push(ActionOperation operation, float value) {
    lastOperation_ = operation;
    float &data_value = data_[operation];
    data_value = isnan(data_value) ? value : max(data_value, value);
  }

 protected:
  float data_[2]{};
  ActionOperation lastOperation_ = DECREASE;
};

class Step {
 public:
  explicit Step(AddressableLight *light, int32_t offset, StepConfig &config)
      : light_(light), offset_(offset), config_(config) {}

  ESPColorView operator[](int32_t index) const {
    if (!config_.reversed) {
      return (*light_)[offset_ + index];
    } else {
      return (*light_)[offset_ + config_.size - 1 - index];
    }
  }
  int32_t size() const { return config_.size; }

  ProgressData &effect_data() { return effect_data_; }
  ProgressData &night_data() { return night_data_; }

 protected:
  AddressableLight *light_;
  int32_t offset_;
  StepConfig config_;
  ProgressData effect_data_;
  ProgressData night_data_;
};

class StairLightingEffect : public AddressableLightEffect {
 public:
  explicit StairLightingEffect(const std::string &name, const vector<StepConfig> &step_config)
      : AddressableLightEffect(name), steps_config_(step_config) {}

  void set_next_step_interval(uint32_t next_step_interval) { next_step_interval_ = next_step_interval; }
  void set_progress_step_interval(uint32_t progress_step_interval) { progress_step_interval_ = progress_step_interval; }

  void add_effect_action(ActionCategory category, ActionOperation operation) {
    effect_actions_.emplace_back(millis(), category, operation);
  }

  void add_night_action(ActionCategory category, ActionOperation operation) {
    night_actions_.emplace_back(millis(), category, operation);
  }

  void apply(AddressableLight &it, const Color &current_color) override {
    const uint32_t time = millis();
    apply_actions(effect_actions_, &Step::effect_data, time);
    apply_actions(night_actions_, &Step::night_data, time);

    bool schedule_show = false;
    for (auto *step : steps_) {
      schedule_show |= apply(*step, current_color);
    }
    if (schedule_show) {
      it.schedule_show();
    }
  }

  void init() override {
    auto *light = static_cast<AddressableLight *>(state_->get_output());
    steps_.resize(steps_config_.size());
    int32_t offset = 0;
    for (auto &step_config : steps_config_) {
      steps_.push_back(new Step(light, offset, step_config));
      offset += step_config.size;
    }
  }

 protected:
  vector<StepConfig> steps_config_;
  uint32_t next_step_interval_ = 0;
  uint32_t progress_step_interval_ = 0;
  vector<Step *> steps_;

  vector<Action> effect_actions_;
  vector<Action> night_actions_;

  virtual bool apply(Step &step, const Color &current_color) = 0;

  void apply_actions(vector<Action> &actions, const std::function<ProgressData &(Step &)> &data, const uint32_t time) {
    clean_actions(actions);
    reset_data(data);
    for (auto &action : actions) {
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
  }

  static void clean_actions(vector<Action> &actions) {
    auto start = actions.begin();
    while (start != actions.end()) {
      auto operation = start->get_operation();
      auto it = start;
      bool any_finished = false;
      while (it != actions.end() || it->get_operation() != operation) {
        any_finished |= it->is_finished();
        it++;
      }
      if (any_finished) {
        actions.erase(start, it);
      } else {
        start = it;
      }
    }
  }

  void reset_data(const std::function<ProgressData &(Step &)> &data) {
    for (auto *step : steps_) {
      data(*step).reset();
    }
  }

  float calculate_step_progress(const Action &action, float progress, int32_t index, bool &finished) {
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

  bool is_finished(const Action &action, int32_t index, float step_progress) {
    switch (action.get_category()) {
      case UP:
        return (index == steps_.size() - 1) && step_progress == 1.0f;
      case DOWN:
        return (index == 0) && step_progress == 1.0f;
      default:
        return step_progress == 1.0f;
    }
  }
};

}  // namespace stair_lighting
}  // namespace esphome