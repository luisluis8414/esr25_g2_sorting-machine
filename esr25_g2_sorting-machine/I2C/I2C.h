/*
 * I2C.h
 *
 *  Created on: 10.06.2025
 *      Author: wehrberger
 */

#ifndef I2C_I2C_H_
#define I2C_I2C_H_ 

#include <stdint.h>

void I2C_init(void);
void I2C_write(uint8_t slave_addr, char data[], int length);
char I2C_read_reg(uint8_t slave_addr, uint8_t reg_addr);

#endif /* I2C_I2C_H_ */
