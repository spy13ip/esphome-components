#include "stair_lighting_step.h"

namespace esphome {
namespace stair_lighting {

void StairLightingStep::init() {
  if (start_ > end_) {
    reverse_ = true;
    offset_ = end_;
    size_ = start_ - end_ + 1;
  } else {
    reverse_ = false;
    offset_ = start_;
    size_ = end_ - start_ + 1;
  }
}

uint32_t StairLightingStep::size() { return size_; }

light::ESPColorView StairLightingStep::operator[](int32_t index) const {
  return (*light_)[reverse_ ? offset_ + size_ - index - 1 : offset_ + index];
}

void StairLightingStep::schedule_show() { light_->schedule_show(); }

}  // namespace stair_lighting
}  // namespace esphome
