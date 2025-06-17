/*
 * timer.c
 *
 *  Created on: 10.06.2025
 *      Author: wehrberger
 */

#include "timer/timer.h"
#include <msp430.h> 

/**
 * @brief Initialisiert den Timer. Konfiguriert Timer_B0 für den Schlafmodus.
 * @param [in] void
 * @return void
 */
void timer_init(void) {
    // Timer_B0 konfigurieren
    TB0CTL = TBSSEL__ACLK | MC__STOP | TBCLR; // ACLK, Stopp-Modus, Timer löschen
    TB0CCTL0 = CCIE;                          // Interrupt für CCR0 aktivieren
    TB0EX0 = TBIDEX_7;                        // Prescaler von 8 (ACLK / 8)
}

/**
 * @brief Blockiert die Ausführung für eine bestimmte Zeit in Millisekunden.
 *        Der MSP430 wird in LPM3 versetzt, bis die Zeit abgelaufen ist. 
 *        Maximale Zeit = 65535 x 0.244ms = ca, 15999ms 
 * @param [in] sleep_ms - Zeit in Millisekunden, für die der MSP schlafen soll.
 * @return void
 */
void timer_sleep_ms(uint16_t sleep_ms) {
    // Berechnung der Zählgrenze basierend auf ACLK (32.768 Hz) und Prescaler von 8
    // bit shift um division durch 1000 zu ersetzen
    const uint16_t timer_count = (uint16_t)((sleep_ms * 4096UL) >> 10);

    // Zählgrenze setzen
    TB0CCR0 = timer_count;

    // Timer starten im Up-Modus
    TB0CTL |= MC__UP;

    // In LPM3 wechseln, bis der Timer-Interrupt ausgelöst wird
    LPM3;

    // Nach dem Aufwachen: Timer stoppen
    TB0CTL &= ~MC__UP;
    TB0CTL |= MC__STOP;
}

/**
 * @brief Timer_B0 Interrupt Service Routine
 *        Wird ausgelöst, wenn der Timer die Zählgrenze erreicht.
 */
#pragma vector = TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR(void) {
    __bic_SR_register_on_exit(LPM3_bits);
}
