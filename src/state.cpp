#include "state.h"

#include <FreeRTOS.h>
#include <task.h>

#include "audio.h"
#include "button.h"
#include "led.h"

MiniHakkeroState StateMachine::currentState = STATE_IDLE;
bool StateMachine::buttonHeld = false;

void StateTask(void *pvParameters) {
    while (1) {
        if (ButtonTaskState::buttons[2] && !StateMachine::buttonHeld) {
            nextState();
            StateMachine::buttonHeld = true;
        } else if (!ButtonTaskState::buttons[2]) {
            StateMachine::buttonHeld = false;
        }

        // Figure what to do with the other buttons

        vTaskDelay(pdMS_TO_TICKS(10)); // Delay to yield to other tasks
    }
}

void nextState() {
    StateMachine::nextState();

    switch (StateMachine::currentState) {
        case STATE_IDLE:
            AudioTaskState::stopPlayback();
            LEDTaskState::ledState = LED_OFF;
            break;
        case STATE_READY:
            LEDTaskState::ledState = LED_READY;
            break;
        case STATE_FIRE:
            AudioTaskState::startPlayback();
            LEDTaskState::ledState = LED_FIRE;
            break;
        default:
            break;
    }
}
