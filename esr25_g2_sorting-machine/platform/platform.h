/* ========================================================================== */
/* platform.h                                                                */
/* ========================================================================== */
/**
 * @file      platform.h
 * @author    wehrberger
 * @date      10.06.2025
 *
 * @brief     Plattform-Steuerung für automatische Sortiermaschine.
 *
 * Dieses Modul steuert die Kippplatform, die Objekte
 * basierend auf ihrer Farbe in verschiedene Richtungen entleert. Die
 * Plattform verwendet zwei Servos:
 *   - Servo 0: Richtungssteuerung (45°=Rot, 90°=Grün, 135°=Blau)
 *   - Servo 4: Kippbewegung zum Entleeren der Plattform
 *
 * Verfügbare Funktionen:
 *   - plattform_default_position() – Standardposition (beide Servos 90°)
 *   - plattform_empty_r/g/b()      – Entleeren in Rot/Grün/Blau-Richtung
 *   - plattform_empty()            – Kippbewegung zum Entleeren
 *
 * @note Dieses Modul benötigt den PCA9685 PWM Treiber und Timer Modul.
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

/**
 * @brief Servo Kanal Definitionen
 */
#define RICHTUNGSSERVO 0 /**< PCA9685 Kanal für Richtungssteuerung */
#define KIPPSERVO 4      /**< PCA9685 Kanal für Kippbewegung */

/**
 * @brief Setzt die Plattform in ihre Standardposition.
 */
void plattform_default_position(void);

/**
 * @brief Setzt die Plattform in ihre Schlafposition.
 */
void plattform_sleep_position(void);

/**
 * @brief Führt die Kippbewegung zum Entleeren der Plattform aus.
 *
 * Diese Funktion führt die eigentliche Entleerungssequenz aus:
 *   1. 500ms Verzögerung vor der Bewegung
 *   2. Kippservo auf 45° (Entleerungsposition)
 *   3. 1000ms warten für vollständiges Entleeren
 *   4. Rückkehr zur Standardposition
 *
 * @note Diese Funktion wird normalerweise nicht direkt aufgerufen,
 *       sondern über die farbspezifischen Entleerungsfunktionen.
 */
void plattform_empty(void);

/**
 * @brief Entleert die Plattform in Richtung Rot.
 */
void plattform_empty_r(void);

/**
 * @brief Entleert die Plattform in Richtung Grün.
 */
void plattform_empty_g(void);

/**
 * @brief Entleert die Plattform in Richtung Blau.
 */
void plattform_empty_b(void);

#endif /* PLATFORM_H_ */
