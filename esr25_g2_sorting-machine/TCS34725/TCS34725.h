#ifndef TCS_H_
#define TCS_H_

#include <stdint.h>

// Sensoradresse
#define TCS34725_ADDRESS     0x29
#define TCS34725_COMMAND_BIT 0x80

// Registeradressen
#define TCS34725_ENABLE      0x00
#define TCS34725_ATIME       0x01
#define TCS34725_CONTROL     0x0F

#define TCS34725_CDATAL      0x14
#define TCS34725_RDATAL      0x16
#define TCS34725_GDATAL      0x18
#define TCS34725_BDATAL      0x1A

/* Calibration factors from a white reference (optional) */
#define CAL_R 1.00f
#define CAL_G 1.00f
#define CAL_B 1.00f

// Funktionen
void tcs_init(void);
uint16_t read_16bit_reg(uint8_t reg);
void tcs_read_rgbc(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c);
void tcs_get_rgb888(uint8_t *r8, uint8_t *g8, uint8_t *b8);

#endif /* TCS_H_ */
