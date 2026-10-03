#include "button.h"

#include <FreeRTOS.h>
#include <task.h>
#include <hardware/gpio.h>
#include <pico/time.h>

#include "constants.h"

bool ButtonTaskState::buttons[BUTTON_COUNT] = {false, false, false, false};
bool ButtonTaskState::rawButtons[BUTTON_COUNT] = {false, false, false, false};
pin_size_t ButtonTaskState::buttonPins[BUTTON_COUNT] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4};
unsigned long ButtonTaskState::lastDebounceTime[BUTTON_COUNT] = {0, 0, 0, 0};

uint32_t ButtonTaskState::touchReading[TOUCH_BUTTON_COUNT] = {0, 0};
uint32_t ButtonTaskState::touchBaseline[TOUCH_BUTTON_COUNT] = {0, 0};
uint32_t ButtonTaskState::touchPressThreshold[TOUCH_BUTTON_COUNT] = {UINT32_MAX, UINT32_MAX};
uint32_t ButtonTaskState::touchReleaseThreshold[TOUCH_BUTTON_COUNT] = {UINT32_MAX, UINT32_MAX};

static uint32_t __not_in_flash_func(measureTouchOnce)(uint pin) {
    uint32_t ticks = 0;

    taskENTER_CRITICAL();
    digitalWrite(pin, HIGH);
    gpio_put(pin, 1);
    gpio_set_dir(pin, GPIO_OUT);
    busy_wait_us_32(TOUCH_CHARGE_US);
    gpio_set_dir(pin, GPIO_IN);
    while (gpio_get(pin) && ticks < TOUCH_MAX_TICKS) {
        ++ticks;
    }
    taskEXIT_CRITICAL();

    return ticks;
}

// Find the minimum time across multiple measurements, because RTOS may increase the measurements
static uint32_t readTouch(uint pin) {
    uint32_t best = UINT32_MAX;
    for (int i = 0; i < TOUCH_SAMPLES; ++i) {
        uint32_t ticks = measureTouchOnce(pin);
        if (ticks < best) {
            best = ticks;
        }
    }
    return best;
}

static void setTouchThresholds(int touchIndex, uint32_t baseline) {
    uint32_t pressDelta = max(baseline / 2, (uint32_t)TOUCH_MIN_DELTA);
    uint32_t releaseDelta = max(baseline / 4, (uint32_t)TOUCH_MIN_DELTA / 2);

    ButtonTaskState::touchBaseline[touchIndex] = baseline;
    ButtonTaskState::touchPressThreshold[touchIndex] = baseline + pressDelta;
    ButtonTaskState::touchReleaseThreshold[touchIndex] = baseline + releaseDelta;
}

static uint32_t measureTouchBaseline(int touchIndex) {
    uint pin = ButtonTaskState::buttonPins[TOUCH_BUTTON_FIRST + touchIndex];
    // use the average (median, NOT mean, because stupid statistics with outliers)
    uint32_t samples[TOUCH_CALIBRATION_SAMPLES];

    for (int i = 0; i < TOUCH_CALIBRATION_SAMPLES; ++i) {
        uint32_t reading = readTouch(pin);

        // Insertion sort? lol
        int j = i;
        while (j > 0 && samples[j - 1] > reading) {
            samples[j] = samples[j - 1];
            --j;
        }
        samples[j] = reading;

        vTaskDelay(pdMS_TO_TICKS(1));
    }

    return samples[TOUCH_CALIBRATION_SAMPLES / 2];
}

void calibrateTouch() {
    for (int i = 0; i < TOUCH_BUTTON_COUNT; ++i) {
        setTouchThresholds(i, measureTouchBaseline(i));
    }
}

void readRawButtonStates() {
    // Physical buttons are active low
    for (int i = 0; i < TOUCH_BUTTON_FIRST; ++i) {
        ButtonTaskState::rawButtons[i] = (digitalRead(ButtonTaskState::buttonPins[i]) == LOW);
    }

    // Touch pads are pressed above the press threshold, and released below the
    // release threshold (hysteresis: in between, the previous state is kept)
    for (int i = 0; i < TOUCH_BUTTON_COUNT; ++i) {
        int button = TOUCH_BUTTON_FIRST + i;
        uint32_t reading = readTouch(ButtonTaskState::buttonPins[button]);
        ButtonTaskState::touchReading[i] = reading;

        if (reading > ButtonTaskState::touchPressThreshold[i]) {
            ButtonTaskState::rawButtons[button] = true;
        } else if (reading < ButtonTaskState::touchReleaseThreshold[i]) {
            ButtonTaskState::rawButtons[button] = false;
        }
    }
}

void ButtonTask(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(TOUCH_SETTLE_MS));
    calibrateTouch();

    while (1) {
        readRawButtonStates();

        for (int i = 0; i < BUTTON_COUNT; ++i) {
            unsigned long currentTime = millis();
            bool pressed = ButtonTaskState::rawButtons[i];
            if (pressed) {
                if (!ButtonTaskState::buttons[i] && (currentTime - ButtonTaskState::lastDebounceTime[i]) > DEBOUNCE_DELAY) {
                    ButtonTaskState::buttons[i] = true;
                    ButtonTaskState::lastDebounceTime[i] = currentTime;
                }
            } else {
                if (ButtonTaskState::buttons[i] && (currentTime - ButtonTaskState::lastDebounceTime[i]) > DEBOUNCE_DELAY) {
                    ButtonTaskState::buttons[i] = false;
                    ButtonTaskState::lastDebounceTime[i] = currentTime;
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
