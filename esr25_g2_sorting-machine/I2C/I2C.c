/* ========================================================================== */
/* I2C.c                                                                      */
/* ========================================================================== */
/**
 * @file      I2C.c
 * @author    wehrberger
 * @date      31.05.2025
 *
 * @brief     Implementierung eines minimalen blockierenden I²C-Master-Treibers.
 */

#include "I2C.h"
#include "msp430fr2355.h"
#include <stdint.h>
#include <stdbool.h>

/** Pointer auf das aktuell zu übertragende Byte. */
static char *packet;

/** Index des nächsten zu übertragenden Bytes. */
static unsigned int data_cnt;

/** Anzahl der Bytes in @ref packet. */
static unsigned int packet_length;

/** Speicher für das zuletzt vom ISR empfangene Byte. */
static char data_in;

static volatile bool i2c_done = false;

void I2C_init(void)
{
    // USCI in Reset setzen um Konfiguration zu ermöglichen
    UCB0CTLW0 |= UCSWRST;

    // SMCLK wählen und durch 20 teilen → 50 kHz SCL
    UCB0CTLW0 |= UCSSEL_3;
    UCB0BRW = 20;

    // I²C Master, 7-Bit Adressierung
    UCB0CTLW0 |= UCMODE_3 | UCMST;

    // Automatischer STOP nach Byte-Zähler (UCB0TBCNT) erreicht Null
    UCB0CTLW1 |= UCASTP_2;

    // Port-Mapping: P1.2 = SDA, P1.3 = SCL
    P1SEL1 &= ~(BIT2 | BIT3);
    P1SEL0 |= BIT2 | BIT3;

    // Modul aktivieren
    UCB0CTLW0 &= ~UCSWRST;

    // Interrupts: RX, TX, STOP, NACK
    UCB0IE |= UCRXIE0 | UCTXIE0 | UCSTPIE | UCNACKIE;
}

void I2C_write(uint8_t slave_addr, char data[], uint8_t length)
{
    UCB0I2CSA = slave_addr;
    packet = data;
    packet_length = length;
    data_cnt = 0;

    // Master-Transmit-Modus
    UCB0CTLW0 |= UCTR;
    UCB0TBCNT = length;

    // START generieren, dann schlafen bis STOP
    UCB0CTLW0 |= UCTXSTT;
    
    i2c_done = false;
    while (!i2c_done){
        __bis_SR_register(LPM3_bits | GIE); // Warten auf STOP → ISR weckt uns auf
    }
}

char I2C_read_reg(uint8_t slave_addr, uint8_t reg_addr)
{
    // Registeradresse zuerst senden
    char addr_buf[1] = {reg_addr};
    I2C_write(slave_addr, addr_buf, 1);

    // In Empfangsmodus wechseln und 1 Byte anfordern
    UCB0CTLW0 &= ~UCTR;
    UCB0TBCNT = 1;

    UCB0CTLW0 |= UCTXSTT; // Repeated START
        
    i2c_done = false;
    while (!i2c_done){
        __bis_SR_register(LPM3_bits | GIE);  // Warten auf STOP → ISR weckt uns auf
    }           

    return data_in;
}

/* ========================================================================== */
/* Interrupt Service Routine                                                  */
/* ========================================================================== */

/**
 * @brief Vereinheitlichte ISR für alle USCI_B0 I²C-Ereignisse.
 *
 * Nur vier Interrupt-Ursachen werden derzeit behandelt:
 *   - UCNACKIFG : Kennzeichnet fehlendes ACK (nur Debug-Hook)
 *   - UCSTPIFG  : STOP erkannt → LPM3 verlassen
 *   - UCRXIFG0  : Ein Byte empfangen
 *   - UCTXIFG0  : Sendepuffer bereit für nächstes Byte
 *
 * Alle anderen Ursachen fallen durch zum default.
 */
#pragma vector = EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void)
{
    switch (__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG)) {
        case USCI_I2C_UCNACKIFG:
            // Derzeit nur für Debug
            i2c_done = true;
            __bic_SR_register_on_exit(LPM3_bits);
            __no_operation();
            break;

        case USCI_I2C_UCSTPIFG:
            // STOP → CPU aufwecken (LPM3 verlassen)
            i2c_done = true;
            __bic_SR_register_on_exit(LPM3_bits);
            break;

        case USCI_I2C_UCRXIFG0:
            // Empfangenes Byte speichern
            data_in = UCB0RXBUF;
            break;

        case USCI_I2C_UCTXIFG0:
            // Nächstes Datenbyte senden oder Übertragung beenden
            UCB0TXBUF = packet[data_cnt++];
            if (data_cnt >= packet_length)
                data_cnt = 0; // Für nächste Transaktion zurücksetzen
            break;

        default:
            // Unbehandelter Vektor – nichts zu tun
            break;
    }
}
