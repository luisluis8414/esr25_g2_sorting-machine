#include "msp430fr2355.h"
#include <stdint.h>
#include "I2C/I2C.h"
#include "PCA9685.h"


void PCA9685_init() {
   //-- Set PWM frequency to 50hz
    // https://cdn-shop.adafruit.com/datasheets/PCA9685.pdf page 25
    // round(25,000,000/(4096 × 50)) - 1 = 121 (0x79)
    // has to happen while in SLEEP Mode
    char PRESCALE_DATA[] = {0xFE, 0x79}; 
    I2C_write(PCA9685_ADDR, PRESCALE_DATA, 2);

    char MODE1_AI_ALLCALL_DATA[] = {0x00, 0x21};
    I2C_write(PCA9685_ADDR, MODE1_AI_ALLCALL_DATA, 2);
}

void PCA9685_set_servo_position(uint8_t channel, uint16_t position) {
    if(channel > 15){
        return;
    }

    char servo_data[] = {LED_ON_L(channel), 0x00, 0x00, (position & 0xFF), (position >> 8)};
    I2C_write(PCA9685_ADDR, servo_data, 5);
}

