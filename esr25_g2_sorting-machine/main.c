#include <msp430.h> 
#include "PCA9685/PCA9685.h"
#include <stdint.h>
#include "platform/platform.h"
#include "I2C/I2C.h"
#include "timer/timer.h"
#include "TCS34725/TCS34725.h"

volatile uint16_t clear_ref     = 0;   // Default-Schwelle beim Start
volatile uint8_t  flag_calib    = 0;      // wird in ISR gesetzt
volatile uint16_t MIN_DELTA_CLR = 0;    // wie viel dunkler es mindestens sein muss

static void calibrate_clear(void)
{
    uint16_t c;
    TCS_single_shot_sleep(&c);   // LED aus – Umgebungslicht-Referenz
    clear_ref = c;
    MIN_DELTA_CLR = c * 0.5f;
}

void init()
{
    I2C_init();
    PCA9685_init();
    timer_init();
    TCS_init();

    calibrate_clear();

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
        if (flag_calib) {                    
            flag_calib = 0;
            calibrate_clear();               
            continue;                       
        }

        uint16_t clear;
        TCS_single_shot_sleep(&clear);

        if (clear + MIN_DELTA_CLR < clear_ref) {
            do_sort();
        }

        timer_sleep_ms(1000); 
    }
}

// --- ISR only wakes CPU ---
#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR(void)
{
    if (P2IFG & BIT3)
    {
        P2IFG &= ~BIT3;                     // clear flag
        flag_calib = 1;  
        __bic_SR_register_on_exit(LPM3_bits); // wake main loop
    }
}
