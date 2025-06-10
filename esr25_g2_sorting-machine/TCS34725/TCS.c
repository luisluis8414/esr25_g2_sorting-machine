/*
 * TCS
 *
 *  Created on: 10.06.2025
 *      Author: Qunaku
 */

#include "msp430fr2355.h"
#include <stdint.h>
#include "TCS.h"
#include "../I2C/I2C.h"

char tcs_read_reg(uint8_t reg){
    uint8_t reg_addr = (reg & 0x1F) | 0x80;
    char data_in = I2C_read_reg(TCS_ADRESS, reg_addr);

    return data_in;
}


