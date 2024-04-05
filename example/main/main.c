#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <stdint.h>
#include <inttypes.h>
#include <esp_random.h>
#include <math.h>
#include "string.h"

#include "opus.h"

#define SAMPLERATE (48000)

static const char* TAG = "main";

void app_main(void) 
{
  ESP_LOGI(TAG, "Starting Opus Version %s", opus_get_version_string());
  int err = 0;
  OpusEncoder* enc = opus_encoder_create(SAMPLERATE, 1, OPUS_APPLICATION_AUDIO, &err);
  if (err != OPUS_OK)
    {
    ESP_LOGE(TAG, "Failed to create encoder %d", err);
    }
  ESP_LOGI(TAG, "Created encoder: 48kSps, 1 channel, Audio application");

  opus_encoder_ctl(enc, OPUS_SET_BITRATE(OPUS_AUTO));
  opus_encoder_ctl(enc, OPUS_SET_BANDWIDTH(OPUS_BANDWIDTH_FULLBAND));
  opus_encoder_ctl(enc, OPUS_SET_VBR(0));
  opus_encoder_ctl(enc, OPUS_SET_VBR_CONSTRAINT(0));
  opus_encoder_ctl(enc, OPUS_SET_COMPLEXITY(1));
  opus_encoder_ctl(enc, OPUS_SET_INBAND_FEC(0));
  opus_encoder_ctl(enc, OPUS_SET_FORCE_CHANNELS(1));
  opus_encoder_ctl(enc, OPUS_SET_DTX(0));
  opus_encoder_ctl(enc, OPUS_SET_PACKET_LOSS_PERC(0));

  // Get encoder sample delay
  opus_int32 skip = 0;
  opus_encoder_ctl(enc, OPUS_GET_LOOKAHEAD(&skip));
  opus_encoder_ctl(enc, OPUS_SET_LSB_DEPTH(16));
  opus_encoder_ctl(enc, OPUS_SET_EXPERT_FRAME_DURATION(OPUS_FRAMESIZE_ARG));

  ESP_LOGI(TAG, "Configured encoder");

  //int max_frame_size = 1000;
  int frame_size = 960;
  int max_payload_bytes = 4096;
  uint8_t* data = (uint8_t*)malloc(max_payload_bytes);
  opus_int16* in = (opus_int16*)malloc(frame_size * 1 * sizeof(opus_int16));
  int64_t start, end, duration;
  int frame_duration_ms = frame_size*1000/SAMPLERATE;
  while(1)
    {
    esp_fill_random(in, frame_size * sizeof(opus_int16));
    //memset(in, 0, frame_size * sizeof(opus_int16));
    //for (int i = 0; i < frame_size; i++)
    //  {
    //  in[i] = (int16_t) ((float) SHRT_MAX * sin((double)i / 50.0));
    //  }
    start = esp_timer_get_time();
    opus_int32 len = opus_encode(enc, in, frame_size, data, max_payload_bytes);
    end = esp_timer_get_time();
    duration = end - start;
    if (len < 0)
      {
      ESP_LOGE(TAG, "Error encoding data to packet %d", (int)len);
      }
    else
      {
      float ratio = (float) (frame_size * sizeof(opus_int16)) / (float) len;
      float cpu_usage = ((float) duration / ((float)frame_duration_ms * 10.0));
      ESP_LOGI(TAG, "Encoded success, %.1f%% CPU, %" PRId64 "us, %dms data, %d bytes out, compression %.1fx", cpu_usage, duration, frame_duration_ms, (int) len, ratio);
      }
    vTaskDelay(200);
    }
}
