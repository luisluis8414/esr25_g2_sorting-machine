#include <msp430.h> 
#include "I2C/I2C.h"
#include "timer/timer.h"

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;

	timer_init();

	P1DIR |= BIT0;  // P1.0 als Ausgang
    P1OUT &= ~BIT0; // LED aus

	__bis_SR_register(GIE);

	while(1){
		P1OUT ^= BIT0; 

		timer_sleep_ms(10000);
	}	
}
