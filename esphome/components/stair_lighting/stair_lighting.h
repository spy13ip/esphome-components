#pragma once

#include <vector>
#include <set>

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

using std::vector;
using std::set;
using namespace esphome::binary_sensor;

namespace esphome {
namespace stair_lighting {

class StairLightingEffect;

struct StairLightingStepConfig {
  int32_t offset;
  int32_t size;
  bool reversed;
};

class StairLightingComponent : public Component {
 public:
  void set_steps_config(const vector<StairLightingStepConfig> &steps_config) { steps_config_ = steps_config; }
  void set_upper_sensor(BinarySensor *upper_sensor) { upper_sensor_ = upper_sensor; }
  void set_lower_sensor(BinarySensor *lower_sensor) { lower_sensor_ = lower_sensor; }

  void set_effect_brightness(float effect_brightness) { effect_brightness_ = effect_brightness; }
  void set_night_brightness(float night_brightness) { night_brightness_ = night_brightness; }

  void turn_up();
  void turn_down();
  void turn_full();
  void turn_off();

  void add_effect(StairLightingEffect *effect);
  void remove_effect(StairLightingEffect *effect);
  vector<StairLightingStepConfig> &get_steps_config() { return steps_config_; }

  void setup() override;
  void loop() override;

 protected:
  vector<StairLightingStepConfig> steps_config_;
  BinarySensor *upper_sensor_ = nullptr;
  BinarySensor *lower_sensor_ = nullptr;
  float effect_brightness_ = 0;
  float night_brightness_ = 0;
  set<StairLightingEffect *> effects_;
};

}  // namespace stair_lighting
}  // namespace esphome