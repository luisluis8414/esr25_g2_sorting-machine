#include <msp430.h> 
#include <stdint.h>

#include "PCA9685/PCA9685.h"
#include "platform/platform.h"
#include "I2C/I2C.h"
#include "timer/timer.h"
#include "state_machine/state_machine.h"
extern Event_t eventBits;
void init() {
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;  
    // BUTTON CONFIG
    P2REN |= BIT3;                  // enable internal Resistor
    P2OUT |= BIT3;                  // configure internal Resistor as pullup Resistor
    P2IE  |= BIT3;                  // P2.3 Taster S2: interrupt enabled
    P2IES |= BIT3;                  // P2.3 Taster S2: interrupt with hi/lo edge

    P4REN |= BIT1;                  // enable internal Resistor
    P4OUT |= BIT1;                  // configure internal Resistor as pullup Resistor
    P4IE  |= BIT1;                  // P4.1 Taster S1: interrupt enabled
    P4IES |= BIT1;                  // P4.1 Taster S1: interrupt with hi/lo edge
    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    P2IFG &= ~BIT3;                 // P2.3 IFG cleared
    P4IFG &= ~BIT1;                 // P4.1 IFG cleared

    I2C_init();
	PCA9685_init();
    timer_init();
    __bis_SR_register(GIE);         // Global interrupt enable

}

/**
 * main.c
 */
int main(void)
{
    State_t currentState = OFF_STATE;
    Event_t event = EVT_NO_EVENT;

    init();

    while (1) {
        event = getEvent(&eventBits);
        if (event != EVT_NO_EVENT) {
            while (event > EVT_NO_EVENT) {
                handleEvent_FSM(&currentState, event);
                event = getEvent(&eventBits);
            }
            idleTask();
        }
        LPM3;
    
    /*plattform_default_position();

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
    */
    }
}
