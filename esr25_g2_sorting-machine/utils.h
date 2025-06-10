#ifndef servos
#define servos

#include <msp430.h> 

// TODO: Replace with timers or remove if not needed
void delay_ms(uint16_t ms) {
    while (ms--) {
        __delay_cycles(1000); // Assuming 1 MHz clock, 1000 cycles = 1 ms
    }
}

// TODO: Replace with timers or remove if not needed
void delay_s(uint16_t seconds) {
    while (seconds--) {
        delay_ms(1000); // 1000 ms = 1 second
    }
}

#endif 