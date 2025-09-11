#include "ultrasonic_interrupt_sensor.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace ultrasonic_interrupt {

static const char *const TAG = "ultrasonic_interrupt.sensor";

void UltrasonicInterruptSensorComponent::setup() {
  this->trigger_pin_->setup();
  this->trigger_pin_->digital_write(false);

  this->echo_pin_->setup();
  this->echo_isr_ = echo_pin_->to_isr();
  this->echo_pin_->attach_interrupt(&UltrasonicInterruptSensorComponent::echo_interrupt, this,
                                    gpio::INTERRUPT_ANY_EDGE);

  this->timeout_ms_ = (this->timeout_us_ + 999) / 1000;
}

void UltrasonicInterruptSensorComponent::update() {
  // триггерный импульс
  this->trigger_pin_->digital_write(true);
  delayMicroseconds(this->pulse_time_us_);
  this->trigger_pin_->digital_write(false);

  // Таймаут ожидания эха
  this->set_timeout(this->timeout_ms_, [this] {
    if (this->new_data_) {
      // Уже есть измерение → обработать
      this->new_data_ = false;

      uint32_t duration = this->pulse_end_ - this->pulse_start_;
      if (duration >= this->timeout_us_) {
        ESP_LOGD(TAG, "'%s' - Distance measurement timed out!", this->name_.c_str());
        this->publish_state(NAN);
      } else {
        float result = us_to_m(duration);
        ESP_LOGD(TAG, "'%s' - Got distance: %.3f m", this->name_.c_str(), result);
        this->publish_state(result);
      }
    } else {
      // Данных нет → эхо не пришло
      ESP_LOGD(TAG, "'%s' - No echo received within timeout!", this->name_.c_str());
      this->publish_state(NAN);
    }
  });
}

void IRAM_ATTR UltrasonicInterruptSensorComponent::echo_interrupt(UltrasonicInterruptSensorComponent *self) {
  if (self->echo_isr_.digital_read()) {
    self->pulse_start_ = micros();
  } else {
    self->pulse_end_ = micros();
    self->new_data_ = true;
  }
}

void UltrasonicInterruptSensorComponent::dump_config() {
  LOG_SENSOR("", "Ultrasonic Sensor (interrupt version)", this);
  LOG_PIN("  Echo Pin: ", this->echo_pin_);
  LOG_PIN("  Trigger Pin: ", this->trigger_pin_);
  ESP_LOGCONFIG(TAG,
                "  Pulse time: %" PRIu32 " µs\n"
                "  Timeout: %" PRIu32 " µs",
                this->pulse_time_us_, this->timeout_us_);
  LOG_UPDATE_INTERVAL(this);
}

float UltrasonicInterruptSensorComponent::us_to_m(uint32_t us) {
  const float speed_sound_m_per_s = 343.0f;
  const float time_s = us / 1e6f;
  const float total_dist = time_s * speed_sound_m_per_s;
  return total_dist / 2.0f;
}

}  // namespace ultrasonic_interrupt
}  // namespace esphome
