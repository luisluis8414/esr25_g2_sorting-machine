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

// Der PCF8574 Baustein hat folgende 7-Bit Adresse: 0x3F (dez 63) : 0011 1111
// Somit ergibt sich f r Schreibzugriffe folgender Adresswert: 0111 1110 = 0x7E
// Die Driverlib erwartet jedoch die Angabe der tats chlichen 7-Bit Adresse, also 0x3F
#define SLAVE_ADDRESS_LCD 0x3F

typedef enum {
    eLCD1602_ok,
    eLCD1602_invalidLine
} lcd1602_res_t;

lcd1602_res_t lcd1602_init(void);
lcd1602_res_t lcd1602_write(uint16_t lines, char* text);
lcd1602_res_t lcd1602_clear(void);
lcd1602_res_t lcd1602_backlight(bool on);
bool          lcd1602_getBacklightState(void);
lcd1602_res_t lcd1602_display(bool on);

#endif /* LCD1602_H_ */
