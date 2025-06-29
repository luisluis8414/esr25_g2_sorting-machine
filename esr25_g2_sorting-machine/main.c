/* ========================================================================== */
/* main.c                                                                       */
/* ========================================================================== */
/**
 * @file      main.c
 * @author    wehrberger, raach, qunaku
 * @date      22.05.2025
 *
 * @brief     Hauptprogramm der Sortieranlage.
 *
 * Dieses Modul implementiert die Hauptfunktionalität der Sortieranlage:
 *   - Initialisierung aller Hardwaremodule
 *   - Event basierte main loop
 *   - Watchdog und GPIO Konfiguration
 */

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

#include "PCA9685/PCA9685.h"
#include "button/button.h"
#include "platform/platform.h"
#include "I2C/I2C.h"
#include "timer/timer.h"
#include "TCS34725/TCS34725.h"
#include "lcd1602_display/lcd1602.h"
#include "lcd1602_display/lcd1602_manager.h"
#include "state_machine/state_machine.h"
#include "led/led.h"

/** @brief Global Event Bits Variable für Event Handling */
Event_t eventBits = 0;

/**
 * @brief Initialisiert alle GPIO Ports.
 *
 * Setzt alle GPIO Ports in einen definierten Ausgangszustand:
 *   - Alle Pins als Ausgänge konfiguriert
 *   - Alle Ausgänge auf Low-Pegel gesetzt
 */
void init_all_ports(void)
{
    P1OUT = 0x00;
    P1DIR = 0xFF;
    P2OUT = 0x00;
    P2DIR = 0xFF;
    P3OUT = 0x00;
    P3DIR = 0xFF;
    P4OUT = 0x00;
    P4DIR = 0xFF;
    P5OUT = 0x00;
    P5DIR = 0xFF;
    P6OUT = 0x00;
    P6DIR = 0xFF;

    PAOUT = 0x00;
    PADIR = 0xFF;
    PBOUT = 0x00;
    PBDIR = 0xFF;
    PCOUT = 0x00;
    PCDIR = 0xFF;
}

/**
 * @brief Initialisiert alle Hardwaremodule der Sortieranlage.
 *
 * Führt die Initialisierung in folgender Reihenfolge durch:
 *   1. GPIO Ports (Grundkonfiguration)
 *   2. I2C Bus für Peripherie
 *   3. PCA9685 Servo Controller
 *   4. Timer für Systemtakt
 *   5. TCS34725 Farbsensor
 *   6. Buttons für Benutzereingaben
 *   7. LCD1602 Display
 *   8. Status LEDs
 *
 * Konfiguriert anschließend den 1sek Systemtakt und
 * versetzt die Plattform in die Schlaf Position.
 */
void init(void)
{
    init_all_ports();

    I2C_init();
    PCA9685_init();
    timer_init();
    TCS_init();
    button_init();
    lcd1602_init();
    led_init();

    timer_systick_init(1000);

    plattform_sleep_position();
    turnDisplayOff();

    __enable_interrupt();
}

/**
 * @brief Main Loop
 *
 *   1. Deaktiviert den Watchdog Timer
 *   2. Aktiviert die GPIOs
 *   3. Initialisiert alle Hardwaremodule
 *   4. Verarbeitet Events in main loop mit LPM3
 *
 * Die Main Loop prüft kontinuierlich auf neue Events und
 * leitet diese an die State Machine weiter. In Phasen ohne
 * Events wird der Prozessor in den LPM3 versetzt.
 *
 * @return Wird nie erreicht (Endlosschleife)
 */
int main(void)
{
    State_t currentState = OFF_STATE;
    Event_t event = EVT_NO_EVENT;

    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    init();

    while (true)
    {
        event = getEvent(&eventBits);
        if (event != EVT_NO_EVENT)
        {
            while (event > EVT_NO_EVENT)
            {
                handleEvent_FSM(&currentState, event);
                event = getEvent(&eventBits);
            }
        }
        LPM3;
    }
}
