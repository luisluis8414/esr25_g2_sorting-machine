/* ========================================================================== */
/* TCS34725.c                                                                 */
/* ========================================================================== */
/**
 * @file      TCS34725.c
 * @author    qunaku, wehrberger
 * @date      10.06.2025
 *
 * @brief     Implementierung des TCS34725 RGB-Farbsensor Treibers.
 */

#include "TCS34725.h"
#include "I2C/I2C.h"
#include <msp430.h>
#include <math.h>

void TCS_init(void)
{
    // Integrationszeit: 100ms (0xD6 = 214, Zeit = (256-214) * 2.4ms ≈ 100ms)
    char atime[] = { CMD(TCS34725_ATIME), 0xD6 };
    I2C_write(TCS34725_ADDRESS, atime, 2);

    // Verstärkung: 4x (0x01 entspricht 4x Verstärkung)
    char control[] = { CMD(TCS34725_CONTROL), 0x01 };
    I2C_write(TCS34725_ADDRESS, control, 2);

    // Sensor aktivieren: Power ON (Bit 0) + ADC Enable (Bit 1) = 0x03
    char enable[] = { CMD(TCS34725_ENABLE), 0x03 };
    I2C_write(TCS34725_ADDRESS, enable, 2);
}

uint16_t TCS_read_16bit_reg(uint8_t reg)
{
    uint8_t low = I2C_read_reg(TCS34725_ADDRESS, CMD(reg));
    uint8_t high = I2C_read_reg(TCS34725_ADDRESS, CMD(reg + 1));
    return ((uint16_t)high << 8) | low;  // Little-Endian Kombination
}

void TCS_read_rgbc(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c)
{
    *c = TCS_read_16bit_reg(TCS34725_CDATAL);
    *r = TCS_read_16bit_reg(TCS34725_RDATAL);
    *g = TCS_read_16bit_reg(TCS34725_GDATAL);
    *b = TCS_read_16bit_reg(TCS34725_BDATAL);
}

void TCS_get_rgb888(uint8_t *r8, uint8_t *g8, uint8_t *b8)
{
    uint16_t r, g, b, c;
    
    TCS_read_rgbc(&r, &g, &b, &c);

    if (!c) {
        *r8 = *g8 = *b8 = 0;
        return;
    }

    // Maximalwert finden um Skalierungsfaktor zu bestimmen
    uint16_t max_val = (r > g) ? r : g;
    max_val = (max_val > b) ? max_val : b;
    
    // Proportional herunterskalieren bis Maximalwert in 8-Bit Bereich passt
    while (max_val > 255) {
        r >>= 1;
        g >>= 1;
        b >>= 1;
        max_val >>= 1;
    }

    *r8 = r;
    *g8 = g;
    *b8 = b;
}