/*
 * timer.h
 *
 *  Created on: 10.06.2025
 *      Author: wehrberger
 */

#ifndef timer_timer_H_
#define timer_timer_H_ 

#include <stdint.h>

void timer_init(void);

void timer_sleep_ms(uint16_t sleep_ms);

#endif /* timer_timer_H_ */
