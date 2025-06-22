/*
 * button.c
 *
 *  Created on: 22.06.2025
 *      Author: raachl
 */

#include "button.h"
#include "lcd1602_display/lcd1602.h"
#include "lcd1602_display/lcd1602_manager.h"

extern lcd1602_res_t lcd1602_backlight(bool on);
extern void writeCurrentCount(uint8_t current_count_all, uint8_t current_count_blue, 
uint8_t current_count_green, uint8_t current_count_red);
extern void writeReady(void); 
extern void writeDetectedColor(COLOR color);

void button_init(void)
{
    // --- P4.1 (Button1) konfigurieren ---
    P4DIR &= ~BIT1;   // Pin als Eingang
    P4OUT |= BIT1;    // Pull-Up aktivieren
    P4REN |= BIT1;    // Pull-Widerstand aktivieren

    P4IES |= BIT1;    // Interrupt bei fallender Flanke (HIGH -> LOW)
    P4IFG &= ~BIT1;   // Interrupt-Flag löschen
    P4IE  |= BIT1;    // Interrupt aktivieren
        // --- P4.1 (Button1) konfigurieren ---
    P2DIR &= ~BIT3;   // Pin als Eingang
    P2OUT |= BIT3;    // Pull-Up aktivieren
    P2REN |= BIT3;    // Pull-Widerstand aktivieren

    P2IES |= BIT3;    // Interrupt bei fallender Flanke (HIGH -> LOW)
    P2IFG &= ~BIT3;   // Interrupt-Flag löschen
    P2IE  |= BIT3;    // Interrupt aktivieren
}

// --- ISR für Port 4 ---
#pragma vector = PORT4_VECTOR
__interrupt void Port_4_ISR(void)
{
    if (P4IFG & BIT1) {
        P4IFG &= ~BIT1; // Flag löschen
        // Button4Event();
        lcd1602_backlight(1);
        writeCurrentCount(12, 8, 5, 7);
        // __bic_SR_register_on_exit(LPM3_bits); /* Exit LPM3 */
    }
}

// --- ISR für Port 2 ---
#pragma vector = PORT2_VECTOR
__interrupt void Port_2_ISR(void)
{
    if (P2IFG & BIT3) {
        P2IFG &= ~BIT3; // Flag löschen
        //Button2Event();
        //lcd1602_backlight(0);
        writeDetectedColor(BLUE);
    }
}
