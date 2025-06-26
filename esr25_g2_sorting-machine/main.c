#include <msp430.h> 
#include <stdint.h>
#include <driverlib.h>

#include "PCA9685/PCA9685.h"
#include "platform/platform.h"
#include "lcd1602_display/lcd1602_manager.h"
#include "lcd1602_display/lcd1602.h"
#include "I2C/I2C.h"
#include "timer/timer.h"
#include "Board.h"
#include "button/button.h"
#include "state_machine/state_machine.h"

extern Event_t eventBits;

//Target frequency for MCLK in kHz
#define CS_MCLK_DESIRED_FREQUENCY_IN_KHZ   1000
//MCLK/FLLRef Ratio
#define CS_MCLK_FLLREF_RATIO   30
// Der PCF8574 Baustein hat folgende 7-Bit Adresse: 0x3F (dez 63) : 0011 1111
// Somit ergibt sich f r Schreibzugriffe folgender Adresswert: 0111 1110 = 0x7E
// Die Driverlib erwartet jedoch die Angabe der tats chlichen 7-Bit Adresse, also 0x3F
#define SLAVE_ADDRESS_LCD 0x3F


void init() {
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;  
    init_cs();
    init_i2c();
	//PCA9685_init();
    button_init();
    timer_init();
    lcd1602_init();
    __bis_SR_register(GIE); // Global interrupt enable
}

int main(void)
{
    State_t currentState = OFF_STATE;
    Event_t event = EVT_NO_EVENT;

    init();

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
    
    /*plattform_default_position();

    timer_sleep_ms(2000);

    plattform_empty_r();

    timer_sleep_ms(2000);

    plattform_default_position();
    while (1)
    {
        if (flag_calib) {                    
            flag_calib = 0;
            calibrate_clear();               
            continue;                       
        }

        uint16_t clear;
        TCS_read_clear(&clear);

        if (clear + MIN_DELTA_CLR < clear_ref) {
            do_sort();
        }

    timer_sleep_ms(2000);

    plattform_default_position();

    timer_sleep_ms(2000);

    plattform_empty_b();

    timer_sleep_ms(2000);

    plattform_default_position();

    timer_sleep_ms(2000);
    */
    }
}

void init_timer(void) {
    static Timer_B_initUpModeParam param = {0};

    param.clockSource = TIMER_B_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_1;
    param.timerPeriod = 999;        // wenn 1000 Taktimpulse gez�hlt
                                    // wurden, erfolgt ein Interrupt
                                    // Periodendauer somit 1ms
    param.timerInterruptEnable_TBIE =
            TIMER_B_TBIE_INTERRUPT_DISABLE;         // no interrupt on 0x0000
    param.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_B_CAPTURECOMPARE_INTERRUPT_ENABLE;    // interrupt on TRmax
    param.timerClear = TIMER_B_DO_CLEAR;
    param.startTimer = true;

    // start Timer:
    Timer_B_initUpMode(TB0_BASE, &param);
}

void init_cs(void) {
    //Set DCO FLL reference = REFO
    CS_initClockSignal(CS_FLLREF, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    //Set ACLK = REFO
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    //Create struct variable to store proper software trim values
    CS_initFLLParam param = {0};

    //Set Ratio/Desired MCLK Frequency, initialize DCO, save trim values
    CS_initFLLCalculateTrim(CS_MCLK_DESIRED_FREQUENCY_IN_KHZ, CS_MCLK_FLLREF_RATIO, &param);

    //Clear all OSC fault flag
    CS_clearAllOscFlagsWithTimeout(1000);

    //Enable oscillator fault interrupt
    SFR_enableInterrupt(SFR_OSCILLATOR_FAULT_INTERRUPT);
}

void init_i2c(void) {
    EUSCI_B_I2C_initMasterParam param = {0};

    // Configure Pins for I2C
    /*
    * Select Port 1
    * Set Pin 2, 3 to input with function, (UCB0SIMO/UCB0SDA, UCB0SOMI/UCB0SCL).
    */

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_UCB0SCL, GPIO_PIN_UCB0SCL, GPIO_FUNCTION_UCB0SCL);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_UCB0SDA, GPIO_PIN_UCB0SDA, GPIO_FUNCTION_UCB0SDA);

    param.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    param.i2cClk = CS_getSMCLK();
    param.dataRate = EUSCI_B_I2C_SET_DATA_RATE_100KBPS;
    param.byteCounterThreshold = 1;
    param.autoSTOPGeneration = EUSCI_B_I2C_NO_AUTO_STOP;
    EUSCI_B_I2C_initMaster(EUSCI_B0_BASE, &param);

    //Specify slave address
    EUSCI_B_I2C_setSlaveAddress(EUSCI_B0_BASE, SLAVE_ADDRESS_LCD);
    //Set in transmit mode
    EUSCI_B_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    //Enable I2C Module to start operations
    EUSCI_B_I2C_enable(EUSCI_B0_BASE);
}

#pragma vector=UNMI_VECTOR
__interrupt void NMI_ISR(void)
{
    uint16_t status;
    do {
        // If it still can't clear the oscillator fault flags after the timeout,
        // trap and wait here.
        status = CS_clearAllOscFlagsWithTimeout(1000);
    } while(status != 0);
}

