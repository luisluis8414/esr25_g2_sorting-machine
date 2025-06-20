/* ========================================================================== */
/* timer.c                                                                    */
/* ========================================================================== */
/**
 * @file      timer.c
 * @author    wehrberger
 * @date      10.06.2025
 *
 * @brief     Implementierung des Timer-Moduls.
 */

#include "timer/timer.h"
#include <msp430.h>

void timer_init(void)
{
    // Timer_B0 für Sleep-Modus konfigurieren
    // Taktquelle: ACLK (32.768 Hz Kristall)
    // Vorteiler: ÷2 → 16.384 Hz Timer-Takt
    // Ein Tick: 0,061035 ms
    TB0CTL   = TBSSEL__ACLK | MC__STOP | TBCLR; // ACLK, stoppen, löschen
    TB0CCTL0 = CCIE;                            // CCR0 Interrupt aktivieren
    TB0EX0   = TBIDEX_1;                        // Extra-Teiler = 2
}

void timer_sleep_ms(uint16_t sleep_ms)
{
    // Millisekunden in Timer-Ticks umwandeln (ACLK / 2 = 16.384 Hz)
    // Berechnung: ticks = (ms · 16.384) / 1000
    uint32_t ticks32      = (uint32_t)sleep_ms * 16384UL;
    uint16_t timer_count  = (uint16_t)(ticks32 / 1000UL);

    if (timer_count == 0)         timer_count = 1;      // Mindestens 1 Tick
    if (timer_count > 65535U)     timer_count = 65535U; // Auf 16-Bit begrenzen

    TB0CCR0 = timer_count;        // Compare-Register laden
    TB0CTL |= MC__UP;             // Timer starten
    LPM3;                         // Schlafen bis CCR0 ISR
    TB0CTL &= ~MC__UP;            // Timer stoppen
    TB0CTL |= MC__STOP;
}

/**
 * @brief Timer_B0 Interrupt Service Routine.
 *
 * Weckt die CPU aus LPM3 auf, wenn CCR0 erreicht wird.
 */
#pragma vector = TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR(void)
{
    __bic_SR_register_on_exit(LPM3_bits); // LPM3 verlassen
}