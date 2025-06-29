/* ========================================================================== */
/* state_machine.c                                                               */
/* ========================================================================== */
/**
 * @file      state_machine.c
 * @author    raachl, wehrberger
 * @date      24.06.2025
 *
 * @brief     Implementierung der State Machine für die Sortieranlage.
 *
 * Dieses Modul implementiert die State Machine Logic zur Steuerung der Sortieranlage.
 * Es verarbeitet State Transitions und Events, steuert die Farberkennung und
 * Sortier Prozess und verwaltet die Sortier Statistiken.
 */

#include "state_machine.h"
#include "TCS34725/TCS34725.h"
#include "lcd1602_display/lcd1602.h"
#include "platform/platform.h"
#include "lcd1602_display/lcd1602_manager.h"
#include "timer/timer.h"
#include "led/led.h"
#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>

/** @brief Referenz Wert für Clear Reading während der Kalibrierung */
volatile uint16_t clear_ref = 0;

/** @brief Minimum Delta für Clear Wert zur Objekt Erkennung */
volatile uint16_t MIN_DELTA_CLR = 0;

/** @brief Anzahl aller sortierten Objekte */
uint8_t total_sorted = 0;

/** @brief Anzahl der sortierten roten Objekte */
uint8_t red_sorted = 0;

/** @brief Anzahl der sortierten grünen Objekte */
uint8_t green_sorted = 0;

/** @brief Anzahl der sortierten blauen Objekte */
uint8_t blue_sorted = 0;

/**
 * @brief Kalibriert den Referenz Wert für das Clear Reading.
 *
 * Liest den derzeitigen Clear Wert und setzt ihn als Referenz.
 * Berechnet auch den Minimum Delta Schwellwert als 40% des Referenz Wertes.
 */
void calibrate_clear(void)
{
    uint16_t c;
    TCS_read_clear(&c);
    clear_ref = c;
    MIN_DELTA_CLR = (c * 4) / 10;
}

/**
 * @brief Prüft auf Objekte mittels dem Farbsensor.
 *
 * Vergleicht den aktuellen Clear Wert mit dem Referenz Wert.
 * Setzt EVT_OBJECT_DETECTED wenn ein Objekt erkannt wurde.
 */
void check_for_objects()
{
    uint16_t clear;
    TCS_read_clear(&clear);

    if (clear + MIN_DELTA_CLR < clear_ref)
    {
        eventBits |= EVT_OBJECT_DETECTED;
    }
}

/**
 * @brief Führt den Sortier Prozess basierend auf der erkannten Farbe durch.
 *
 * Liest RGB Werte, bestimmt die dominante Farbe und aktiviert den
 * entsprechenden Sortier Mechanismus. Aktualisiert die Sortier Statistiken
 * und die Anzeige.
 */
void do_sort(void)
{
    uint8_t r, g, b;
    led_ready_off();
    led_sorting_on(); 
    TCS_get_rgb(&r, &g, &b);

    if (r > g && r > b)
    {
        plattform_empty_r();
        writeDetectedColor(RED);
        red_sorted++;
    }
    else if (g > b)
    {
        plattform_empty_g();
        writeDetectedColor(GREEN);
        green_sorted++;
    }
    else
    {
        plattform_empty_b();
        writeDetectedColor(BLUE);
        blue_sorted++;
    }
    total_sorted++;
    led_sorting_off();
    led_ready_on();

    timer_sleep_ms(500);
    writeCurrentCount(total_sorted, blue_sorted, green_sorted, red_sorted);
}

/**
 * @brief Extrahiert das Event mit der höchsten Priorität aus den Event Bits.
 *
 * Verarbeitet die Event Bits von MSB zu LSB (bis zu 8 Bits),
 * gibt das höchste gesetzte Bit zurück und löscht es.
 *
 * @param[in,out] event Pointer zu den Event Bits
 * @return Das Event mit der höchsten Priorität oder EVT_NO_EVENT wenn keine Events vorhanden sind
 */
Event_t getEvent(Event_t *event)
{
    uint16_t ii = 8;
    Event_t bitMask = 0x0080;

    if (*event != 0)
    {
        while (ii > 0)
        {
            if ((bitMask & *event) > 0)
            {
                *event &= ~bitMask;
                return (bitMask);
            }
            else
            {
                bitMask >>= 1;
            }
            ii--;
        }
    }
    return 0;
}

