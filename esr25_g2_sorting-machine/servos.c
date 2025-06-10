#include "PCA9685.h"
#include "utils.h"

void enter_default_position(void) {
    set_basic_servo_angle(0, 90);


    set_basic_servo_angle(4, 90);
}

void empty(void) {
    delay_ms(500);

    set_basic_servo_angle(4, 45);
}

void empty_posi1(void) {
    set_basic_servo_angle(0, 45);

    empty();
}

void empty_posi2(void) {
    set_basic_servo_angle(0, 90);

    empty();
}

void empty_posi3(void) {
    set_basic_servo_angle(0, 135);

    empty();
}