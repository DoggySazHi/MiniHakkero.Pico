#ifndef LED_H
#define LED_H

#include <Arduino.h>
#include <FastLED.h>

#include "constants.h"

enum LEDState {
    LED_OFF,
    LED_READY,
    LED_FIRE,
    LED_NO_OF_STATES // Used to determine the number of states in the enum
};

void LEDTask(void *pvParameters);

void led_state_off();
void led_state_ready();
void led_state_fire();

class LEDTaskState {
public:
    static CRGB leds[WS2812B_LED_TOTAL];
    static LEDState ledState;
    
    static void nextState() {
        ledState = static_cast<LEDState>((ledState + 1) % LED_NO_OF_STATES);
    }
};

#endif // LED_H