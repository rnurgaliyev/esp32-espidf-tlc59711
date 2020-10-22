#include <esp_event.h>
#include <esp_log.h>

#include "effects.h"

#define TAG "EFFECT_VERTICAL_TRAVELERS"
#define SPEED pdMS_TO_TICKS(60)
#define DELAY pdMS_TO_TICKS(180)

void vertical_travelers_stop();
void send_pixel_down(uint8_t x, uint8_t y, TickType_t xTicksToDelay);
void send_pixel_up(uint8_t x, uint8_t y, TickType_t xTicksToDelay);

rgb_t on = {255, 255, 255};
rgb_t off = {0, 0, 0};

void vertical_travelers() {
    int x, y, last_x = 0, last_y = 0;

    ESP_ERROR_CHECK(esp_event_handler_register_with(event_loop, EFFECT_EVENTS, EFFECT_EVENT_STOP, vertical_travelers_stop, NULL));

    // Initial fill
    for (x = 0; x < 8; x++) {
        for (y = 0; y < 8; y++) {
            fb_set_pixel(x, y, ((rand() % 2) * 7), on);
        }
    }

    while (true) {
        x = rand() % 8;
        y = rand() % 8;

        if (y != last_y && x != last_x) {
            if (color_equals(on, fb_get_pixel(x, y, 0))) {
                send_pixel_down(x, y, SPEED);
            } else {
                send_pixel_up(x, y, SPEED);
            }
            last_y = y;
            last_x = x;

            vTaskDelay(DELAY);
        }
    }
}

void vertical_travelers_stop() {
    ESP_LOGI(TAG, "stopped");
    fb_clear();
    ESP_ERROR_CHECK(esp_event_handler_unregister_with(event_loop, EFFECT_EVENTS, EFFECT_EVENT_STOP, vertical_travelers_stop));
}

void send_pixel_down(uint8_t x, uint8_t y, TickType_t xTicksToDelay) {
    for (int i = 0; i < 7; i++) {
        fb_set_pixel(x, y, i, off);
        fb_set_pixel(x, y, i + 1, on);
        vTaskDelay(SPEED);
    }
}

void send_pixel_up(uint8_t x, uint8_t y, TickType_t xTicksToDelay) {
    for (int i = 7; i > 0; i--) {
        fb_set_pixel(x, y, i, off);
        fb_set_pixel(x, y, i - 1, on);
        vTaskDelay(SPEED);
    }
}