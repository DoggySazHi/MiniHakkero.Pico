#ifndef STATE_H
#define STATE_H

void StateTask(void *pvParameters);
void nextState();

enum MiniHakkeroState {
    STATE_IDLE,
    STATE_READY,
    STATE_FIRE,
    STATE_NO_OF_STATES // Used to determine the number of states in the enum
};

class StateMachine {
public:
    static MiniHakkeroState currentState;

    static bool buttonHeld;

    static void nextState() {
        currentState = static_cast<MiniHakkeroState>((currentState + 1) % STATE_NO_OF_STATES);
    }
};

#endif // STATE_H