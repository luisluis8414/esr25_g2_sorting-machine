/* ========================================================================== */
/* I2C.c                                                                      */
/* ========================================================================== */
/**
 * @file      I2C.c
 * @author    wehrberger
 * @copyright Copyright © 2025
 *
 * @brief     Implementation of a minimal blocking I²C master driver.
 *
 * The driver relies on the USCI_B0 interrupt vector to feed and drain the
 * TX/RX FIFOs.  All public API calls finish only after the ISR has issued
 * or detected the STOP condition, keeping the calling context simple and
 * synchronous.
 */

#include "I2C.h"
#include "msp430fr2355.h"
#include <stdint.h>

/* --------------------------------------------------------------------------
 *  Private data
 * -------------------------------------------------------------------------- */

/** Pointer to the byte currently being transmitted. */
static char *packet;

/** Index of the next byte to transmit. */
static unsigned int data_cnt;

/** Number of bytes contained in @ref packet. */
static unsigned int packet_length;

/** Storage for the last byte received by the ISR. */
static char data_in;

/* --------------------------------------------------------------------------
 *  Public API
 * -------------------------------------------------------------------------- */

void I2C_init(void)
{
    /* Put USCI in reset to allow configuration. */
    UCB0CTLW0 |= UCSWRST;

    /* Select SMCLK and divide by 20 → 50 kHz SCL. */
    UCB0CTLW0 |= UCSSEL_3;
    UCB0BRW = 20;

    /* I²C master, 7-bit addressing. */
    UCB0CTLW0 |= UCMODE_3 | UCMST;

    /* Automatic STOP after byte counter (UCB0TBCNT) hits zero. */
    UCB0CTLW1 |= UCASTP_2;

    /* Port mapping: P1.2 = SDA, P1.3 = SCL. */
    P1SEL1 &= ~(BIT2 | BIT3);
    P1SEL0 |= BIT2 | BIT3;

    /* Enable the module. */
    UCB0CTLW0 &= ~UCSWRST;

    /* Interrupts: RX, TX, STOP, NACK. */
    UCB0IE |= UCRXIE0 | UCTXIE0 | UCSTPIE | UCNACKIE;

    /* Global interrupt enable (driver is interrupt driven). */
    __bis_SR_register(GIE);
}

void I2C_write(uint8_t slave_addr, char data[], uint8_t length)
{
    UCB0I2CSA = slave_addr;
    packet = data;
    packet_length = length;
    data_cnt = 0;

    /* Master-transmit mode. */
    UCB0CTLW0 |= UCTR;
    UCB0TBCNT = length;

    /* Generate START, then go to sleep until STOP. */
    UCB0CTLW0 |= UCTXSTT;
    LPM0;
}

char I2C_read_reg(uint8_t slave_addr, uint8_t reg_addr)
{
    /* Send register address first. */
    char addr_buf[1] = {reg_addr};
    I2C_write(slave_addr, addr_buf, 1);

    /* Switch to receive mode and ask for 1 byte. */
    UCB0CTLW0 &= ~UCTR;
    UCB0TBCNT = 1;

    UCB0CTLW0 |= UCTXSTT; /* Repeated START. */
    LPM0;                 /* Wait for STOP → ISR wakes us up. */

    return data_in;
}

/* --------------------------------------------------------------------------
 *  Interrupt service routine
 * -------------------------------------------------------------------------- */
/**
 * @brief Unified ISR for all USCI_B0 I²C events.
 *
 * Only four interrupt causes are currently handled:
 *   - UCNACKIFG : flagging a missing ACK (debug hook only)
 *   - UCSTPIFG  : STOP detected → leave LPM0
 *   - UCRXIFG0  : one byte received
 *   - UCTXIFG0  : transmit buffer ready for next byte
 *
 * All other causes fall through to the default case.
 */
#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void)
{
    switch (__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG)) {
        case USCI_I2C_UCNACKIFG:
            /* Right now only for debug */
            __no_operation();
            break;

        case USCI_I2C_UCSTPIFG:
            /* STOP → wake the CPU (exit LPM0). */
            __bic_SR_register_on_exit(LPM0_bits);
            break;

        case USCI_I2C_UCRXIFG0:
            /* Store the received byte. */
            data_in = UCB0RXBUF;
            break;

        case USCI_I2C_UCTXIFG0:
            /* Feed the next data byte or finish the transfer. */
            UCB0TXBUF = packet[data_cnt++];
            if (data_cnt >= packet_length)
                data_cnt = 0; /* Reset for next transaction. */
            break;

        default:
            /* Unhandled vector – nothing to do. */
            break;
    }
}
