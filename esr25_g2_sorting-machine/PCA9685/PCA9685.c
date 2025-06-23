/* ========================================================================== */
/* PCA9685.c                                                                  */
/* ========================================================================== */
/**
 * @file      PCA9685.c
 * @author    wehrberger
 * @date      31.05.2025
 *
 * @brief     Implementierung des PCA9685 16-Kanal PWM-Treibers.
 */

#include "msp430fr2355.h"
#include <stdint.h>
#include "I2C/I2C.h"
#include "PCA9685.h"

void PCA9685_init() 
{
    // PRESCALE-Wert für 50 Hz PWM setzen
    // Berechnung: round(25,000,000/(4096 × 50)) - 1 = 121 (0x79)
    // Referenz: https://cdn-shop.adafruit.com/datasheets/PCA9685.pdf Seite 25
    char PRESCALE_DATA[] = {0xFE, 0x79}; 
    I2C_write(PCA9685_ADDR, PRESCALE_DATA, 2);

    // MODE1 Register konfigurieren: Auto-Increment + ALLCALL aktivieren
    char MODE1_AI_ALLCALL_DATA[] = {0x00, 0x21};
    I2C_write(PCA9685_ADDR, MODE1_AI_ALLCALL_DATA, 2);
}

void PCA9685_set_servo_position(uint8_t channel, uint16_t position) 
{
    if(channel > 15){
        return;
    }

    // PWM-Register setzen: LED_ON = 0, LED_OFF = position
    // Format: [Register, LED_ON_L, LED_ON_H, LED_OFF_L, LED_OFF_H]
    char servo_data[] = {LED_ON_L(channel), 0x00, 0x00, (position & 0xFF), (position >> 8)};
    I2C_write(PCA9685_ADDR, servo_data, 5);
}
