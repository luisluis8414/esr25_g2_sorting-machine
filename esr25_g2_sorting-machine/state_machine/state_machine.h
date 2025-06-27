#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

#include <stdint.h>
#include <stdbool.h>

// Events
#define EVT_NO_EVENT 0x00
#define EVT_SYSTEM_TICK BIT0
#define EVT_S1 BIT1
#define EVT_S2 BIT2
#define EVT_OBJECT_DETECTED BIT3

// States
typedef enum
{
    OFF_STATE,
    MODE_SELECTION_STATE,
    AUTO_SORT_STATE,
    MANUAL_SORT_STATE,
    DISPLAY_STATE
} State_t;

typedef uint16_t Event_t;

extern Event_t eventBits;

void handleEvent_FSM(State_t *currentState, Event_t event);
Event_t getEvent(Event_t *event);

#endif /* STATE_MACHINE_H_ */
