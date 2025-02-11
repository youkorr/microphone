#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/components/microphone/microphone.h"
#include "esphome/components/i2s_audio/i2s_audio.h"

namespace esphome {
namespace es7210 {

class ES7210Component : public Component, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  
  void set_i2s_audio(i2s_audio::I2SAudioComponent *i2s) { i2s_ = i2s; }
  void set_sample_rate(uint32_t rate) { sample_rate_ = rate; }
  void set_bits_per_sample(uint8_t bits) { bits_per_sample_ = bits; }
  void set_i2s_din_pin(uint8_t pin) { i2s_din_pin_ = pin; }
  void set_adc_type(const std::string &type) { adc_type_ = type; }
  i2s_port_t get_i2s_port() const { return i2s_->get_port(); }

 protected:
  uint32_t sample_rate_{16000};
  uint8_t bits_per_sample_{16};
  uint8_t i2s_din_pin_{GPIO_NUM_16};
  std::string adc_type_{"external"};
  i2s_audio::I2SAudioComponent *i2s_{nullptr};
};

class ES7210Microphone : public microphone::Microphone {
 public:
  void set_parent(ES7210Component *parent) { parent_ = parent; }
  
 protected:
  bool is_ready_() const override { return parent_ != nullptr; }
  int read_chunk_(int16_t *buffer, size_t length) override;
  
 private:
  ES7210Component *parent_{nullptr};
};

}  // namespace es7210
}  // namespace esphome
