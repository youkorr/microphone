#include "es7210.h"
#include "esphome/core/log.h"

namespace esphome {
namespace es7210 {

static const char *const TAG = "es7210";

void ES7210Microphone::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ES7210 Microphone");
  
  // Initialize I2C
  if (!this->read_byte(0x00)) {
    ESP_LOGE(TAG, "ES7210 not found!");
    this->mark_failed();
    return;
  }

  // Configure I2S
  if (this->i2s_) {
    this->i2s_->set_sample_rate(this->sample_rate_);
    this->i2s_->set_bits_per_sample(this->bits_per_sample_);
    this->i2s_->set_din_pin(this->i2s_din_pin_);
  }

  this->ready_ = true;
}

void ES7210Microphone::dump_config() {
  ESP_LOGCONFIG(TAG, "ES7210 Microphone:");
  LOG_I2C_DEVICE(this);
  ESP_LOGCONFIG(TAG, "  Sample Rate: %u Hz", this->sample_rate_);
  ESP_LOGCONFIG(TAG, "  Bits per Sample: %u", this->bits_per_sample_);
  ESP_LOGCONFIG(TAG, "  I2S DIN Pin: GPIO%u", this->i2s_din_pin_);
  ESP_LOGCONFIG(TAG, "  ADC Type: %s", this->adc_type_.c_str());
}

int ES7210Microphone::read_chunk_(int16_t *buffer, size_t length) {
  if (!this->ready_ || !this->i2s_) return 0;
  return this->i2s_->read(buffer, length);
}

}  // namespace es7210
}  // namespace esphome
