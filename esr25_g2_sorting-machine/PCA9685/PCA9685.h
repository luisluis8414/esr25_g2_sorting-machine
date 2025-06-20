/* ========================================================================== */
/* PCA9685.h                                                                  */
/* ========================================================================== */
/**
 * @file      PCA9685.h
 * @author    wehrberger
 * @date      31.05.2025
 *
 * @brief     PCA9685 16-Kanal PWM-Treiber für Servo- und LED-Steuerung.
 *
 * Dieses Modul bietet eine einfache Schnittstelle zum PCA9685 16-Kanal
 * 12-Bit PWM-Treiber über I²C-Kommunikation. Der Treiber bietet grundlegende
 * Funktionalität für:
 *   - PCA9685_init()                – Initialisierung für 50 Hz PWM-Betrieb
 *   - PCA9685_set_servo_position()  – PWM-Position für Servo-Steuerung setzen
 *
 * Der Treiber ist optimiert für Servo-Anwendungen mit 50 Hz PWM-Frequenz
 * und bietet vordefinierte Konstanten für die Kippplatform.
 *
 * @note Dieser Treiber benötigt eine funktionsfähige I²C-Schnittstelle.
 */

#ifndef PCA9685_H
#define PCA9685_H

#include <stdint.h>

/* ========================================================================== */
/* Konstanten                                           */
/* ========================================================================== */

/** @brief I²C-Slave-Adresse des PCA9685-Moduls. */
#define PCA9685_ADDR      0x40

/* ========================================================================== */
/* PCA9685 Registeradressen                                                   */
/* ========================================================================== */

/** @brief LED0_ON_L Register – Kanal 0 Ein-Zeit Low-Byte. */
#define LED0_ON_L         0x06

/** @brief LED0_ON_H Register – Kanal 0 Ein-Zeit High-Byte. */
#define LED0_ON_H         0x07

/** @brief LED0_OFF_L Register – Kanal 0 Aus-Zeit Low-Byte. */
#define LED0_OFF_L        0x08

/** @brief LED0_OFF_H Register – Kanal 0 Aus-Zeit High-Byte. */
#define LED0_OFF_H        0x09

/* ========================================================================== */
/* Register makros                                                 */
/* ========================================================================== */

/** @brief Berechnet LED_ON_L Registeradresse für gegebenen Kanal. */
#define LED_ON_L(channel)  (LED0_ON_L + 4 * (channel))

/** @brief Berechnet LED_ON_H Registeradresse für gegebenen Kanal. */
#define LED_ON_H(channel)  (LED0_ON_H + 4 * (channel))

/** @brief Berechnet LED_OFF_L Registeradresse für gegebenen Kanal. */
#define LED_OFF_L(channel) (LED0_OFF_L + 4 * (channel))

/** @brief Berechnet LED_OFF_H Registeradresse für gegebenen Kanal. */
#define LED_OFF_H(channel) (LED0_OFF_H + 4 * (channel))

/* ========================================================================== */
/* Servo-Positionskonstanten                                                  */
/* ========================================================================== */

/** Konstanten sind optimiert für die Kippplatform

/** @brief PWM-Wert für 45° Servo-Position (≈ 1.23 ms Pulsweite). */
#define SERVO_DEG_PULSE_45  252

/** @brief PWM-Wert für 90° Servo-Position (≈ 1.78 ms Pulsweite). */
#define SERVO_DEG_PULSE_90  365

/** @brief PWM-Wert für 135° Servo-Position (≈ 2.33 ms Pulsweite). */
#define SERVO_DEG_PULSE_135 477

/**
 * @brief Initialisiert den PCA9685 für 50 Hz PWM-Betrieb.
 *
 * Konfiguriert den PCA9685 mit folgenden Einstellungen:
 *   - PWM-Frequenz: 50 Hz (geeignet für Standard-Servos)
 *   - PRESCALE: 121 (0x79) für 25 MHz Oszillator
 *   - Auto-Increment und ALLCALL aktiviert
 *
 * @note Der PCA9685 wird kurzzeitig in den SLEEP-Modus versetzt,
 *       um den PRESCALE-Wert zu setzen.
 */
void PCA9685_init(void);

/**
 * @brief Setzt die PWM-Position eines bestimmten Kanals.
 *
 * Konfiguriert die PWM-Ausgabe für einen spezifischen Kanal.
 * Das PWM-Signal startet immer bei 0 (LED_ON = 0) und endet
 * bei der angegebenen Position (LED_OFF = position).
 *
 * @param[in] channel Kanal-Nummer (0-15). Werte außerhalb dieses
 *                    Bereichs werden ignoriert.
 * @param[in] position 12-Bit PWM-Position (0-4095). Bestimmt die
 *                     Pulsweite des PWM-Signals.
 */
void PCA9685_set_servo_position(uint8_t channel, uint16_t position);

#endif /* PCA9685_H */