#ifndef CHARGE_H
#define CHARGE_H

#define CHARGE_INPUT_THRESHOLD 1000

void ChargeTask(void *pvParameters);

enum ChargeState {
    CHARGE_NOT_CHARGING,
    CHARGE_CHARGING,
    CHARGE_FULLY_CHARGED,
};

class ChargeTaskState {
public:
    static ChargeState chargeState;
};

#endif // CHARGE_H