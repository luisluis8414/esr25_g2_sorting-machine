/* ========================================================================== */
/* timer.h                                                                    */
/* ========================================================================== */
/**
 * @file      timer.h
 * @author    wehrberger
 * @date      10.06.2025
 *
 * @brief     Timer-Modul für präzise Timingfunktionen.
 *
 * Dieses Modul bietet Timer basierte Verzögerungsfunktionen.
 * Es verwendet Timer_B0 mit ACLK (32.768 Hz Kristall) als Taktquelle und
 * versetzt den Mikrocontroller in LPM3 während der Verzögerung.
 *
 * Technische Details:
 *   - Taktquelle: ACLK (32.768 Hz)
 *   - Divider: ÷2 → 16.384 Hz Timer-Frequenz
 *   - Auflösung: ~0,061 ms pro Tick
 *   - Maximale Verzögerung: ~3998 ms (16-Bit Timer)
 *
 * Verfügbare Funktionen:
 *   - timer_init()      – Timer-Initialisierung
 *   - timer_sleep_ms()  – Blockierende Verzögerung in Millisekunden
 *
 * @note Der Timer muss vor der ersten Verwendung mit timer_init() 
 *       initialisiert werden.
 */

#ifndef TIMER_TIMER_H_
#define TIMER_TIMER_H_

#include <stdint.h>

/**
 * @brief Initialisiert das Timer-Modul.
 *
 * Konfiguriert Timer_B0 für den Einsatz in Verzögerungsfunktionen:
 *   - Taktquelle: ACLK (32.768 Hz Kristall)
 *   - Divider: ÷2 für 16.384 Hz Timer-Frequenz
 *   - CCR0-Interrupt aktiviert für Aufwecken aus LPM3
 *
 * @note Diese Funktion muss vor der Verwendung anderer Timer Funktionen
 *       aufgerufen werden.
 */
void timer_init(void);

/**
 * @brief Blockiert die Programmausführung für eine bestimmte Zeit.
 *
 * Versetzt den MSP430 in den LPM3 für die angegebene Dauer.
 *
 * @param[in] sleep_ms Verzögerungszeit in ms (1-3998).
 *
 * @note Maximale Verzögerungszeit beträgt etwa 3998 ms aufgrund der
 *       16-Bit.
 *
 * @warning Die tatsächliche Verzögerung kann aufgrund der Timer-Auflösung
 *          um ±0,061 ms abweichen.
 */
void timer_sleep_ms(uint16_t sleep_ms);

#endif /* TIMER_TIMER_H_ */