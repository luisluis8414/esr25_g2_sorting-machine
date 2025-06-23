/* ========================================================================== */
/* TCS34725.c                                                                 */
/* ========================================================================== */
/**
 * @file      TCS34725.c
 * @author    qunaku, wehrberger
 * @date      10.06.2025
 *
 * @brief     Implementierung des TCS34725 RGB-Farbsensor Treibers.
 */

#include "TCS34725.h"
#include "I2C/I2C.h"
#include <msp430.h>
#include "intrinsics.h"
#include "timer/timer.h"

void TCS_init(void)
{
    P1DIR |= BIT7;  // LED-Pin als Ausgang konfigurieren
    P1OUT &= ~BIT7; // LED ausschalten

    // Sensor aktivieren (PON = 1)
    char enable_cmd[] = { TCS_CMD(TCS34725_ENABLE), 0x01 };
    I2C_write(TCS34725_ADDRESS, enable_cmd, sizeof(enable_cmd));

    timer_sleep_ms(3); // Warm-up ≥2.4 ms

    // Integrationszeit auf 100 ms setzen
    char atime_cmd[] = { TCS_CMD(TCS34725_ATIME), 0xD6 };
    I2C_write(TCS34725_ADDRESS, atime_cmd, sizeof(atime_cmd));

    // Verstärkung auf 4x setzen
    char control_cmd[] = { TCS_CMD(TCS34725_CONTROL), 0x01 };
    I2C_write(TCS34725_ADDRESS, control_cmd, sizeof(control_cmd));

    // Lichtschranken-Schwellenwerte setzen
    char threshold_cmd[] = {
        TCS_CMD_AI(0x04),  // Auto-Increment ab AILTL
        0xE8, 0x03,        // AILTL, AILTH (0x03E8 = 1000)
        0xFF, 0xFF         // AIHTL, AIHTH (0xFFFF = max)
    };
    I2C_write(TCS34725_ADDRESS, threshold_cmd, sizeof(threshold_cmd));

    // Persistenz auf 1 Ereignis setzen
    char pers_cmd[] = { TCS_CMD(0x0C), 0x01 };
    I2C_write(TCS34725_ADDRESS, pers_cmd, sizeof(pers_cmd));

    // Sensor wieder schlafen legen (PON = 0)
    char sleep_cmd[] = { TCS_CMD(TCS34725_ENABLE), 0x00 };
    I2C_write(TCS34725_ADDRESS, sleep_cmd, sizeof(sleep_cmd));
}

uint16_t TCS_read_16bit_reg(uint8_t reg)
{
    uint8_t low = I2C_read_reg(TCS34725_ADDRESS, TCS_CMD(reg));
    uint8_t high = I2C_read_reg(TCS34725_ADDRESS, TCS_CMD(reg + 1));
    return ((uint16_t)high << 8) | low; // Little-Endian Kombination
}

void TCS_read_clear(uint16_t *clear)
{
    /* Power-On (PON) */
    char pon[] = { TCS_CMD(TCS34725_ENABLE), 0x01 };
    I2C_write(TCS34725_ADDRESS, pon, 2);
    timer_sleep_ms(3);                       // Warm-up ≥2.4 ms

    /* ADC starten (PON|AEN) */
    char aen[] = { TCS_CMD(TCS34725_ENABLE), 0x03 };
    I2C_write(TCS34725_ADDRESS, aen, 2);

        // Wartezeit für Messung (ATIME = 100ms + Puffer)
    timer_sleep_ms(120);

    /* Clear-Kanal lesen */
    *clear = TCS_read_16bit_reg(TCS34725_CDATAL);

    /* Sensor vollständig ausschalten (PON = 0) */
    char off[] = { TCS_CMD(TCS34725_ENABLE), 0x00 };
    I2C_write(TCS34725_ADDRESS, off, 2);
}


void TCS_get_rgb(uint8_t *r8, uint8_t *g8, uint8_t *b8)
{
    TCS_led_on(); // LED einschalten
    uint16_t r, g, b, c;

    // Sensor aktivieren (PON = 1)
    char enable_cmd[] = { TCS_CMD(TCS34725_ENABLE), 0x01 };
    I2C_write(TCS34725_ADDRESS, enable_cmd, 2);

    timer_sleep_ms(3); // Wartezeit für Sensor-Warm-up

    // ADC starten (PON | AEN)
    char adc_cmd[] = { TCS_CMD(TCS34725_ENABLE), 0x03 };
    I2C_write(TCS34725_ADDRESS, adc_cmd, 2);

    // Wartezeit für Messung (ATIME = 100ms + Puffer)
    timer_sleep_ms(120);

    // RGBC-Werte auslesen
    c = TCS_read_16bit_reg(TCS34725_CDATAL);
    r = TCS_read_16bit_reg(TCS34725_RDATAL);
    g = TCS_read_16bit_reg(TCS34725_GDATAL);
    b = TCS_read_16bit_reg(TCS34725_BDATAL);

    // Sensor ausschalten
    char sleep_cmd[] = { TCS_CMD(TCS34725_ENABLE), 0x00 };
    I2C_write(TCS34725_ADDRESS, sleep_cmd, 2);

    TCS_led_off(); // LED ausschalten

    // Wenn der Clear-Kanal-Wert 0 ist, RGB-Werte auf 0 setzen
    if (!c) {
        *r8 = *g8 = *b8 = 0;
        return;
    }

    // Maximalwert finden
    uint16_t max_val = (r > g) ? r : g;
    max_val = (max_val > b) ? max_val : b;

    // RGB-Werte proportional herunterskalieren, bis der Maximalwert in den 8-Bit-Bereich passt
    while (max_val > 255) {
        r >>= 1;
        g >>= 1;
        b >>= 1;
        max_val >>= 1;
    }

    // Skalierte 8-Bit RGB-Werte speichern
    *r8 = r;
    *g8 = g;
    *b8 = b;
}

void TCS_led_on(void)
{
    P1OUT |= BIT7; // LED einschalten
}

void TCS_led_off(void)
{
    P1OUT &= ~BIT7; // LED ausschalten
}
