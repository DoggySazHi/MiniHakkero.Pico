#include "charge.h"

#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "constants.h"

ChargeState ChargeTaskState::chargeState = ChargeState::CHARGE_NOT_CHARGING;

void ChargeTask(void *pvParameters) {
    while (1) {
        bool usbConnected = (analogRead(USB_SENSE) > CHARGE_INPUT_THRESHOLD);
        bool batteryCharging = (analogRead(BAT_SENSE) > CHARGE_INPUT_THRESHOLD);

        if (usbConnected) {
            if (batteryCharging) {
                ChargeTaskState::chargeState = ChargeState::CHARGE_CHARGING;
            } else {
                ChargeTaskState::chargeState = ChargeState::CHARGE_FULLY_CHARGED;
            }
        } else {
            ChargeTaskState::chargeState = ChargeState::CHARGE_NOT_CHARGING;
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // Delay to yield to other tasks
    }
}