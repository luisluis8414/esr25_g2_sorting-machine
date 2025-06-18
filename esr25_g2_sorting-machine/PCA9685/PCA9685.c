#include "msp430fr2355.h"
#include <stdint.h>
#include "I2C/I2C.h"
#include "PCA9685.h"

/**
 * @brief Initialisiert den PCA9685 für 50 Hz PWM.
 * 
 * Diese Funktion setzt den PRESCALE-Wert auf 121 (0x79), um eine PWM-Frequenz
 * von 50 Hz zu erreichen. Der PCA9685 wird in den SLEEP-Modus versetzt, um
 * den PRESCALE-Wert zu setzen, und anschließend in den normalen Betriebsmodus
 * zurückgesetzt.
 */
void PCA9685_init() {
    // https://cdn-shop.adafruit.com/datasheets/PCA9685.pdf Seite 25
    // round(25,000,000/(4096 × 50)) - 1 = 121 (0x79)
    char PRESCALE_DATA[] = {0xFE, 0x79}; 
    I2C_write(PCA9685_ADDR, PRESCALE_DATA, 2);

    char MODE1_AI_ALLCALL_DATA[] = {0x00, 0x21};
    I2C_write(PCA9685_ADDR, MODE1_AI_ALLCALL_DATA, 2);
}

/**
 * @brief Setzt die PWM-Position eines bestimmten Kanals.
 * 
 * Diese Funktion schreibt die entsprechenden Registerwerte, um die PWM-Position
 * eines Servos oder einer LED auf einem bestimmten Kanal einzustellen.
 * 
 * @param channel Der Kanal, der eingestellt werden soll (0 bis 15).
 *                Wenn der Kanal außerhalb dieses Bereichs liegt, wird die
 *                Funktion ohne Aktion beendet.
 * @param position Die 12-Bit-Position (0 bis 4095), die die Pulsweite definiert.
 *                 Die unteren 8 Bits werden in das LOW-Register geschrieben,
 *                 die oberen 4 Bits in das HIGH-Register.
 */
void PCA9685_set_servo_position(uint8_t channel, uint16_t position) {
    if(channel > 15){
        return;
    }

    // (position & 0xFF) das niedrigere Byte 
    // (position >> 8) das höhere Byte 
    char servo_data[] = {LED_ON_L(channel), 0x00, 0x00, (position & 0xFF), (position >> 8)};
    I2C_write(PCA9685_ADDR, servo_data, 5);
}
