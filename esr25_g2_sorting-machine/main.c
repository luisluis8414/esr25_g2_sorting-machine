#include <msp430.h> 
#include "PCA9685/PCA9685.h"
#include <stdint.h>
#include <stdbool.h>
#include "platform/platform.h"
#include "I2C/I2C.h"
#include "timer/timer.h"
#include "TCS34725/TCS34725.h"

/* ========================================================================== */
/* State Machine Definitions                                                  */
/* ========================================================================== */

typedef enum {
    OFF_STATE,
    ON_STATE
} State_t;

#define EVT_NO_EVENT        0x00
#define EVT_SYSTEM_TICK     BIT0
#define EVT_BUTTON_PRESS    BIT1
#define EVT_OBJECT_DETECTED BIT2

typedef uint16_t Event_t;
Event_t eventBits = EVT_NO_EVENT;

/* ========================================================================== */
/* Global Variables                                                           */
/* ========================================================================== */

volatile uint16_t clear_ref     = 0;   // Default-Schwelle beim Start
volatile uint16_t MIN_DELTA_CLR = 0;   // wie viel dunkler es mindestens sein muss

void idleTask() 
{
    // Optional: Add any idle processing here
    return;
}

/* ========================================================================== */
/* Application Functions                                                      */
/* ========================================================================== */

static void calibrate_clear(void)
{
    uint16_t c;
    TCS_read_clear(&c);   // LED aus – Umgebungslicht-Referenz
    clear_ref = c;
    MIN_DELTA_CLR = c * 0.5f;
}

void check_for_objects(void)
{
    uint16_t clear;
    TCS_read_clear(&clear);

    if (clear + MIN_DELTA_CLR < clear_ref) {
        eventBits |= EVT_OBJECT_DETECTED;
    }
}

void do_sort(void)
{
    uint8_t r, g, b;
    TCS_get_rgb(&r, &g, &b);

    if (r > g && r > b) {
        plattform_empty_r();  // Red is dominant
    }
    else if (g > b) {
        plattform_empty_g();  // Green is dominant
    }
    else {
        plattform_empty_b();  // Blue is dominant
    }
}

/* ========================================================================== */
/* Initialization                                                             */
/* ========================================================================== */

void init(void)
{
    I2C_init();
    PCA9685_init();
    timer_init();
    TCS_init();
    
    // Initialize system tick for 1000ms period
    timer_systick_init(1000);

    plattform_sleep_position();

    calibrate_clear();

    // --- Button P2.3 setup with interrupt ---
    P2DIR &= ~BIT3;     // input
    P2REN |= BIT3;      // enable pull-up/down
    P2OUT |= BIT3;      // pull-up

    P2IES |= BIT3;      // falling edge (press)
    P2IFG &= ~BIT3;     // clear flag
    P2IE |= BIT3;       // enable interrupt
}

/* ========================================================================== */
/* Event Handling Functions                                                   */
/* ========================================================================== */

Event_t getEvent(Event_t *event) 
{
    uint16_t ii = 8;
    Event_t bitMask = 0x0080;

    if (*event != 0) {
        while (ii > 0) {
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
    State_t state = *currentState;

    switch (state) {
        case OFF_STATE:
            switch (event) {
                case EVT_NO_EVENT:
                    break;
                case EVT_SYSTEM_TICK:
                    // No reaction on system tick in OFF state
                    break;
                case EVT_BUTTON_PRESS:
                    // Button press turns machine ON
                    calibrate_clear();
                    plattform_default_position();
                    timer_systick_start();
                    *currentState = ON_STATE;
                    break;
                case EVT_OBJECT_DETECTED:
                    // No reaction to objects in OFF state
                    break;
                default:
                    break;
            }
            break;

        case ON_STATE:
            switch (event) {
                case EVT_NO_EVENT:
                    break;
                case EVT_SYSTEM_TICK:
                    // Check for objects periodically
                    check_for_objects();
                    break;
                case EVT_BUTTON_PRESS:
                    // Button press turns machine OFF
                    timer_systick_stop();
                    plattform_sleep_position();
                    *currentState = OFF_STATE;
                    break;
                case EVT_OBJECT_DETECTED:
                    // Sort the detected object
                    do_sort();
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
    }
}

/* ========================================================================== */
/* Main Function                                                              */
/* ========================================================================== */

int main(void)
{
    State_t currentState = OFF_STATE;
    Event_t event = EVT_NO_EVENT;

    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    init();
    
    __enable_interrupt();

    while (true) {
        event = getEvent(&eventBits);
        if (event != EVT_NO_EVENT) {
            while (event > EVT_NO_EVENT) {
                handleEvent_FSM(&currentState, event);
                event = getEvent(&eventBits);
            }
            idleTask();
        }
        LPM3;
    }
}

/* ========================================================================== */
/* Interrupt Service Routines                                                 */
/* ========================================================================== */

/**
 * @brief ISR for Port2 - Button Press
 */
#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR(void)
{
    if (P2IFG & BIT3) {
        P2IFG &= ~BIT3;                     // clear flag
        eventBits |= EVT_BUTTON_PRESS;
        __bic_SR_register_on_exit(LPM3_bits); // wake main loop
    }
}

/**
 * @brief Timer_B0 Overflow/CCR1-CCR6 Interrupt Service Routine.
 * 
 * Behandelt System-Tick Overflow Interrupts.
 */
#pragma vector = TIMER0_B1_VECTOR
__interrupt void TIMER0_B1_ISR(void)
{
    switch(__even_in_range(TB0IV, TB0IV_TBIFG))
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
            if(!TIMER_SLEEP_MODE){
                __bic_SR_register_on_exit(LPM3_bits);
            }
            break;
        default:
            break;
    }
}