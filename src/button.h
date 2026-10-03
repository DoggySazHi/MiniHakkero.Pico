#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

#define BUTTON_COUNT 4
#define DEBOUNCE_DELAY 50 // milliseconds

// Buttons [0, TOUCH_BUTTON_FIRST) are physical, the rest are touch-capacitive
#define TOUCH_BUTTON_FIRST 2
#define TOUCH_BUTTON_COUNT (BUTTON_COUNT - TOUCH_BUTTON_FIRST)

// Something something RC circuit - charge the pad, see how long it takes to discharge
#define TOUCH_CHARGE_US 5
#define TOUCH_MAX_TICKS 8000 // we are truly lost if this happens
#define TOUCH_SAMPLES 6
#define TOUCH_CALIBRATION_SAMPLES 32
#define TOUCH_SETTLE_MS 100
#define TOUCH_MIN_DELTA 40

void ButtonTask(void *pvParameters);

// Measure the baseline of every touch pad (do not touch them!) and update thresholds
void calibrateTouch();

class ButtonTaskState {
public:
    static bool buttons[BUTTON_COUNT];
    static bool rawButtons[BUTTON_COUNT];
    static pin_size_t buttonPins[BUTTON_COUNT];
    static unsigned long lastDebounceTime[BUTTON_COUNT];

    // Indexed by touch button (button index - TOUCH_BUTTON_FIRST)
    static uint32_t touchReading[TOUCH_BUTTON_COUNT];
    static uint32_t touchBaseline[TOUCH_BUTTON_COUNT];
    static uint32_t touchPressThreshold[TOUCH_BUTTON_COUNT];
    static uint32_t touchReleaseThreshold[TOUCH_BUTTON_COUNT];
};

#endif // BUTTON_H
