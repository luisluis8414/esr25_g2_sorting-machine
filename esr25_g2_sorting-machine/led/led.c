/* ========================================================================== */
/* led.c                                                                        */
/* ========================================================================== */
/**
 * @file      led.c
 * @author    wehrberger
 * @date      29.06.2025
 *
 * @brief     LED Steuerung für die Sortieranlage.
 *
 * Dieses Modul implementiert die Steuerung der Status-LEDs:
 *   - Rote LED (P6.6)   - Zeigt Bereitschaft zum Sortieren an
 *   - Grüne LED (P1.0)  - Zeigt aktiven Sortiervorgang an
 */

#include "led.h"
#include <msp430.h>

void led_init(void)
{
    // P1.0 (grüne LED) als Ausgang für Sortiervorgang-Anzeige
    P1DIR |= BIT0;
    P1OUT &= ~BIT0; // Initial ausgeschaltet

    // P6.6 (rote LED) als Ausgang für Bereitschaftsanzeige
    P6DIR |= BIT6;
    P6OUT &= ~BIT6; // Initial ausgeschaltet
}

void led_ready_on(void)
{
    P6OUT |= BIT6; // Rote LED einschalten für Bereitschaft
}

void led_ready_off(void)
{
    P6OUT &= ~BIT6; // Rote LED ausschalten
}

void led_sorting_on(void)
{
    P1OUT |= BIT0; // Grüne LED einschalten für Sortiervorgang
}

void led_sorting_off(void)
{
    P1OUT &= ~BIT0; // Grüne LED ausschalten
}
