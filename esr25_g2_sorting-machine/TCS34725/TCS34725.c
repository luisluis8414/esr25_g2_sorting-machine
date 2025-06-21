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
    #include <math.h>
    #include "intrinsics.h"
    #include "timer/timer.h"

    void TCS_init(void)
    {
        // LED-Pin P1.7 als Ausgang konfigurieren und ausschalten
        P1DIR |= BIT7;
        P1OUT &= ~BIT7;

        // Schritt 1: Nur Power-On aktivieren
        char enable_pon[] = { CMD(TCS34725_ENABLE), 0x01 };
        I2C_write(TCS34725_ADDRESS, enable_pon, 2);

        // 2.4ms Warm-Up Phase (laut Datenblatt)
        timer_sleep_ms(5);

        // Schritt 2: Integration Time setzen – z.B. 100ms = ATIME = 0xD6
        char atime[] = { CMD(TCS34725_ATIME), 0xD6 };
        I2C_write(TCS34725_ADDRESS, atime, 2);

        // Schritt 3: Verstärkung setzen – 0x01 = 4x Gain
        char control[] = { CMD(TCS34725_CONTROL), 0x01 };
        I2C_write(TCS34725_ADDRESS, control, 2);

        // Schritt 4: ADC aktivieren (AEN) → jetzt PON | AEN = 0x03
        char enable_adc[] = { CMD(TCS34725_ENABLE), 0x03 };
        I2C_write(TCS34725_ADDRESS, enable_adc, 2);

        setupWakeOnDarkness();

        TCS_clear_int();
    }

    uint16_t TCS_read_16bit_reg(uint8_t reg)
    {
        uint8_t low = I2C_read_reg(TCS34725_ADDRESS, CMD(reg));
        uint8_t high = I2C_read_reg(TCS34725_ADDRESS, CMD(reg + 1));
        return ((uint16_t)high << 8) | low;  // Little-Endian Kombination
    }

    void TCS_read_rgbc(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c)
    {
        TCS_led_on();

        timer_sleep_ms(200);

        *c = TCS_read_16bit_reg(TCS34725_CDATAL);
        *r = TCS_read_16bit_reg(TCS34725_RDATAL);
        *g = TCS_read_16bit_reg(TCS34725_GDATAL);
        *b = TCS_read_16bit_reg(TCS34725_BDATAL);

        TCS_led_off();
    }

    void TCS_get_rgb888(uint8_t *r8, uint8_t *g8, uint8_t *b8)
    {
        uint16_t r, g, b, c;
        
        TCS_read_rgbc(&r, &g, &b, &c);

        if (!c) {
            *r8 = *g8 = *b8 = 0;
            return;
        }

        // Maximalwert finden um Skalierungsfaktor zu bestimmen
        uint16_t max_val = (r > g) ? r : g;
        max_val = (max_val > b) ? max_val : b;
        
        // Proportional herunterskalieren bis Maximalwert in 8-Bit Bereich passt
        while (max_val > 255) {
            r >>= 1;
            g >>= 1;
            b >>= 1;
            max_val >>= 1;
        }

        *r8 = r;
        *g8 = g;
        *b8 = b;
    }


    void TCS_led_on(void)
    {
        P1OUT |= BIT7;  // LED einschalten
    }

    void TCS_led_off(void)
    {
        P1OUT &= ~BIT7; // LED ausschalten
    }

    void setupWakeOnDarkness(void)
    {
        // Schritt 1: Nur Power-On aktivieren
        char enable_pon[] = { CMD(TCS34725_ENABLE), 0x01 };
        I2C_write(TCS34725_ADDRESS, enable_pon, 2);

        timer_sleep_ms(5);  // Wartezeit >2.4ms

        // Schritt 2: Schwellen setzen (Clear-Kanal)
        char ailtl[] = { CMD(0x04), 0xE8 };  // 0x03E8 = 1000
        char ailth[] = { CMD(0x05), 0x03 };
        char aihtl[] = { CMD(0x06), 0xFF };
        char aihth[] = { CMD(0x07), 0xFF };
        I2C_write(TCS34725_ADDRESS, ailtl, 2);
        I2C_write(TCS34725_ADDRESS, ailth, 2);
        I2C_write(TCS34725_ADDRESS, aihtl, 2);
        I2C_write(TCS34725_ADDRESS, aihth, 2);

        // Schritt 3: Interrupt-Persistenz
        char pers[] = { CMD(0x0C), 0x01 };
        I2C_write(TCS34725_ADDRESS, pers, 2);

        // Schritt 4: Interrupt + ADC aktivieren
        char enable_full[] = { CMD(TCS34725_ENABLE), 0x13 };  // PON | AEN | AIEN
        I2C_write(TCS34725_ADDRESS, enable_full, 2);

        // Schritt 5: MSP430 INT an P1.4 konfigurieren
        P1DIR &= ~BIT4;
        P1REN |= BIT4;
        P1OUT |= BIT4;
        P1IES |= BIT4;
        P1IFG &= ~BIT4;
        P1IE  |= BIT4;
    }

    void TCS_clear_int(void) {
        char clear_int[] = { 0xE6 };  // Special Function: Clear Interrupt
        I2C_write(TCS34725_ADDRESS, clear_int, 1);
    }


    #pragma vector=PORT1_VECTOR
    __interrupt void PORT1_ISR(void)
    {
        if (P1IFG & BIT4)
        {
            P1IFG &= ~BIT4;                    // Flag löschen
            __bic_SR_register_on_exit(LPM3_bits);  // CPU aufwecken

            __no_operation();
        }
    }
