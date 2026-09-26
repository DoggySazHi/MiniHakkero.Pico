#include "led.h"

#include <FreeRTOS.h>
#include <task.h>

#include "charge.h"

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
    FastLED.clear(true);

    digitalWrite(WS2812B_EN, HIGH);

    if (ChargeTaskState::chargeState == CHARGE_CHARGING) {
        // If charging, show a pulsing red light on the inner ring
        uint8_t brightness = (sin(millis() / 1000.0 * 2 * PI) + 1) / 2 * 40; // Pulsing effect
        for (int i = WS2812B_LED_OUTER; i < WS2812B_LED_TOTAL; ++i) {
            LEDTaskState::leds[i] = CRGB(brightness, 0, 0); // Red
        }
    } else if (ChargeTaskState::chargeState == CHARGE_FULLY_CHARGED) {
        // If fully charged, show a solid green light on the inner ring
        for (int i = WS2812B_LED_OUTER; i < WS2812B_LED_TOTAL; ++i) {
            LEDTaskState::leds[i] = CRGB(0, 40, 0); // Green
        }
    } else {
        digitalWrite(WS2812B_EN, LOW); // Turn off the LED ring if not charging
        digitalWrite(WS2812B_DIN, LOW); // Ensure the data line is low (avoid quiescent current)
    }
}

void led_state_ready() {
    // Rainbow pattern for the outer ring
    for (int i = 0; i < WS2812B_LED_OUTER; ++i) {
        uint8_t hue = (i * 255) / WS2812B_LED_OUTER;
        hue = (hue + (millis() / 10)) % 255;
        LEDTaskState::leds[i] = CHSV(hue, 255, 255);
    }

    FastLED.show();
}

void led_state_fire() {
    // All white, with some rainbow on the outer ring (darkened)
    for (int i = 0; i < WS2812B_LED_OUTER; ++i) {
        uint8_t hue = (i * 255) / WS2812B_LED_OUTER;
        hue = (hue + (millis() / 10)) % 255;
        LEDTaskState::leds[i] = CHSV(hue, 255, 150);
    }
    
    for (int i = WS2812B_LED_OUTER; i < WS2812B_LED_TOTAL; ++i) {
        LEDTaskState::leds[i] = CRGB::White; // White
    }

    FastLED.show();
}
