#include <msp430.h>
#include "I2C/I2C.h"
#include "TCS34725/tcs.h"

uint16_t r = 0, g = 0, b = 0, c = 0;


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    I2C_init();    // Dein vorhandener Code
    tcs_init();    // Sensor initialisieren

    while (1)
    {
        tcs_read_rgbc(&r, &g, &b, &c);
        __no_operation();

    

        __delay_cycles(1000000); // Warte 1 Sekunde bei 1 MHz
    }
}
