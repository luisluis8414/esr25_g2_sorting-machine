#include "PCA9685/PCA9685.h"
#include "platform.h"
#include "../timer/timer.h"

/**
 * @brief Sets the platform to its default position.
 * 
 * This function moves the servos controlling the platform to their default 
 * positions, which are set to 90 degrees.
 */
void plattform_default_position(void) {
    PCA9685_set_servo_position(0, SERVO_DEG_PULSE_90);
    PCA9685_set_servo_position(4, SERVO_DEG_PULSE_90);
}

/**
 * @brief Empties the platform.
 * 
 * This function moves the servo controlling the platform to a position 
 * that empties its contents. A delay of 500 ms is added before the movement.
 */
void plattform_empty(void) {
    timer_sleep_ms(500);
    PCA9685_set_servo_position(4, SERVO_DEG_PULSE_45);
    timer_sleep_ms(1000);
    plattform_default_position();
}

/**
 * @brief Empties the platform in the red direction.
 * 
 * This function moves the servo controlling the platform to the 45-degree 
 * position (red direction) and then calls `plattform_empty()` to complete 
 * the emptying process.
 */
void plattform_empty_r(void) {
    PCA9685_set_servo_position(0, SERVO_DEG_PULSE_45);
    plattform_empty();
}

/**
 * @brief Empties the platform in the green direction.
 * 
 * This function moves the servo controlling the platform to the 90-degree 
 * position (green direction) and then calls `plattform_empty()` to complete 
 * the emptying process.
 */
void plattform_empty_g(void) {
    PCA9685_set_servo_position(0, SERVO_DEG_PULSE_90);
    plattform_empty();
}

/**
 * @brief Empties the platform in the blue direction.
 * 
 * This function moves the servo controlling the platform to the 135-degree 
 * position (blue direction) and then calls `plattform_empty()` to complete 
 * the emptying process.
 */
void plattform_empty_b(void) {
    PCA9685_set_servo_position(0, SERVO_DEG_PULSE_135);
    plattform_empty();
}
