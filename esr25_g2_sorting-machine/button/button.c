/*
 * button.c
 *
 *  Created on: 22.06.2025
 *      Author: raachl
 */

#include "button.h"
#include "../state_machine/state_machine.h"
#include <msp430fr2355.h>
#include <stdbool.h>

extern Event_t eventBits;

static volatile bool debounce_active = false;

void button_init(void)
{
    // --- P4.1 (Button1) konfigurieren ---
    P4DIR &= ~BIT1;
    P4OUT |= BIT1;
    P4REN |= BIT1;
    P4IES |= BIT1;
    P4IFG &= ~BIT1;
    P4IE  |= BIT1;

    // --- P2.3 (Button2) konfigurieren ---
    P2DIR &= ~BIT3;
    P2OUT |= BIT3;
    P2REN |= BIT3;
    P2IES |= BIT3;
    P2IFG &= ~BIT3;
    P2IE  |= BIT3;

    // Timer_B2 vorbereiten (für ACLK-Takt)
    TB2CTL = TBSSEL__ACLK | MC__STOP | TBCLR;
    TB2CCTL0 = 0;
}

inline void button_debounce_start(void)
{
    if (debounce_active) return;

    debounce_active = true;

    TB2CCR0 = 16384 - 1;                    // 500 ms
    TB2CCTL0 = CCIE;                        // Interrupt aktivieren
    TB2CTL = TBSSEL__ACLK | MC__UP | TBCLR; // ACLK, Up mode, clear

    P4IE &= ~BIT1;
    P2IE &= ~BIT3;
}

// ISR für Port 4
#pragma vector = PORT4_VECTOR
__interrupt void Port_4_ISR(void)
{
    if (!debounce_active) {
        eventBits |= EVT_S1;
        button_debounce_start();
        _bic_SR_register_on_exit(LPM3_bits);
    }
        P4IFG &= ~BIT1;
}

// ISR für Port 2
#pragma vector = PORT2_VECTOR
__interrupt void Port_2_ISR(void)
{
    if (!debounce_active) {
        eventBits |= EVT_S2;
        button_debounce_start();
        _bic_SR_register_on_exit(LPM3_bits);
    }
        P2IFG &= ~BIT3;
}

// ISR für Timer_B2 CCR0
#pragma vector = TIMER2_B0_VECTOR
__interrupt void TimerB2_ISR(void)
{
    TB2CTL = MC__STOP;
    TB2CCTL0 &= ~CCIE;

    debounce_active = false;

    P4IFG &= ~BIT1;
    P4IE  |= BIT1;
    P2IFG &= ~BIT3;
    P2IE  |= BIT3;
}
