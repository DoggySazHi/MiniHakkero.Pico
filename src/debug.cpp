#include "debug.h"

#include <FreeRTOS.h>
#include "task.h"

#include "state.h"
#include "charge.h"
#include "button.h"

void DebugTask(void *pvParameters) {
    Serial.begin(115200);

    while (!Serial) {
        vTaskDelay(pdMS_TO_TICKS(10)); // Wait for serial to be ready
    }

    vTaskDelay(pdMS_TO_TICKS(1000));

    Serial.println("Debug task started");

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1000 milliseconds (1 second)
        Serial.printf("\n\nTime: %d\n",
            millis());
        Serial.printf("- Hakkero State: %d\n",
            StateMachine::currentState);
        Serial.printf("- Charge State: %d (%f, %f, %f)\n",
            ChargeTaskState::chargeState,
            toVoltage(ChargeTaskState::batterySenseReading),
            toVoltage(ChargeTaskState::statSenseReading),
            toVoltage(ChargeTaskState::usbSenseReading)); 
        Serial.printf("- Button State: %d %d %d %d\n",
            ButtonTaskState::buttons[0],
            ButtonTaskState::buttons[1],
            ButtonTaskState::buttons[2],
            ButtonTaskState::buttons[3]);
        Serial.printf("- Touch (ticks): %d/%d (base %d, press >%d), %d/%d (base %d, press >%d)\n",
            ButtonTaskState::touchReading[0],
            ButtonTaskState::touchReleaseThreshold[0],
            ButtonTaskState::touchBaseline[0],
            ButtonTaskState::touchPressThreshold[0],
            ButtonTaskState::touchReading[1],
            ButtonTaskState::touchReleaseThreshold[1],
            ButtonTaskState::touchBaseline[1],
            ButtonTaskState::touchPressThreshold[1]);
    }
}