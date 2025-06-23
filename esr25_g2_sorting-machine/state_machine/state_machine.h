/*
 * state_machine_h
 *
 *  Created on: 22.06.2025
 *      Author: raachl
 */
#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

#include <stdint.h>

#define EVT_NO_EVENT    0x00
#define EVT_S1          BIT1
#define EVT_S2          BIT2

/* Anzahl der states kann erweitert werden. Fr jeden state muss festgelegt werden, wie
 * auf die auftretenden events reagiert werden soll.
 */
typedef enum {
    OFF_STATE,
    DISPLAY_STATE,
    ON_STATE,
} State_t;

typedef uint16_t Event_t;

Event_t getEvent (Event_t *event);
void handleEvent_FSM(State_t *currentState, Event_t event);
void idleTask();

#endif /* STATE_MACHINE_H_ */
