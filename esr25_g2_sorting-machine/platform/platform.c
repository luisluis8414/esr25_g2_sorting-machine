/* ========================================================================== */
/* platform.c                                                                */
/* ========================================================================== */
/**
 * @file      platform.c
 * @author    wehrberger
 * @date      10.06.2025
 *
 * @brief     Implementierung der Plattform Steuerung für die Sortiermaschine.
 */

#include "PCA9685/PCA9685.h"
#include "platform.h"
#include "../timer/timer.h"

void plattform_default_position(void)
{
    PCA9685_set_servo_position(RICHTUNGSSERVO, SERVO_DEG_PULSE_90); 
    PCA9685_set_servo_position(KIPPSERVO, SERVO_DEG_PULSE_90);     

    timer_sleep_ms(500);
}

void plattform_sleep_position(void)
{
    PCA9685_set_servo_position(RICHTUNGSSERVO, SERVO_DEG_PULSE_90); 
    PCA9685_set_servo_position(KIPPSERVO, SERVO_DEG_PULSE_135);    
}

void plattform_empty(void)
{
    timer_sleep_ms(700); // Kurze Verzögerung vor Bewegung

    // Plattform kippen zum Entleeren
    PCA9685_set_servo_position(KIPPSERVO, SERVO_DEG_PULSE_40);
    timer_sleep_ms(1500); // Zeit zum vollständigen Entleeren

    // Zurück zur Standardposition
    plattform_default_position();
}

void plattform_empty_r(void)
{
    // Richtung auf Rot (50°) einstellen
    PCA9685_set_servo_position(RICHTUNGSSERVO, SERVO_DEG_PULSE_50);
    plattform_empty();
}

void plattform_empty_g(void)
{
    // Richtung auf Grün (90°) einstellen
    PCA9685_set_servo_position(RICHTUNGSSERVO, SERVO_DEG_PULSE_90);
    plattform_empty();
}

void plattform_empty_b(void)
{
    // Richtung auf Blau (120°) einstellen
    PCA9685_set_servo_position(RICHTUNGSSERVO, SERVO_DEG_PULSE_120);
    plattform_empty();
}
