/*
 * timer.c
 *
 *  Created on: 10.06.2025
 *      Author: wehrberger
 */

#include "timer/timer.h"
#include <msp430.h>

/**
 * @brief Initializes the timer.
 *
 * Configures Timer_B0 for use in sleep mode.
 * Clock source : ACLK  (32 768 Hz crystal)
 * Prescaler    : ÷ 2   → 16 384 Hz timer clock
 * One tick     : 0.061 035 ms
 */
void timer_init(void)
{
    TB0CTL   = TBSSEL__ACLK | MC__STOP | TBCLR; /* ACLK, stop, clear      */
    TB0CCTL0 = CCIE;                            /* Enable CCR0 interrupt  */
    TB0EX0   = TBIDEX_1;                        /* Extra divider = 2      */
}

/**
 * @brief Blocks execution for a specified time in milliseconds.
 *
 * Puts the MSP430 into LPM3 for the requested duration.
 * Conversion: ticks = (ms · 16 384) / 1000 
 * Max single delay with 16-bit timer and ÷ 2 prescaler ≈ 3998 ms.
 *
 * @param sleep_ms  Delay in milliseconds 
 */
void timer_sleep_ms(uint16_t sleep_ms)
{
    /* Convert milliseconds to timer ticks (ACLK / 2 = 16 384 Hz) */
    uint32_t ticks32      = (uint32_t)sleep_ms * 16384UL;
    uint16_t timer_count  = (uint16_t)(ticks32 / 1000UL);

    if (timer_count == 0)         timer_count = 1;      /* ≥1 tick          */
    if (timer_count > 65535U)     timer_count = 65535U; /* clip to 16 bit   */

    TB0CCR0 = timer_count;        /* Load compare register                */
    TB0CTL |= MC__UP;             /* Start timer                          */
    LPM3;                         /* Sleep until CCR0 ISR                 */
    TB0CTL &= ~MC__UP;            /* Stop timer                           */
    TB0CTL |= MC__STOP;
}

/**
 * @brief Timer_B0 Interrupt Service Routine.
 *
 * Wakes the CPU from LPM3 when CCR0 is reached.
 */
#pragma vector = TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR(void)
{
    __bic_SR_register_on_exit(LPM3_bits); /* Exit LPM3 */
}
