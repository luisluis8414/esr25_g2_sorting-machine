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

// Funktionen
void tcs_init(void);
uint16_t tcs_read16(uint8_t reg);
void tcs_read_rgbc(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c);

#endif /* TCS_H_ */
