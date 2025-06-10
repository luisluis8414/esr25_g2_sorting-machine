/*
 * I2C.h
 *
 *  Created on: 10.06.2025
 *      Author: qunaku
 */

#ifndef TCS_TCS_H_
#define TCS_TCS_H_ 

#include <stdint.h>

#define TCS_ADRESS  0x29

char tcs_read_reg(uint8_t reg);

#endif 