#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_event.h"
#include "esp_log.h"

#include "event_loop.h"
#include "mic.h"
#include "effects.h"

#define TAG "SPECTRUM"

void mic_spectrum_stop();

void mic_frame_ready_handler(void* handler_arg, esp_event_base_t base, int32_t id, void* event_data)
{
    uint8_t *s = ((uint8_t*) event_data);
    rgb_t rgb;
    uint8_t c = 0;


    ESP_LOGI(TAG, "Frame ready: [%d %d %d %d %d %d %d %d]", s[0] / 32, s[1] / 32, s[2] / 32, s[3] / 32, s[4] / 32, s[5] / 32, s[6] / 32, s[7] / 32);

    fb_shift_x(FB_SHIFT_BACK);

    for (int x = 0; x < 8; x++) {
        uint8_t v = s[x] / 32;
        rgb = hue_to_rgb_sine2(c);
        while (v > 0) {
            fb_set_pixel(7, x, 8 - v--, rgb);
        }
        c += 32;
    }
}


void mic_spectrum() {
    ESP_ERROR_CHECK(esp_event_handler_register_with(event_loop, MIC_EVENTS, MIC_EVENT_FRAME_READY, mic_frame_ready_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register_with(event_loop, EFFECT_EVENTS, EFFECT_EVENT_STOP, mic_spectrum_stop, NULL));
    while (true)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void mic_spectrum_stop() {
    ESP_LOGI(TAG, "Called mic_spectrum_stop");
    ESP_ERROR_CHECK(esp_event_handler_unregister_with(event_loop, MIC_EVENTS, MIC_EVENT_FRAME_READY, mic_frame_ready_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister_with(event_loop, EFFECT_EVENTS, EFFECT_EVENT_STOP, mic_spectrum_stop));
}