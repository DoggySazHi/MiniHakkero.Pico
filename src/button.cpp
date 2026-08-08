#include "button.h"

#include <FreeRTOS.h>
#include <task.h>

#include "constants.h"

bool ButtonTaskState::buttons[BUTTON_COUNT] = {false, false, false, false};
bool ButtonTaskState::rawButtons[BUTTON_COUNT] = {false, false, false, false};
pin_size_t ButtonTaskState::buttonPins[BUTTON_COUNT] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4};
unsigned long ButtonTaskState::lastDebounceTime[BUTTON_COUNT] = {0, 0, 0, 0};

void readRawButtonStates() {
    // The first two buttons are actual buttons
    for (int i = 0; i < 2; ++i) {
        ButtonTaskState::rawButtons[i] = (digitalRead(ButtonTaskState::buttonPins[i]) == LOW);
    }

    // The last two buttons are capacitive touch sensors, which we do some math
    // TODO
}

void ButtonTask(void *pvParameters) {
    while (1) {
        readRawButtonStates();

        for (int i = 0; i < BUTTON_COUNT; ++i) {
            unsigned long currentTime = millis();
            PinStatus currentState = digitalRead(ButtonTaskState::buttonPins[i]);
            if (currentState == LOW) {
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
