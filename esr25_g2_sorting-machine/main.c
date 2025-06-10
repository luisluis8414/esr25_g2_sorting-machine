#include <msp430.h> 
#include "PCA9685/PCA9685.h"
#include <stdint.h>
#include "platform/platform.h"
#include "I2C/I2C.h"

void init() {
    I2C_init();
	PCA9685_init();
}

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;  

    init();

    while (1) {
    plattform_default_position();

    __delay_cycles(2000000);

    plattform_empty_r();

    __delay_cycles(2000000);

    plattform_default_position();

    __delay_cycles(2000000);

    plattform_empty_g();

    __delay_cycles(2000000);

    plattform_default_position();

    __delay_cycles(2000000);

    plattform_empty_b();

    __delay_cycles(2000000);

    plattform_default_position();

    __delay_cycles(2000000);
    }
}
