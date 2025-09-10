#pragma once

#include "esphome/core/component.h"
#include "esphome/core/gpio.h"
#include "esphome/components/sensor/sensor.h"

#include <cinttypes>

namespace esphome {
namespace ultrasonic_interrupt {

class UltrasonicInterruptSensorComponent : public sensor::Sensor, public PollingComponent {
 public:
  void set_trigger_pin(GPIOPin *trigger_pin) { trigger_pin_ = trigger_pin; }
  void set_echo_pin(InternalGPIOPin *echo_pin) { echo_pin_ = echo_pin; }
  void set_pulse_time_us(uint32_t pulse_time_us) { this->pulse_time_us_ = pulse_time_us; }
  void set_timeout_us(uint32_t timeout_us) { this->timeout_us_ = timeout_us; }

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Set up pins and register interval.
  void setup() override;
  void update() override;

  float get_setup_priority() const override { return setup_priority::DATA; }
  void dump_config() override;

  static void IRAM_ATTR gpio_intr(UltrasonicInterruptSensorComponent *self) {
    bool level = self->echo_pin_->digital_read();
    if (level) {
      self->pulse_start_ = micros();
    } else {
      self->pulse_end_ = micros();
      self->new_data_ = true;
    }
  }

 protected:
  static float us_to_m(uint32_t us);

  GPIOPin *trigger_pin_;
  InternalGPIOPin *echo_pin_;
  uint32_t pulse_time_us_{};
  uint32_t timeout_us_{};

  volatile uint32_t pulse_start_{0};
  volatile uint32_t pulse_end_{0};
  volatile bool new_data_{false};
};

}  // namespace ultrasonic_interrupt
}  // namespace esphome
