#include "led.h"

#include <FreeRTOS.h>
#include <task.h>

CRGB LEDTaskState::leds[WS2812B_LED_TOTAL];
LEDState LEDTaskState::ledState = LED_OFF;

void LEDTask(void *pvParameters) {
    FastLED.addLeds<NEOPIXEL, WS2812B_DIN>(LEDTaskState::leds, WS2812B_LED_TOTAL);

    while (1) {
        switch (LEDTaskState::ledState) {
            case LED_OFF:
                led_state_off();
                break;
            case LED_READY:
                led_state_ready();
                break;
            case LED_FIRE:
                led_state_fire();
                break;
            default:
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void led_state_off() {
    FastLED.clear();
    FastLED.show();
}

void led_state_ready() {
    // Rainbow pattern for the outer ring
    for (int i = 0; i < WS2812B_LED_OUTER; ++i) {
        LEDTaskState::leds[i] = CHSV((i * 255) / WS2812B_LED_OUTER, 255, 255); // Rainbow
    }

    FastLED.show();
}

void led_state_fire() {
    // All white, with some rainbow on the outer ring
    for (int i = 0; i < WS2812B_LED_OUTER; ++i) {
        LEDTaskState::leds[i] = CHSV((i * 255) / WS2812B_LED_OUTER, 255, 255); // Rainbow
    }
    for (int i = WS2812B_LED_OUTER; i < WS2812B_LED_TOTAL; ++i) {
        LEDTaskState::leds[i] = CRGB::White; // White
    }

    FastLED.show();
}
