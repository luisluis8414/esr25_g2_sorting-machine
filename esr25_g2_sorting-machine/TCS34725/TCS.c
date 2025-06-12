#include "tcs.h"
#include "I2C/I2C.h"
#include <msp430.h>

void tcs_init(void)
{
    // Integration Time: 100 ms
    char atime[] = { TCS34725_COMMAND_BIT | TCS34725_ATIME, 0xD6 };
    I2C_write(TCS34725_ADDRESS, atime, 2);

    // Gain: 4x
    char control[] = { TCS34725_COMMAND_BIT | TCS34725_CONTROL, 0x01 };
    I2C_write(TCS34725_ADDRESS, control, 2);

    // Sensor aktivieren: Power ON + ADC enable
    char enable[] = { TCS34725_COMMAND_BIT | TCS34725_ENABLE, 0x03 };
    I2C_write(TCS34725_ADDRESS, enable, 2);

    // Warte 100 ms für Aufwärmzeit
    __delay_cycles(100000); // ca. 100 ms bei 1 MHz
}

uint16_t tcs_read16(uint8_t reg)
{
    uint8_t low = I2C_read_reg(TCS34725_ADDRESS, TCS34725_COMMAND_BIT | reg);
    uint8_t high = I2C_read_reg(TCS34725_ADDRESS, TCS34725_COMMAND_BIT | (reg + 1));
    return ((uint16_t)high << 8) | low;
}

void tcs_read_rgbc(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c)
{
    *c = tcs_read16(TCS34725_CDATAL);
    *r = tcs_read16(TCS34725_RDATAL);
    *g = tcs_read16(TCS34725_GDATAL);
    *b = tcs_read16(TCS34725_BDATAL);
}
