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
 * @brief Setzt die Plattform in ihre Standardposition.
 *
 * Bewegt beide Servos (Richtung und Kippen) in ihre 90°-Standardposition.
 * Dies ist die neutrale Position, in der die Plattform bereit ist,
 * neue Objekte zu empfangen.
 */
void plattform_default_position(void);

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
 *
 * Stellt den Richtungsservo auf 45° (Rot-Position) und führt
 * anschließend die Entleerungssequenz aus.
 */
void plattform_empty_r(void);

/**
 * @brief Entleert die Plattform in Richtung Grün.
 *
 * Stellt den Richtungsservo auf 90° (Grün-Position) und führt
 * anschließend die Entleerungssequenz aus.
 */
void plattform_empty_g(void);

/**
 * @brief Entleert die Plattform in Richtung Blau.
 *
 * Stellt den Richtungsservo auf 135° (Blau-Position) und führt
 * anschließend die Entleerungssequenz aus.
 */
void plattform_empty_b(void);

#endif /* PLATFORM_H_ */
