#include "msp430fr2355.h"
#include <stdint.h>
#include "I2C/I2C.h"
#include "PCA9685.h"

// schwere Berechungen, später Funktion durch feste Werte ersetzen, 45, 90, 135 deg? 
// uint16_t degrees_to_pulse(uint8_t angle_deg, uint16_t max_pulse, uint16_t min_pulse) {
//     if (angle_deg >= 180) {
//         return max_pulse;
//     }
//     const uint32_t span = (uint32_t)max_pulse - (uint32_t)min_pulse;
//     uint32_t scaled = ((uint32_t)angle_deg * span) / 180U;
//     return (uint16_t)(min_pulse + scaled);
// }


// void set_basic_servo_angle(uint8_t channel, uint8_t angle_deg) {
//     if (channel > 15) {
//         return; 
//     }

//     if (angle_deg > 180) {
//         angle_deg = 180;
//     }

//     uint16_t pulse = degrees_to_pulse(angle_deg, BASIC_SERVO_MAX_PULSE, BASIC_SERVO_MIN_PULSE);
//     set_servo_position(channel, pulse);
// }


void init_PCA9685() {
   //-- Set PWM frequency to 50hz
    // https://cdn-shop.adafruit.com/datasheets/PCA9685.pdf page 25
    // round(25,000,000/(4096 × 50)) - 1 = 121 (0x79)
    // has to happen while in SLEEP Mode
    char PRESCALE_DATA[] = {0xFE, 0x79}; 
    I2C_write(PCA9685_ADDR, PRESCALE_DATA, 2);

    char MODE1_AI_ALLCALL_DATA[] = {0x00, 0x21};
    I2C_write(PCA9685_ADDR, MODE1_AI_ALLCALL_DATA, 2);
}

void set_servo_position(uint8_t channel, uint16_t position) {
    if(channel > 15){
        return;
    }

    char servo_data[] = {LED_ON_L(channel), 0x00, 0x00, (position & 0xFF), (position >> 8)};
    I2C_write(PCA9685_ADDR, servo_data, 5);
}

