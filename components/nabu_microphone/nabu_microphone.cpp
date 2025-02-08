#include "nabu_microphone.h"

#ifdef USE_ESP32

#include <driver/i2s.h>

#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "esphome/core/ring_buffer.h"

#ifdef USE_OTA
#include "esphome/components/ota/ota_backend.h"
#endif

namespace esphome {
namespace nabu_microphone {

static const size_t RING_BUFFER_LENGTH = 64;  // Measured in milliseconds
static const size_t QUEUE_LENGTH = 10;

static const size_t NUMBER_OF_CHANNELS = 2;
static const size_t DMA_BUFFER_SIZE = 160;
static const size_t DMA_BUFFERS_COUNT = 4;
static const size_t FRAMES_IN_ALL_DMA_BUFFERS = DMA_BUFFER_SIZE * DMA_BUFFERS_COUNT;
static const size_t SAMPLES_IN_ALL_DMA_BUFFERS = FRAMES_IN_ALL_DMA_BUFFERS * NUMBER_OF_CHANNELS;

static const size_t TASK_DELAY_MS = 10;

static const char *const TAG = "i2s_audio.microphone";

void NabuMicrophone::setup() {
    ESP_LOGCONFIG(TAG, "Setting up I2S Audio Microphone...");

#if SOC_I2S_SUPPORTS_ADC
    if (this->adc_) {
        if (this->parent_->get_port() != I2S_NUM_0) {
            ESP_LOGE(TAG, "Internal ADC only works on I2S0!");
            this->mark_failed();
            return;
        }
    } else
#endif
    if (this->pdm_) {
        if (this->parent_->get_port() != I2S_NUM_0) {
            ESP_LOGE(TAG, "PDM only works on I2S0!");
            this->mark_failed();
            return;
        }
    }

    this->event_queue_ = xQueueCreate(QUEUE_LENGTH, sizeof(TaskEvent));

#ifdef USE_OTA
    ota::get_global_ota_callback()->add_on_state_callback(
        [this](ota::OTAState state, float progress, uint8_t error, ota::OTAComponent *comp) {
            if (state == ota::OTA_STARTED) {
                if (this->read_task_handle_ != nullptr) {
                    vTaskSuspend(this->read_task_handle_);
                }
            } else if (state == ota::OTA_ERROR) {
                if (this->read_task_handle_ != nullptr) {
                    vTaskResume(this->read_task_handle_);
                }
            }
        });
#endif
}

void NabuMicrophoneChannel::setup() {
    const size_t ring_buffer_size = RING_BUFFER_LENGTH * this->parent_->get_sample_rate() / 1000 * sizeof(int16_t);
    this->ring_buffer_ = RingBuffer::create(ring_buffer_size);
    if (this->ring_buffer_ == nullptr) {
        ESP_LOGE(TAG, "Could not allocate ring buffer");
        this->mark_failed();
        return;
    }
}

void NabuMicrophoneChannel::loop() {
    if (this->parent_->is_running()) {
        if (this->is_muted_) {
            if (this->requested_stop_) {
                // The microphone was muted when stopping was requested
                this->state_ = microphone::STATE_STOPPED;
            } else {
                this->state_ = microphone::STATE_MUTED;
            }
        } else {
            this->state_ = microphone::STATE_RUNNING;
        }
    } else {
        this->state_ = microphone::STATE_STOPPED;
    }
}

}  // namespace nabu_microphone
}  // namespace esphome

#endif  // USE_ESP32
