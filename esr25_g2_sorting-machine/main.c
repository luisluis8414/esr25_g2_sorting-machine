#include <msp430.h> 
#include "I2C/I2C.h"
#include "intrinsics.h"
#include "msp430fr2355.h"
#include "TCS34725/TCS.h"


#define LED_PIN     BIT0

char data;

/**
 * main.c
 */
 void delay_ms(unsigned int ms)
{
    while (ms--)
    {
        __delay_cycles(1000); // Bei 1 MHz = ~1ms
    }
}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;

	I2C_init();

	data = tcs_read_reg(0x12);
	__no_operation();

	


	return 0;
}

