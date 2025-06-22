/*
 * lcd1602_manager.h
 *
 *  Created on: 22.06.2025
 *      Author: raachl
 */

#ifndef LCD1602_MANAGER_H_
#define LCD1602_MANAGER_H_

#include <stdint.h>
#include <stdbool.h>
typedef enum {
    RED,
    BLUE,
    GREEN,
    UNKNOWN
} COLOR;

void writeReady(void);
void writeCurrentCount(uint8_t current_count_all, uint8_t current_count_blue, 
uint8_t current_count_green, uint8_t current_count_red);
void writeDetectedColor(COLOR color);
void clearDisplayAndBacklightOff(void);

#endif /* LCD1602_MANAGER_H_ */
