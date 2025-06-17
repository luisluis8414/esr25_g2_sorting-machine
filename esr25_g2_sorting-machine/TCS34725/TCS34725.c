#include "TCS34725.h"
#include "I2C/I2C.h"
#include <msp430.h>
#include <math.h>

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
}

uint16_t read_16bit_reg(uint8_t reg)
{
    uint8_t low = I2C_read_reg(TCS34725_ADDRESS, TCS34725_COMMAND_BIT | reg);
    uint8_t high = I2C_read_reg(TCS34725_ADDRESS, TCS34725_COMMAND_BIT | (reg + 1));
    return ((uint16_t)high << 8) | low;
}

void tcs_read_rgbc(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c)
{
    *c = read_16bit_reg(TCS34725_CDATAL);
    *r = read_16bit_reg(TCS34725_RDATAL);
    *g = read_16bit_reg(TCS34725_GDATAL);
    *b = read_16bit_reg(TCS34725_BDATAL);
}

void tcs_get_rgb888(uint8_t *r8, uint8_t *g8, uint8_t *b8)
{
    uint16_t r, g, b, c;
    tcs_read_rgbc(&r, &g, &b, &c);

    if (!c) {                    // sensor is dark or covered
        *r8 = *g8 = *b8 = 0;
        return;
    }

    /* Normalise to the clear channel and apply white-balance factors */
    float rf = (r * CAL_R) / (float)c;
    float gf = (g * CAL_G) / (float)c;
    float bf = (b * CAL_B) / (float)c;

    /* Put the strongest channel at 255 and scale the others the same way */
    float scale = 255.0f / fmaxf(fmaxf(rf, gf), bf);
    *r8 = (uint8_t)(rf * scale + 0.5f);
    *g8 = (uint8_t)(gf * scale + 0.5f);
    *b8 = (uint8_t)(bf * scale + 0.5f); 
}
    
