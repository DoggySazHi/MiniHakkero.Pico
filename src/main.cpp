#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "constants.h"
#include "audio.h"
#include "button.h"
#include "charge.h"
#include "led.h"
#include "state.h"
#include "debug.h"

void setup() {
    pinMode(BUTTON_1, INPUT_PULLUP);
    pinMode(BUTTON_2, INPUT_PULLUP);
    pinMode(BUTTON_3, INPUT_PULLUP);
    pinMode(BUTTON_4, INPUT_PULLUP);

    pinMode(SPEAKER_SDA, OUTPUT);
    pinMode(SPEAKER_SCL, OUTPUT);
    pinMode(SPEAKER_LRC, OUTPUT);

    pinMode(WS2812B_DIN, OUTPUT);
    pinMode(WS2812B_EN, OUTPUT);

    xTaskCreate(AudioTask, "AudioTask", 2048, NULL, 1, NULL);
    xTaskCreate(ButtonTask, "ButtonTask", 1024, NULL, 1, NULL);
    xTaskCreate(ChargeTask, "ChargeTask", 1024, NULL, 1, NULL);
    xTaskCreate(LEDTask, "LEDTask", 1024, NULL, 1, NULL);
    xTaskCreate(StateTask, "StateTask", 1024, NULL, 1, NULL);
    xTaskCreate(DebugTask, "DebugTask", 1024, NULL, 1, NULL);
}

void loop() {
    // FreeRTOS lmao!
}
