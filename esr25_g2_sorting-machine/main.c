#include <msp430.h> 
#include "PCA9685/PCA9685.h"
#include <stdint.h>
#include "platform/platform.h"
#include "I2C/I2C.h"
#include "timer/timer.h"

void init() {
    I2C_init();
	PCA9685_init();
    timer_init();
}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;  

    init();

    while (1) {
    plattform_default_position();

    timer_sleep_ms(2000);

    plattform_empty_r();

    timer_sleep_ms(2000);

    plattform_default_position();

    timer_sleep_ms(2000);

    plattform_empty_g();

    timer_sleep_ms(2000);

    plattform_default_position();

    timer_sleep_ms(2000);

    plattform_empty_b();

    timer_sleep_ms(2000);

    plattform_default_position();

    timer_sleep_ms(2000);
    }
}
