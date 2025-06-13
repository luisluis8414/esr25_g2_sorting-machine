#include <msp430.h> 
#include <driverlib.h>
#include "I2C/I2C.h"
#include "lcd1602_display/lcd1602.h"

// Der PCF8574 Baustein hat folgende 7-Bit Adresse: 0x3F (dez 63) : 0011 1111
// Somit ergibt sich f�r Schreibzugriffe folgender Adresswert: 0111 1110 = 0x7E
// Die Driverlib erwartet jedoch die Angabe der tats�chlichen 7-Bit Adresse, also 0x3F
#define SLAVE_ADDRESS_LCD 0x3F
void init_gpio(void);
void init_timer(void);
void init_cs(void);
void init_i2c(void);
void sleep(uint16_t ms);



/**
 * main.c
 */
int main(void)
{
	const char s1[17] = "Sortiermaschine ";
	const char s2[17] = "ist bereit......";
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
    init_timer();
	//I2C_init();
    init_cs();
	init_i2c();
    PMM_unlockLPM5();
    __bis_SR_register(GIE);     //Enable global interrupt

    lcd1602_init();
    lcd1602_backlight(true);

    while (1) {
        lcd1602_write(1,s1);
        lcd1602_write(2,s2);
        sleep(1000);
        lcd1602_clear();
        sleep(1000);
        lcd1602_backlight(!lcd1602_getBacklightState());
        sleep(1000);
    }
	
	return 0;
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

// Die Funktion kehrt erst dann zur�ck, wenn Timer0_B3 die angegebene Anzahl von
// ms absolviert hat. Unbedingt beachten: Diese einfache Implementierung eines
// sleep-Timers funktioniert nur, solange kein Interrupt nesting verwendet wird.
// W�hrend der sleep-Perioden des Timer k�nnen andere Interrupt-Routinen ausgef�hrt
// werden; diese d�rfen aber nicht in die main loop zur�ckkehren, sondern m�ssen
// den aktuellen sleep mode beim Verlassen der ISR wieder herstellen.
void sleep(uint16_t ms) {
    while (ms--) LPM0;
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


// TimerB0 Interrupt Vector (TBxIV) handler
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR(void)
{
    __bic_SR_register_on_exit(LPM0_bits);
}