/**
 * @brief Haupt Event Handler der State Machine.
 *
 * Verarbeitet Events basierend auf dem aktuellen State und führt entsprechende
 * Aktionen aus. Implementiert folgende State Übergänge:
 *   - OFF_STATE: Startpunkt, Übergang zu DISPLAY_STATE oder MODE_SELECTION_STATE möglich
 *   - DISPLAY_STATE: Zeigt Sortier Statistiken, kann Zähler zurücksetzen oder zu OFF_STATE wechseln
 *   - MODE_SELECTION_STATE: Auswahl zwischen AUTO_SORT_STATE und MANUAL_SORT_STATE
 *   - AUTO_SORT_STATE: Automatisches Sortieren mit periodischer Objekt Erkennung
 *   - MANUAL_SORT_STATE: Manuelles Sortieren durch Knopfdruck ausgelöst
 *
 * @param[in,out] currentState Pointer zum aktuellen State
 * @param[in] event Zu verarbeitendes Event
 */
void handleEvent_FSM(State_t *currentState, Event_t event)
{
    switch (*currentState)
    {
    case OFF_STATE:
        switch (event)
        {
        case EVT_S1:
            turnDisplayOn();
            writeCurrentCount(total_sorted, blue_sorted, green_sorted, red_sorted);
            *currentState = DISPLAY_STATE;
            break;
        case EVT_S2:
            turnDisplayOn();
            lcd1602_write(1, "Sortiermodus:");
            lcd1602_write(2, "S1: auto, S2: man");
            *currentState = MODE_SELECTION_STATE;
            break;
        case EVT_SYSTEM_TICK:
            timer_systick_stop();
            break;
        case EVT_OBJECT_DETECTED:
            break;
        }
        break;

    case DISPLAY_STATE:
        switch (event)
        {
        case EVT_S1:
            turnDisplayOff();
            *currentState = OFF_STATE;
            break;
        case EVT_S2:
            total_sorted = 0;
            red_sorted = 0;
            green_sorted = 0;
            blue_sorted = 0;

            writeCurrentCount(total_sorted, blue_sorted, green_sorted, red_sorted);
            break;
        case EVT_SYSTEM_TICK:
            timer_systick_stop();
            break;
        case EVT_OBJECT_DETECTED:
            break;
        }
        break;

    case MODE_SELECTION_STATE:
        switch (event)
        {
        case EVT_S1:
            lcd1602_clear();
            plattform_default_position();
            timer_systick_start();
            calibrate_clear();
            led_ready_on();
            lcd1602_write(1, "Auto-Sort aktiv");
            *currentState = AUTO_SORT_STATE;
            break;

        case EVT_S2:
            lcd1602_clear();
            plattform_default_position();
            calibrate_clear();
            led_ready_on();
            lcd1602_write(1, "Manueller Modus");
            *currentState = MANUAL_SORT_STATE;
            break;

        case EVT_SYSTEM_TICK:
        case EVT_OBJECT_DETECTED:
            break;
        }
        break;

    case AUTO_SORT_STATE:
        switch (event)
        {
        case EVT_S1:
            break;
        case EVT_S2:
            led_ready_off();
            timer_systick_stop();
            plattform_sleep_position();
            turnDisplayOff();
            *currentState = OFF_STATE;
            break;
        case EVT_SYSTEM_TICK:
            check_for_objects();
            break;
        case EVT_OBJECT_DETECTED:
            do_sort();
            break;
        }
        break;

    case MANUAL_SORT_STATE:
        switch (event)
        {
        case EVT_S1:
            check_for_objects();
            break;
        case EVT_S2:
            led_ready_off();
            plattform_sleep_position();
            turnDisplayOff();
            *currentState = OFF_STATE;
            break;
        case EVT_SYSTEM_TICK:
            timer_systick_stop();
            break;
        case EVT_OBJECT_DETECTED:
            do_sort();
            break;
        }
        break;
    }
}

#pragma vector = TIMER0_B1_VECTOR
__interrupt void TIMER0_B1_ISR(void)
{
    switch (__even_in_range(TB0IV, TB0IV_TBIFG))
    {
    case TB0IV_NONE:
        break;
    case TB0IV_TBCCR1:
        break;
    case TB0IV_TBCCR2:
        break;
    case TB0IV_TBIFG:
        eventBits |= EVT_SYSTEM_TICK;
        guiSysTickCnt++;
        break;
    default:
        break;
    }
    __bic_SR_register_on_exit(LPM3_bits);
}
