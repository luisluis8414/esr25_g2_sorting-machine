#include <msp430.h> 
#include <stdint.h>

#include "PCA9685/PCA9685.h"
#include "platform/platform.h"
#include "lcd1602_display/lcd1602_manager.h"
#include "lcd1602_display/lcd1602.h"
#include "I2C/I2C.h"
#include "timer/timer.h"
#include "button/button.h"
#include "state_machine/state_machine.h"

extern Event_t eventBits;

void init() {
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;  

    I2C_init();

    timer_init();
    
	PCA9685_init();
    button_init();
    lcd1602_init();

    __bis_SR_register(GIE); // Global interrupt enable
}

int main(void)
{
    State_t currentState = OFF_STATE;
    Event_t event = EVT_NO_EVENT;

    init();

    lcd1602_backlight(true);
    lcd1602_write(1, "Hallo Welt");
    lcd1602_write(2, "Ich lebe!");

    while (1) {
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

