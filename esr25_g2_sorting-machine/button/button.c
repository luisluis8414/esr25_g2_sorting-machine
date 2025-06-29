/* ========================================================================== */
/* button.c                                                                      */
/* ========================================================================== */
/**
 * @file      button.c
 * @author    raachl
 * @date      22.06.2025
 *
 * @brief     Button Implementierung für die Sortieranlage.
 *
 * Dieses Modul implementiert die ISRs für die Buttons
 * und das Debouncing mittels Timer. Die Buttons generieren Events für die
 * State Machine wenn sie gedrückt werden.
 */

#include "button.h"
#include "../state_machine/state_machine.h"
#include <msp430fr2355.h>
#include <stdbool.h>

extern Event_t eventBits;

/** @brief Status Flag für aktives Debouncing */
static volatile bool debounce_active = false;

/**
 * @brief Initialisiert die Button Hardware.
 *
 * Konfiguriert die GPIO Pins für beide Buttons:
 *   - P4.1 (Button 1): Input mit Pull-up und Interrupt
 *   - P2.3 (Button 2): Input mit Pull-up und Interrupt
 *
 * Bereitet auch Timer B2 für das Debouncing vor.
 */
void button_init(void)
{
    // --- P4.1 (Button1) konfigurieren ---
    P4DIR &= ~BIT1;
    P4OUT |= BIT1;
    P4REN |= BIT1;
    P4IES |= BIT1;
    P4IFG &= ~BIT1;
    P4IE |= BIT1;

    // --- P2.3 (Button2) konfigurieren ---
    P2DIR &= ~BIT3;
    P2OUT |= BIT3;
    P2REN |= BIT3;
    P2IES |= BIT3;
    P2IFG &= ~BIT3;
    P2IE |= BIT3;

    // Timer B2 vorbereiten (für ACLK Takt)
    TB2CTL = TBSSEL__ACLK | MC__STOP | TBCLR;
    TB2CCTL0 = 0;
}

/**
 * @brief Startet den Debounce Timer.
 *
 * Startet Timer B2 für 500ms Debouncing Zeit und deaktiviert
 * die Button Interrupts während dieser Zeit. Verhindert mehrfaches
 * Starten des Timers.
 */
inline void button_debounce_start(void)
{
    if (debounce_active)
        return;

    debounce_active = true;

    TB2CCR0 = 16384 - 1;                    // 500 ms
    TB2CCTL0 = CCIE;                        // Interrupt aktivieren
    TB2CTL = TBSSEL__ACLK | MC__UP | TBCLR; // ACLK, Up mode, clear

    P4IE &= ~BIT1;
    P2IE &= ~BIT3;
}

/**
 * @brief Interrupt Service Routine für Port 4 (Button 1).
 *
 * Wird aufgerufen wenn Button 1 gedrückt wird. Setzt das entsprechende
 * Event Bit und startet das Debouncing.
 */
#pragma vector = PORT4_VECTOR
__interrupt void Port_4_ISR(void)
{
    if (!debounce_active)
    {
        eventBits |= EVT_S1;
        button_debounce_start();
        _bic_SR_register_on_exit(LPM3_bits);
    }
    P4IFG &= ~BIT1;
}

/**
 * @brief Interrupt Service Routine für Port 2 (Button 2).
 *
 * Wird aufgerufen wenn Button 2 gedrückt wird. Setzt das entsprechende
 * Event Bit und startet das Debouncing.
 */
#pragma vector = PORT2_VECTOR
__interrupt void Port_2_ISR(void)
{
    if (!debounce_active)
    {
        eventBits |= EVT_S2;
        button_debounce_start();
        _bic_SR_register_on_exit(LPM3_bits);
    }
    P2IFG &= ~BIT3;
}

/**
 * @brief Timer B2 Interrupt Service Routine für Debouncing.
 *
 * Wird nach 500ms aufgerufen um das Debouncing zu beenden.
 * Stoppt den Timer, deaktiviert den Timer Interrupt und
 * reaktiviert die Button Interrupts.
 */
#pragma vector = TIMER2_B0_VECTOR
__interrupt void TimerB2_ISR(void)
{
    TB2CTL = MC__STOP;
    TB2CCTL0 &= ~CCIE;

    debounce_active = false;

    P4IFG &= ~BIT1;
    P4IE |= BIT1;
    P2IFG &= ~BIT3;
    P2IE |= BIT3;
}
