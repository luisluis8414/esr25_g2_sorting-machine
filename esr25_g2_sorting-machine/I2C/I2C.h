/* ========================================================================== */
/* I2C.h                                                                      */
/* ========================================================================== */
/**
 * @file      I2C.h
 * @author    wehrberger
 * @copyright Copyright © 2025
 *
 * @brief     Minimal master-mode I²C driver for the MSP430FR2355.
 *
 * The module offers three blocking helper functions:
 *   - I2C_init()         – configure USCI B0 for 50 kHz I²C master
 *   - I2C_write()        – transmit an arbitrary number of bytes
 *   - I2C_read_reg()     – read a single byte register
 *
 * Communication is handled in the background by the EUSCI_B0 interrupt
 * service routine.  The public functions simply put the CPU into LPM0
 * until the corresponding STOP condition has been generated/received.
 */

#ifndef I2C_I2C_H_
#define I2C_I2C_H_

#include <stdint.h>

/**
 * @brief Initialise USCI B0 for 7-bit I²C master operation.
 *
 * This routine assumes that SMCLK is already running at **1 MHz**.
 * With the divider set to 20 the resulting SCL frequency is therefore
 * 50 kHz.
 *
 * The peripheral is configured for:
 *   - Clock source:  SMCLK (1 MHz expected)
 *   - Bit-rate:      50 kHz (SMCLK / 20)
 *   - Automatic STOP generation via byte counter
 *
 * Ports P1.2 (SDA) and P1.3 (SCL) are multiplexed to their I²C function.
 *
 * The function enables the global interrupt flag so that the ISR can
 * serve I²C events.
 */
void I2C_init(void);

/**
 * @brief Write a contiguous block of data to a slave.
 *
 * The first byte in @p data is interpreted as the target register/
 * memory address inside the slave device.  All following bytes are
 * written to increasing addresses.
 *
 * @param[in] slave_addr  7-bit slave address (left-aligned, i.e. the
 *                        LSB is ignored by the hardware).
 * @param[in] data        Pointer to the transmit buffer.
 * @param[in] length      Number of bytes to send (≥ 1).
 *
 * @note The function blocks the CPU by entering LPM0.  Control is
 *       returned once the STOP condition has been sent.
 */
void I2C_write(uint8_t slave_addr, char data[], uint8_t length);

/**
 * @brief Read one byte from a given register of a slave.
 *
 * A write-then-read transaction is performed:
 *   1.  Send the register address (no STOP).
 *   2.  Repeated START followed by one data byte reception.
 *
 * @param[in] slave_addr  7-bit slave address (left-aligned).
 * @param[in] reg_addr    Register address inside the slave.
 *
 * @return The value stored in the requested register.
 *
 * @warning The routine is blocking and will enter LPM0 until the byte
 *          has been received.
 */
char I2C_read_reg(uint8_t slave_addr, uint8_t reg_addr);


#endif /* I2C_I2C_H_ */
