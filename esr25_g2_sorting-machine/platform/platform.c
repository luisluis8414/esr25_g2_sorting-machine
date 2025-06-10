#include "PCA9685/PCA9685.h"
#include "platform.h"

void plattform_default_position(void) {
    PCA9685_set_servo_position(0, SERVO_DEG_PULSE_90);

    PCA9685_set_servo_position(4, SERVO_DEG_PULSE_90);
}

void plattform_empty(void) {
    __delay_cycles(500000);

    PCA9685_set_servo_position(4, SERVO_DEG_PULSE_45);
}

void plattform_empty_r(void) {
    PCA9685_set_servo_position(0, SERVO_DEG_PULSE_45);

    plattform_empty();
}

void plattform_empty_g(void) {
    PCA9685_set_servo_position(0, SERVO_DEG_PULSE_90);

    plattform_empty();
}

void plattform_empty_b(void) {
    PCA9685_set_servo_position(0, SERVO_DEG_PULSE_135);

    plattform_empty();
}
