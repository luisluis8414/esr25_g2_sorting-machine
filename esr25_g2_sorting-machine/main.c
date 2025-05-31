#include <msp430.h> 
#include "PCA9685.h"
#include "I2C.h"
#include <stdint.h>

// TODO: Replace with timers or remove if not needed
void delay_ms(uint16_t ms) {
    while (ms--) {
        __delay_cycles(1000); // Assuming 1 MHz clock, 1000 cycles = 1 ms
    }
}

// TODO: Replace with timers or remove if not needed
void delay_s(uint16_t seconds) {
    while (seconds--) {
        delay_ms(1000); // 1000 ms = 1 second
    }
}

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;  

    init_I2C();
	init_PCA9685();

	while (1) {
		set_servo_angle(0, 0);

		delay_ms(2000);

		set_servo_angle(0, 180);

		delay_ms(2000);
    }
}
