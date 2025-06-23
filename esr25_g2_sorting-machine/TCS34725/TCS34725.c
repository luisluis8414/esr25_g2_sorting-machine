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

    /* ------------------------------------------------------------------ */
    /*  TCS_init()  – nur Parameter setzen, Sensor bleibt OFF   */
    /* ------------------------------------------------------------------ */
    void TCS_init(void)
    {
        P1DIR |= BIT7;
        P1OUT &= ~BIT7;

        /* PON an, um schreiben zu können (0x01) */
        char pon[] = { CMD(TCS34725_ENABLE), 0x01 };
        I2C_write(TCS34725_ADDRESS, pon, 2);

        timer_sleep_ms(3);                       // min 2.4 ms Warm-up

        /* 100 ms Integration (0xD6) */
        char atime[] = { CMD(TCS34725_ATIME), 0xD6 };
        I2C_write(TCS34725_ADDRESS, atime, 2);

        /* Gain 4× */
        char gain[]  = { CMD(TCS34725_CONTROL), 0x01 };
        I2C_write(TCS34725_ADDRESS, gain, 2);

        /* Lichtschranken-Schwelle: Clear < 0x0200  */
        /* Lichtschranken-Schwelle: Clear < 0x0200 */
        char thresholds[] = { 
            CMD_AI(0x04),  // Auto-Increment ab AILTL
            0xE8, 0x03,    // AILTL, AILTH (0x03E8 = 1000)
            0xFF, 0xFF     // AIHTL, AIHTH (0xFFFF = max)
        };
        I2C_write(TCS34725_ADDRESS, thresholds, 5);

        /* Persistenz 1 Ereignis (optional) */
        char pers[] = { CMD(0x0C), 0x01 };
        I2C_write(TCS34725_ADDRESS, pers, 2);

        /* Sensor wieder schlafen (PON = 0) */
        char sleep[] = { CMD(TCS34725_ENABLE), 0x00 };
        I2C_write(TCS34725_ADDRESS, sleep, 2);
    }

    void TCS_single_shot_sleep(uint16_t *c_out)
    {
        /* 1️⃣  Power-On (PON) */
        char pon[] = { CMD(TCS34725_ENABLE), 0x01 };
        I2C_write(TCS34725_ADDRESS, pon, 2);
        timer_sleep_ms(3);                       // Warm-up ≥2.4 ms

        /* 2️⃣  ADC starten (PON|AEN) */
        char aen[] = { CMD(TCS34725_ENABLE), 0x03 };
        I2C_write(TCS34725_ADDRESS, aen, 2);

        /* 3️⃣  Einfach warten – kein Polling */
        timer_sleep_ms(120);

        /* 4️⃣  Clear-Kanal lesen */
        *c_out = TCS_read_16bit_reg(TCS34725_CDATAL);

        /* 5️⃣  Sensor vollständig ausschalten (PON = 0) */
        char off[] = { CMD(TCS34725_ENABLE), 0x00 };
        I2C_write(TCS34725_ADDRESS, off, 2);
    }

    uint16_t TCS_read_16bit_reg(uint8_t reg)
    {
        uint8_t low = I2C_read_reg(TCS34725_ADDRESS, CMD(reg));
        uint8_t high = I2C_read_reg(TCS34725_ADDRESS, CMD(reg + 1));
        return ((uint16_t)high << 8) | low;  // Little-Endian Kombination
    }

    void TCS_single_shot_rgb(uint16_t *r, uint16_t *g,
                         uint16_t *b, uint16_t *c)
    {
        char buf[2];

        /* Sensor ON (PON) */
        buf[0] = CMD(TCS34725_ENABLE); buf[1] = 0x01;
        I2C_write(TCS34725_ADDRESS, buf, 2);

        TCS_led_on();

        timer_sleep_ms(3);

        /* ADC starten (AEN) */
        buf[1] = 0x03;                               // PON | AEN
        I2C_write(TCS34725_ADDRESS, buf, 2);

        /* warten 120 ms (ATIME 100 ms + Reserve) */
        timer_sleep_ms(120);

        /* 4️⃣  RGBC auslesen */
        *c = TCS_read_16bit_reg(TCS34725_CDATAL);
        *r = TCS_read_16bit_reg(TCS34725_RDATAL);
        *g = TCS_read_16bit_reg(TCS34725_GDATAL);
        *b = TCS_read_16bit_reg(TCS34725_BDATAL);

        /* Sensor OFF  */
        buf[1] = 0x00;
        I2C_write(TCS34725_ADDRESS, buf, 2);

        TCS_led_off();
    }

    void TCS_get_rgb888(uint8_t *r8, uint8_t *g8, uint8_t *b8)
    {
        uint16_t r, g, b, c;
        
        TCS_single_shot_rgb(&r, &g, &b, &c);

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

    