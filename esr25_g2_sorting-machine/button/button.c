/*
 * button.c
 *
 *  Created on: 22.06.2025
 *      Author: raachl
 */

#include "button.h"
#include "../state_machine/state_machine.h"

extern Event_t eventBits;

void button_init(void)
{
    // --- P4.1 (Button1) konfigurieren ---
    P4DIR &= ~BIT1;   // Pin als Eingang
    P4OUT |= BIT1;    // Pull-Up aktivieren
    P4REN |= BIT1;    // Pull-Widerstand aktivieren

    P4IES |= BIT1;    // Interrupt bei fallender Flanke (HIGH -> LOW)
    P4IFG &= ~BIT1;   // Interrupt-Flag löschen
    P4IE  |= BIT1;    // Interrupt aktivieren
        // --- P4.1 (Button1) konfigurieren ---
    P2DIR &= ~BIT3;   // Pin als Eingang
    P2OUT |= BIT3;    // Pull-Up aktivieren
    P2REN |= BIT3;    // Pull-Widerstand aktivieren

    P2IES |= BIT3;    // Interrupt bei fallender Flanke (HIGH -> LOW)
    P2IFG &= ~BIT3;   // Interrupt-Flag löschen
    P2IE  |= BIT3;    // Interrupt aktivieren

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    P2IFG &= ~BIT3;                 // P2.3 IFG cleared
    P4IFG &= ~BIT1;                 // P4.1 IFG cleared

}

// --- ISR für Port 4 ---
#pragma vector = PORT4_VECTOR
__interrupt void Port_4_ISR(void)
{
    eventBits |= EVT_S1;
    P4IFG &= ~BIT1;                         // P4.1 IFG cleared
    _bic_SR_register_on_exit(LPM3_bits);
}

// --- ISR für Port 2 ---
#pragma vector = PORT2_VECTOR
__interrupt void Port_2_ISR(void)
{
    eventBits |= EVT_S2;
    P2IFG &= ~BIT3;                         // P2.3 IFG cleared
    _bic_SR_register_on_exit(LPM3_bits);
}
