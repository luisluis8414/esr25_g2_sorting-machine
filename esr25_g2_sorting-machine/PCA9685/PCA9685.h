#ifndef PCA9685_H
#define PCA9685_H

#include <stdint.h>

/// Die I2C-Adresse des PCA9685-Moduls.
#define PCA9685_ADDR      0x40

/// Registeradresse für LED0_ON_L.
#define LED0_ON_L         0x06
/// Registeradresse für LED0_ON_H.
#define LED0_ON_H         0x07
/// Registeradresse für LED0_OFF_L.
#define LED0_OFF_L        0x08
/// Registeradresse für LED0_OFF_H.
#define LED0_OFF_H        0x09

/// Berechnet die Registeradresse für LED_ON_L eines bestimmten Kanals.
#define LED_ON_L(channel)  (LED0_ON_L + 4 * (channel))
/// Berechnet die Registeradresse für LED_ON_H eines bestimmten Kanals.
#define LED_ON_H(channel)  (LED0_ON_H + 4 * (channel))
/// Berechnet die Registeradresse für LED_OFF_L eines bestimmten Kanals.
#define LED_OFF_L(channel) (LED0_OFF_L + 4 * (channel))
/// Berechnet die Registeradresse für LED_OFF_H eines bestimmten Kanals.
#define LED_OFF_H(channel) (LED0_OFF_H + 4 * (channel))

/// Pulsweite für 45 Grad bei einem Servo.
#define SERVO_DEG_PULSE_45  252 // 252 * (20 ms / 4096) ≈ 1.23 ms
/// Pulsweite für 90 Grad bei einem Servo.
#define SERVO_DEG_PULSE_90  365 // 365 * (20 ms / 4096) ≈ 1.78 ms
/// Pulsweite für 135 Grad bei einem Servo.
#define SERVO_DEG_PULSE_135 477 // 477 * (20 ms / 4096) ≈ 2.33 ms

/**
 * @brief Initialisiert den PCA9685 für 50 Hz PWM.
 * 
 * Diese Funktion setzt den PRESCALE-Wert und den MODE1-Registerwert, um
 * den PCA9685 in den gewünschten Betriebsmodus zu versetzen.
 */
void PCA9685_init(void);

/**
 * @brief Setzt die PWM-Position eines bestimmten Kanals.
 * 
 * @param channel Der Kanal, der eingestellt werden soll (0 bis 15).
 * @param position Die 12-Bit-Position (0 bis 4095), die die Pulsweite definiert.
 * 
 * @note Die PWM startet immer bei 0 (LED_ON wird auf 0 gesetzt), 
 *       und die Position definiert den Zeitpunkt, an dem das Signal endet (LED_OFF).
 */
void PCA9685_set_servo_position(uint8_t channel, uint16_t position);

#endif
