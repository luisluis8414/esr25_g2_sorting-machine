/* ========================================================================== */
/* button.h                                                                      */
/* ========================================================================== */
/**
 * @file      button.h
 * @author    raachl
 * @date      22.06.2025
 *
 * @brief     Button Handling für die Sortieranlage.
 *
 * Dieses Modul stellt die Funktionalität für die zwei Buttons der Sortieranlage
 * bereit. Es implementiert Debouncing und Event Generierung für die State Machine.
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include <msp430.h>
#include <stdint.h>

/**
 * @brief Initialisiert die Buttons.
 *
 * Konfiguriert die GPIO Pins für beide Buttons mit Pull-up und
 * aktiviert die Interrupts.
 */
void button_init(void);

/**
 * @brief Startet den Debounce Timer.
 *
 * Startet einen 500ms Timer für das Debouncing der Buttons und
 * deaktiviert die Button Interrupts während dieser Zeit.
 */
inline void button_debounce_start(void);

#endif /* BUTTON_H_ */
