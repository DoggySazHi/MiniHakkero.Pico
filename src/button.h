#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

#define BUTTON_COUNT 4
#define DEBOUNCE_DELAY 50 // milliseconds

void ButtonTask(void *pvParameters);

class ButtonTaskState {
public:
    static bool buttons[BUTTON_COUNT];
    static bool rawButtons[BUTTON_COUNT];
    static pin_size_t buttonPins[BUTTON_COUNT];
    static unsigned long lastDebounceTime[BUTTON_COUNT];
};

#endif // BUTTON_H