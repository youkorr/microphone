#include "es7210.h"
#include "esphome/core/log.h"
#include "driver/i2s.h"
#include "gpio_definitions.h"

namespace esphome {
namespace es7210 {

static const char *const TAG = "es7210";

void ES7210Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ES7210 Codec for ESP32 S3 Box 3");
  
  // Verify device presence
  uint8_t value;
  if (!this->read_register(ES7210_RESET_REG, &value)) {
    ESP_LOGE(TAG, "ES7210 not found at address 0x%02X", this->address_);
    mark_failed();
    return;
  }

  // Initialize codec and configure I2S
  initialize_codec();
  configure_analog_path();
  configure_i2s();
}

// ... [rest of the file remains the same until read_register]

bool ES7210Component::write_register(uint8_t reg, uint8_t value) {
  return this->write_byte(reg, value);
}

bool ES7210Component::read_register(uint8_t reg, uint8_t *value) {
  return this->read_byte(reg, value);
}

}  // namespace es7210
}  // namespace esphome
