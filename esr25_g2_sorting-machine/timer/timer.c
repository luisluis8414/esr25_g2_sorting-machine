#include "timer/timer.h"
#include <msp430.h>

uint16_t guiSysTickCnt = 0;
static uint16_t muiSysTickPer_ms = 1000;
static uint16_t muiUpCnt = 0x7FFF;
static volatile bool timer1_done = false;

void timer_init(void)
{
    // Timer_B0 für System-Tick konfigurieren
    TB0CTL = TBSSEL__ACLK | MC__STOP | TBCLR; // ACLK, stoppen, löschen
    // TB0CCTL0 = CCIE;                            // CCR0 Interrupt aktivieren
    guiSysTickCnt = 0;

    // Timer_B1 für Sleep-Funktionalität konfigurieren
    TB1CTL = TBSSEL__ACLK | ID__2 | MC__STOP | TBCLR; // ACLK, stoppen, löschen
    TB1CCTL0 = CCIE;                                  // CCR0 Interrupt aktivieren
}

void timer_sleep_ms(uint16_t sleep_ms)
{
    if (sleep_ms > 3998)
        sleep_ms = 3998;

    // Millisekunden in Timer-Ticks umwandeln (ACLK = 32.768 Hz)
    // Berechnung: ticks = (ms × 32.768) / 1000
    uint32_t ticks32 = (uint32_t)sleep_ms * 16384UL;
    uint16_t timer_count = (uint16_t)(ticks32 / 1000UL);

    if (timer_count == 0)
        timer_count = 1;


    TB1CCR0 = timer_count; // Timer_B1 verwenden
    TB1CTL |= MC__UP;      // Timer_B1 starten

    timer1_done = false;

     while (!timer1_done)
     {
        LPM3;                  // Schlafen bis CCR0 ISR
     }
    
    TB1CTL &= ~MC__UP;     // Timer_B1 stoppen
    TB1CTL |= MC__STOP;
}

void timer_systick_init(uint32_t period_ms)
{
    muiSysTickPer_ms = period_ms;
    guiSysTickCnt = 0;
    // Berechnung für System-Tick (ACLK = 32.768 Hz)
    uint32_t ulUpCnt = ((period_ms * 32768UL) / 1000UL);
    muiUpCnt = (uint16_t)(ulUpCnt - 1);
}

void timer_systick_start(void)
{
    guiSysTickCnt = 0;
    TB0CCR0 = muiUpCnt;
    TB0CTL = TBSSEL__ACLK | MC__UP | TBCLR | TBIE; // Timer_B0 für System-Tick
}

void timer_systick_stop(void)
{
    TB0CTL &= ~MC__UP;
    TB0CTL |= MC__STOP;
}

void timer_systick_sleep(uint32_t sleep_ms)
{
    uint16_t pause_cnt = sleep_ms / muiSysTickPer_ms;
    while (pause_cnt > 0)
    {
        pause_cnt--;
        LPM3;
    }
}

/* ========================================================================== */
/* Interrupt Service Routines                                                 */
/* ========================================================================== */

/**
 * @brief Timer_B1 CCR0 Interrupt Service Routine für Sleep-Funktionalität.
 */
#pragma vector = TIMER1_B0_VECTOR
__interrupt void TIMER1_B0_ISR(void)
{   
    timer1_done = true;
    __bic_SR_register_on_exit(LPM3_bits); // LPM3 verlassen
}