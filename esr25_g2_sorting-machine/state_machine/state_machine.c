#include "state_machine.h"
#include "TCS34725/TCS34725.h"
#include "lcd1602_display/lcd1602.h"
#include "platform/platform.h"
#include "lcd1602_display/lcd1602_manager.h"
#include "timer/timer.h"
#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>

volatile uint16_t clear_ref = 0;
volatile uint16_t MIN_DELTA_CLR = 0;

uint8_t total_sorted = 0;
uint8_t red_sorted = 0;
uint8_t green_sorted = 0;
uint8_t blue_sorted = 0;

void calibrate_clear(void)
{
    uint16_t c;
    TCS_read_clear(&c);
    clear_ref = c;
    MIN_DELTA_CLR = c * 0.2f;
}

void check_for_objects()
{
    uint16_t clear;
    TCS_read_clear(&clear);

    if (clear + MIN_DELTA_CLR < clear_ref)
    {
        eventBits |= EVT_OBJECT_DETECTED;
    }
}

void do_sort(void)
{
    uint8_t r, g, b;
    TCS_get_rgb(&r, &g, &b);

    if (r > g && r > b)
    {
        writeDetectedColor(RED);
        plattform_empty_r();
        red_sorted++;
    }
    else if (g > b)
    {
        writeDetectedColor(GREEN);
        plattform_empty_g();
        green_sorted++;
    }
    else
    {
        writeDetectedColor(BLUE);
        plattform_empty_b();
        blue_sorted++;
    }
    total_sorted++;
    writeCurrentCount(total_sorted, blue_sorted, green_sorted, red_sorted);
}

Event_t getEvent (Event_t *event) {
    // if more than 7 different events are required, all 16 bits can be used
    // within the event variable; with 8 bits only, the function consumes less time
    uint16_t ii = 8;
    Event_t bitMask = 0x0080;

    if (*event != 0) {
        while (ii>0) {
            if ((bitMask & *event) > 0) {
                *event &= ~bitMask;
                return(bitMask);
            } else {
                bitMask >>= 1;
            }
            ii--;
        }
    }
    return 0;
}

void handleEvent_FSM(State_t *currentState, Event_t event)
{
    switch (*currentState)
    {
    case OFF_STATE:
        switch (event)
        {
        case EVT_S1:
            lcd1602_backlight(true);
            writeCurrentCount(total_sorted, blue_sorted, green_sorted, red_sorted);
            *currentState = DISPLAY_STATE;
            break;
        case EVT_S2:
            *currentState = MODE_SELECTION_STATE;
            break;
        case EVT_SYSTEM_TICK:
        case EVT_OBJECT_DETECTED:
            break;
        }
        break;
    
    case DISPLAY_STATE:
        switch (event)
        {
        case EVT_S1:
            clearDisplayAndBacklightOff();
            *currentState = OFF_STATE;
            break;
        case EVT_S2:
            break;
        case EVT_SYSTEM_TICK:
        case EVT_OBJECT_DETECTED:
            break;
        }
        break;

    case MODE_SELECTION_STATE:
        switch (event)
        {
        case EVT_S1:
            lcd1602_backlight(true);
            lcd1602_clear();
            lcd1602_write(0, "Auto-Sort aktiv");
            plattform_default_position();
            timer_systick_start();
            writeReady();
            calibrate_clear();
            *currentState = AUTO_SORT_STATE;
            break;

        case EVT_S2:
            lcd1602_backlight(true);
            lcd1602_clear();
            lcd1602_write(0, "Manueller Modus");
            calibrate_clear();
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
            timer_systick_stop();
            plattform_sleep_position();
            clearDisplayAndBacklightOff();
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
            break;
        case EVT_S2:
            plattform_sleep_position();
            clearDisplayAndBacklightOff();
            *currentState = OFF_STATE;
            break;
        case EVT_SYSTEM_TICK:
            break;
        case EVT_OBJECT_DETECTED:
            do_sort();
            break;
        }
        break;
    }
}
