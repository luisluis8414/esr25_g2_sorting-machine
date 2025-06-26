/*
 * lcd1602.h
 *
 *  Created on: 22.06.2025
 *      Author: raachl
 */

#ifndef LCD1602_H_
#define LCD1602_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    eLCD1602_ok,
    eLCD1602_invalidLine
} lcd1602_res_t;

lcd1602_res_t lcd1602_init(void);
lcd1602_res_t lcd1602_write(uint16_t lines, char* text);
lcd1602_res_t lcd1602_clear(void);
lcd1602_res_t lcd1602_backlight(bool on);
bool          lcd1602_getBacklightState(void);


#endif /* LCD1602_H_ */
