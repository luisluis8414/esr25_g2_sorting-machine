/*
 * button.h
 *
 *  Created on: 22.06.2025
 *      Author: raachl
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include <msp430.h>
#include <stdint.h>

// Event type definition 
typedef uint16_t Event_t;

// Button-Pins definieren
#define BUTTON1_PORT P4IN
#define BUTTON1_PIN BIT1 // P4.1
#define BUTTON2_PORT P2IN
#define BUTTON2_PIN BIT1 // P2.1

void button_init(void);
inline void button_debounce_start(void);

#endif /* BUTTON_H_ */
