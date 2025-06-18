/*
 * timer.h
 *
 *  Created on: 10.06.2025
 *      Author: wehrberger
 */

#ifndef timer_timer_H_
#define timer_timer_H_

#include <stdint.h>

/**
 * @brief Initializes the timer.
 * 
 * Configures Timer_B0 for use in sleep mode. This function must be called 
 * before using any other timer-related functions.
 */
void timer_init(void);

/**
 * @brief Blocks execution for a specified time in milliseconds.
 * 
 * Puts the MSP430 into low-power mode (LPM3) for the specified duration. 
 * The maximum sleep time is approximately 15999 ms.
 * 
 * @param sleep_ms The time in milliseconds for which the MSP430 should sleep.
 */
void timer_sleep_ms(uint16_t sleep_ms);

#endif /* timer_timer_H_ */
