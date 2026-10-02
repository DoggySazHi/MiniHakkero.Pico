#ifndef CHARGE_H
#define CHARGE_H

#define CHARGE_INPUT_THRESHOLD 500

void ChargeTask(void *pvParameters);
float toVoltage(int adcValue);

enum ChargeState {
    CHARGE_NOT_CHARGING,
    CHARGE_CHARGING,
    CHARGE_FULLY_CHARGED,
};

class ChargeTaskState {
public:
    static ChargeState chargeState;
    static int batterySenseReading;
    static int statSenseReading;
    static int usbSenseReading;
};

#endif // CHARGE_H