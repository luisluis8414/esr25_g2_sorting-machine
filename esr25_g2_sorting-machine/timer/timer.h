/* ========================================================================== */
/* timer.h                                                                    */
/* ========================================================================== */
/**
 * @file      timer.h
 * @author    wehrberger
 * @date      10.06.2025
 *
 * @brief     Timer-Modul mit Sleep- und System-Tick-Funktionalität.
 *
 * Dieses Modul kombiniert Timer-basierte Verzögerungsfunktionen mit
 * System-Tick-Funktionalität für State-Machine-Anwendungen.
 * - Timer_B0: System-Tick (ACLK / 2 = 16.384 Hz)
 * - Timer_B1: Sleep-Funktionalität (ACLK / 2 = 16.384 Hz)
 */

#ifndef TIMER_TIMER_H_
#define TIMER_TIMER_H_

#include <stdint.h>
#include <stdbool.h>


/* ========================================================================== */
/* Globale Variablen                                                          */
/* ========================================================================== */

extern uint16_t guiSysTickCnt;
volatile bool TIMER_SLEEP_MODE;

/* ========================================================================== */
/* Timer Funktionen                                                           */
/* ========================================================================== */

/**
 * @brief Initialisiert beide Timer-Module.
 */
void timer_init(void);

/**
 * @brief Blockiert die Programmausführung für eine bestimmte Zeit.
 * 
 * Verwendet Timer_B1 und interferiert nicht mit dem System-Tick.
 * 
 * @param[in] sleep_ms Verzögerungszeit in ms (1-3998).
 */
void timer_sleep_ms(uint16_t sleep_ms);

/**
 * @brief Initialisiert den System-Tick mit gegebener Periode.
 * 
 * @param[in] period_ms Tick-Periode in Millisekunden.
 */
void timer_systick_init(uint32_t period_ms);

/**
 * @brief Startet den System-Tick auf Timer_B0.
 */
void timer_systick_start(void);

/**
 * @brief Stoppt den System-Tick.
 */
void timer_systick_stop(void);

/**
 * @brief Blockiert bis zur angegebenen Anzahl von System-Ticks.
 * 
 * @param[in] sleep_ms Wartezeit in Millisekunden.
 */
void timer_systick_sleep(uint32_t sleep_ms);

#endif /* TIMER_TIMER_H_ */
