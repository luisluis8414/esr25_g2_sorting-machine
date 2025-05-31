#include "msp430fr2355.h"
#include <stdint.h>
#include "../include/I2C.h"

static char* packet;
static unsigned int data_cnt;
static unsigned int packet_length;
static char data_in;

// init, start in write mode
void init_I2C(void) {
    UCB0CTLW0 |= UCSWRST;    // put into SW reset

    UCB0CTLW0 |= UCSSEL_3;  // choose SMCLK

    UCB0BRW = 20;            // sets prescaler to 50khz -- 1Mhz / 20 

    UCB0CTLW0 |= UCMODE_3;  // put into i2c mode
    UCB0CTLW0 |= UCMST;     // put into master mode

    UCB0CTLW1 |= UCASTP_2;  // auto stop mode based on byte counter UCB0TBCNT

    //-- configure ports
    P1SEL1 &= ~BIT3;        // P1.3 = SCL
    P1SEL0 |= BIT3;

    P1SEL1 &= ~BIT2;        // P1.2 = SDA
    P1SEL0 |= BIT2;

    UCB0CTLW0 &= ~UCSWRST;  // take out of SW reset

    //-- enable IRQs
    UCB0IE |= UCRXIE0 | UCTXIE0 | UCSTPIE;      // enable READ, WRITE, PAUSE Interrupts
    __bis_SR_register(GIE);                     // global interrupt enable
}

// simple I2C write, first entry in data is addr, rest is data, so {startaddr, data for startaddr, data for startaddr + 1,....}
void write_I2C(uint8_t slave_addr, char data[], int length) {
    UCB0I2CSA = slave_addr;

    packet = data;
    packet_length = length;

    UCB0CTLW0 |= UCTR; 
    UCB0TBCNT = length;

    data_cnt = 0;   

    UCB0CTLW0 |= UCTXSTT; // generate start bit

    LPM0;
}

void read_I2C(uint8_t slave_addr, uint8_t reg_addr) {
    char addr_buf[1] = { reg_addr };

    write_I2C(slave_addr, addr_buf, 1);

    UCB0CTLW0 &= ~UCTR; 

    UCB0TBCNT = 1;

    UCB0CTLW0 |= UCTXSTT; // generate start bit

    LPM0;
}


//--------------------------------------------
//-- ISR
#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void)
{
      switch (__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG)) {
        // case USCI_NONE: break;
        // case USCI_I2C_UCALIFG: break;       // Arbitration lost
        // case USCI_I2C_UCNACKIFG: break;     // NACK received
        // case USCI_I2C_UCSTTIFG: break;      // START condition received
         case USCI_I2C_UCSTPIFG:                  // STOP condition received
            // bic = bit clear
            __bic_SR_register_on_exit(LPM0_bits);
            break;      
        // case USCI_I2C_UCRXIFG3: break;
        // case USCI_I2C_UCTXIFG3: break;
        // case USCI_I2C_UCRXIFG2: break;
        // case USCI_I2C_UCTXIFG2: break;
        // case USCI_I2C_UCRXIFG1: break;
        // case USCI_I2C_UCTXIFG1: break;
        case USCI_I2C_UCRXIFG0:     
            data_in = UCB0RXBUF;
            break;
        case USCI_I2C_UCTXIFG0:
            if (data_cnt == packet_length - 1) {
                char data = packet[data_cnt];
                UCB0TXBUF = data;
                data_cnt = 0;
            }else {
                UCB0TXBUF = packet[data_cnt];
                data_cnt++;
            }
            break;
        // case USCI_I2C_UCBCNTIFG: break;
        // case USCI_I2C_UCCLTOIFG: break;
        // case USCI_I2C_UCBIT9IFG: break;
        default: break;
    }
}