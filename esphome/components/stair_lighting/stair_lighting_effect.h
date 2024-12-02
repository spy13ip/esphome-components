#pragma once

#include <vector>
#include "esphome/components/light/addressable_light_effect.h"

#include "types.h"

using std::vector;
using std::isnan;
using std::max;
using std::min;

using namespace esphome::light;

namespace esphome {
namespace stair_lighting {

class StairLightingComponent;

class Action {
 public:
  uint32_t get_start_time() const { return start_time_; }
  ActionCategory get_category() const { return category_; }
  ActionOperation get_operation() const { return operation_; }
  bool is_finished() const { return finished_; }

  void reset(uint32_t start_time, ActionCategory category, ActionOperation operation) {
    start_time_ = start_time;
    category_ = category;
    operation_ = operation;
    finished_ = false;
  }
  void finish() { finished_ = true; }

 protected:
  uint32_t start_time_ = 0;
  ActionCategory category_ = ActionCategory::FULL;
  ActionOperation operation_ = ActionOperation::OFF;
  bool finished_ = false;
};

struct ProgressData {
  ActionOperation operation = OFF;
  float value = 0.0f;
};

class Step {
 public:
  explicit Step(AddressableLight *light, int32_t offset, int32_t size, bool reversed)
      : light_(light), offset_(offset), size_(size), reversed_(reversed) {}

  ESPColorView operator[](int32_t index) const {
    if (!reversed_) {
      return (*light_)[offset_ + index];
    } else {
      return (*light_)[offset_ + size_ - 1 - index];
    }
  }
  int32_t size() const { return size_; }

  ProgressData &effect_data() { return effect_data_; }

 protected:
  AddressableLight *light_ = nullptr;
  int32_t offset_ = 0;
  int32_t size_ = 0;
  bool reversed_ = false;

  ProgressData effect_data_;
};

class StairLightingEffect : public AddressableLightEffect {
 public:
  explicit StairLightingEffect(const std::string &name) : AddressableLightEffect(name) {}

  void set_parent(StairLightingComponent *parent) { parent_ = parent; }
  void set_next_step_interval(uint32_t next_step_interval) { next_step_interval_ = next_step_interval; }
  void set_progress_step_interval(uint32_t progress_step_interval) { progress_step_interval_ = progress_step_interval; }

  void run_action(ActionCategory category, ActionOperation operation);

  void init() override;
  void start() override;
  void stop() override;

  void apply(AddressableLight &it, const Color &current_color) override;

 protected:
  StairLightingComponent *parent_ = nullptr;
  vector<Step *> steps_;
  uint32_t next_step_interval_ = 0;
  uint32_t progress_step_interval_ = 0;

  Action action_;

  virtual bool show(const Color &current_color, float brightness, float night_brightness) = 0;
  float calculate_step_progress(ActionCategory category, float full_progress, int32_t index);
};

}  // namespace stair_lighting
}  // namespace esphome
