#include <msp430.h> 
#include "PCA9685.h"
#include <stdint.h>
#include "servos.h"
#include "utils.h"
#include "I2C/I2C.h"

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;  

    I2C_init();
	init_PCA9685();

    while (1) {
    enter_default_position();

    delay_s(2);

    empty_posi1();

    delay_s(2);

    enter_default_position();

    delay_s(2);

    empty_posi2();

    delay_s(2);

    enter_default_position();

    delay_s(2);

    empty_posi3();

    delay_s(2);

    enter_default_position();

    delay_s(2);
    }
}
