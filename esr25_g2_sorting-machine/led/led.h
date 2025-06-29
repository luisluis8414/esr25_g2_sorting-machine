/* ========================================================================== */
/* led.h                                                                        */
/* ========================================================================== */
/**
 * @file      led.h
 * @author    wehrberger
 * @date      29.06.2025
 *
 * @brief     LED Steuerung für die Sortieranlage.
 */

#ifndef LED_H_
#define LED_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Initialisiert die Status LEDs.
 *
 * Konfiguriert die GPIO Pins für beide LEDs als Ausgänge
 * und setzt sie initial auf ausgeschaltet:
 *   - P6.6 (rote LED)   - Bereitschaftsanzeige
 *   - P1.0 (grüne LED)  - Sortiervorgang-Anzeige
 */
void led_init(void);

/**
 * @brief Schaltet die rote Bereitschafts-LED ein.
 *
 * Aktiviert die grüne LED an P6.6 um anzuzeigen, dass die
 * Sortieranlage bereit ist, Objekte zu sortieren.
 */
void led_ready_on(void);

/**
 * @brief Schaltet die rote Bereitschafts-LED aus.
 *
 * Deaktiviert die grüne LED an P6.6 wenn die Sortieranlage
 * nicht mehr bereit ist zu sortieren.
 */
void led_ready_off(void);

/**
 * @brief Schaltet die rote Sortiervorgang-LED ein.
 *
 * Aktiviert die rote LED an P1.0 um anzuzeigen, dass die
 * Sortieranlage gerade einen Sortiervorgang durchführt.
 */
void led_sorting_on(void);

/**
 * @brief Schaltet die rote Sortiervorgang-LED aus.
 *
 * Deaktiviert die rote LED an P1.0 wenn der aktuelle
 * Sortiervorgang abgeschlossen ist.
 */
void led_sorting_off(void);

#endif /* LED_H_ */
