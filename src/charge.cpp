#include "charge.h"

#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#include "constants.h"

ChargeState ChargeTaskState::chargeState = ChargeState::CHARGE_NOT_CHARGING;
int ChargeTaskState::batterySenseReading = 0;
int ChargeTaskState::statSenseReading = 0;
int ChargeTaskState::usbSenseReading = 0;

void ChargeTask(void *pvParameters) {
    analogReadResolution(12); // 0 - 4095 (3.3v), note we have a 100k/100k voltage divider

    while (1) {
        ChargeTaskState::batterySenseReading = analogRead(BAT_SENSE);
        ChargeTaskState::statSenseReading = analogRead(STAT_SENSE);
        ChargeTaskState::usbSenseReading = analogRead(USB_SENSE);

        bool usbConnected = (ChargeTaskState::usbSenseReading > CHARGE_INPUT_THRESHOLD);
        bool batteryCharging = (ChargeTaskState::statSenseReading > CHARGE_INPUT_THRESHOLD);

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

float toVoltage(int adcValue) {
    return (adcValue * 3.3) / 4095.0 * 2.0;
}