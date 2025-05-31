#ifndef I2C_H
#define I2C_H

#include <stdint.h>    

void init_I2C(void);

void write_I2C(uint8_t slave_addr, char data[], int length);

void read_I2C(uint8_t slave_addr, uint8_t reg_addr);

#endif 
