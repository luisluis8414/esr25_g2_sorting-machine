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

Event_t eventBits = 0;

void init_all_ports(void)
{
    P1OUT = 0x00; P1DIR = 0xFF;
    P2OUT = 0x00; P2DIR = 0xFF;
    P3OUT = 0x00; P3DIR = 0xFF;
    P4OUT = 0x00; P4DIR = 0xFF;
    P5OUT = 0x00; P5DIR = 0xFF;
    P6OUT = 0x00; P6DIR = 0xFF;

    PAOUT = 0x00; PADIR = 0xFF;
    PBOUT = 0x00; PBDIR = 0xFF;
    PCOUT = 0x00; PCDIR = 0xFF;
}

void init(void)
{
    init_all_ports();

    I2C_init();
    PCA9685_init();
    timer_init();
    TCS_init();
    button_init();
    lcd1602_init();


    timer_systick_init(1000);

    plattform_sleep_position();
    turnDisplayOff();

    __enable_interrupt();
}

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
