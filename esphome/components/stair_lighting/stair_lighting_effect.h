#pragma once

#include <vector>

#include "esphome/components/light/addressable_light_effect.h"
#include "stair_lighting.h"

using std::vector;
using namespace esphome::light;

namespace esphome {
namespace stair_lighting {

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

class StairLightingStep {
 public:
  explicit StairLightingStep(int32_t size, bool reversed) : size_(size), reversed_(reversed) {}

  void init(AddressableLight *light, int32_t offset) {
    light_ = light;
    offset_ = offset;
  }

  ESPColorView operator[](int32_t index) const {
    if (!reversed_) {
      return (*light_)[offset_ + index];
    } else {
      return (*light_)[offset_ + size_ - 1 - index];
    }
  }
  int32_t size() const { return size_; }

  ProgressData &effect_data() { return effect_data_; }
  ProgressData &night_data() { return night_data_; }

 protected:
  AddressableLight *light_ = nullptr;
  int32_t offset_ = 0;
  int32_t size_ = 0;
  bool reversed_ = false;

  ProgressData effect_data_;
  ProgressData night_data_;
};

class StairLightingEffect : public AddressableLightEffect {
 public:
  explicit StairLightingEffect(const std::string &name) : AddressableLightEffect(name) {}

  void set_parent(StairLightingComponent *parent) { parent_ = parent; }
  void add_steps(vector<StairLightingStep *> &steps) {
    for (auto *step : steps) {
      steps_.push_back(step);
    }
  }
  void set_next_step_interval(uint32_t next_step_interval) { next_step_interval_ = next_step_interval; }
  void set_progress_step_interval(uint32_t progress_step_interval) { progress_step_interval_ = progress_step_interval; }

  void run_effect_action(ActionCategory category, ActionOperation operation);
  void run_night_action(ActionCategory category, ActionOperation operation);

  void init() override;
  void start() override;
  void stop() override;

  void apply(AddressableLight &it, const Color &current_color) override;

 protected:
  StairLightingComponent *parent_ = nullptr;
  vector<StairLightingStep *> steps_;
  uint32_t next_step_interval_ = 0;
  uint32_t progress_step_interval_ = 0;

  vector<Action> effect_actions_;
  vector<Action> night_actions_;

  virtual bool apply(StairLightingStep &step, const Color &current_color) = 0;

  void apply_actions(vector<Action> &actions, const std::function<ProgressData &(StairLightingStep &)> &data, uint32_t time);
  static void clean_actions(vector<Action> &actions);
  void reset_data(const std::function<ProgressData &(StairLightingStep &)> &data);
  float calculate_step_progress(const Action &action, float progress, int32_t index, bool &finished);
  bool is_finished(const Action &action, int32_t index, float step_progress);
};

}  // namespace stair_lighting
}  // namespace esphome