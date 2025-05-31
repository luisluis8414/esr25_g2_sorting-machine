#ifndef PCA9685_H
#define PCA9685_H

#include <stdint.h>

#define PCA9685_ADDR      0x40

#define LED0_ON_L         0x06
#define LED0_ON_H         0x07
#define LED0_OFF_L        0x08
#define LED0_OFF_H        0x09

#define LED_ON_L(channel)  (LED0_ON_L + 4 * (channel))
#define LED_ON_H(channel)  (LED0_ON_H + 4 * (channel))
#define LED_OFF_L(channel) (LED0_OFF_L + 4 * (channel))
#define LED_OFF_H(channel) (LED0_OFF_H + 4 * (channel))


#define SERVO_MIN_PULSE  145   
#define SERVO_MAX_PULSE  605   

// #define SERVO_MIN_PULSE  205   // ≈1.0 ms 
// #define SERVO_MAX_PULSE  410   // ≈2.0 ms 


uint16_t degrees_to_pulse(uint8_t angle_deg);

void set_servo_angle(uint8_t channel, uint8_t angle_deg);

// Initialize PCA9685 to 50 Hz PWM (writes PRESCALE and MODE1 registers).
void init_PCA9685(void);

// Set PWM on channel [0..15], position = 12-bit (0..4095).
void set_servo_position(uint8_t channel, uint16_t position);

#endif 
