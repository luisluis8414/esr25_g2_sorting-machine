#include <msp430.h> 
#include "PCA9685/PCA9685.h"
#include <stdint.h>
#include "platform/platform.h"
#include "I2C/I2C.h"
#include "timer/timer.h"
#include "TCS34725/TCS34725.h"

void init()
{
    I2C_init();
    PCA9685_init();
    timer_init();
    TCS_init();

    // --- Button P2.3 setup with interrupt ---
    P2DIR &= ~BIT3;     // input
    P2REN |= BIT3;      // enable pull-up/down
    P2OUT |= BIT3;      // pull-up

    P2IES |= BIT3;      // falling edge (press)
    P2IFG &= ~BIT3;     // clear flag
    P2IE |= BIT3;       // enable interrupt
}

void do_sort()
{
    uint8_t r, g, b;
    TCS_get_rgb888(&r, &g, &b);

    if (r > g && r > b) {
        plattform_empty_r();  // Red is dominant
    }
    else if (g > b) {
        plattform_empty_g();  // Green is dominant
    }
    else {
        plattform_empty_b();  // Blue is dominant
    }

}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    init();
    __enable_interrupt();

    plattform_default_position();
    while (1)
    {
        __bis_SR_register(LPM3_bits + GIE);  // sleep until interrupt

        timer_sleep_ms(1000); // wait for ob
        
        do_sort();

        timer_sleep_ms(1000);

        TCS_clear_int();
    }
}

// --- ISR only wakes CPU ---
#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR(void)
{
    if (P2IFG & BIT3)
    {
        P2IFG &= ~BIT3;                     // clear flag
        __bic_SR_register_on_exit(LPM3_bits); // wake main loop
    }
}
