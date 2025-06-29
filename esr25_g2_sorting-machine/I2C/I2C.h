/* ========================================================================== */
/* I2C.h                                                                      */
/* ========================================================================== */
/**
 * @file      I2C.h
 * @author    wehrberger
 * @date      31.05.2025
 *
 * @brief     Minimaler Master-Modus I²C-Treiber für den MSP430FR2355.
 *
 * Das Modul bietet drei blockierende Hilfsfunktionen:
 *   - I2C_init()         – USCI B0 für 50 kHz I²C Master konfigurieren
 *   - I2C_write()        – Beliebige Anzahl von Bytes übertragen
 *   - I2C_read_reg()     – Ein einzelnes Byte-Register lesen
 *
 * Die Kommunikation wird im Hintergrund von der EUSCI_B0 Interrupt-Service-
 * Routine behandelt. Die Funktionen versetzen die CPU
 * in LPM0 bis die entsprechende STOP-Bedingung generiert wurde.
 *
 * @note Detzt voraus, dass SMCLK mit 1 MHz läuft. Vor Verwendung
 *       anderer Funktionen muss die init() Methode aufgerufen werden.
 */

#ifndef I2C_I2C_H_
#define I2C_I2C_H_

#include <stdint.h>

/**
 * @brief Initialisiert USCIB0 für 7-Bit I²C-Master-Betrieb.
 *
 * Diese Routine nimmt an, dass SMCLK bereits mit **1 MHz** läuft.
 * Mit dem auf 20 gesetzten Teiler beträgt die resultierende SCL-Frequenz
 * dann 50 kHz.
 *
 * Das wird USCIB0 konfiguriert für:
 *   - Taktquelle:  SMCLK (1 MHz erwartet)
 *   - Bitrate:     50 kHz (SMCLK / 20)
 *   - Automatische STOP-Generierung über Byte-Zähler
 *
 * Ports P1.2 (SDA) und P1.3 (SCL) werden auf ihre I²C-Funktion gemultiplext.
 *
 * Aktiviert das globale Interrupt-Flag, damit die ISR
 * I²C-Ereignisse bedienen kann.
 */
void I2C_init(void);

/**
 * @brief Schreibt einen zusammenhängenden Datenblock an einen Slave.
 *
 * Das erste Byte in @p data wird als Zielregister/Speicheradresse
 * innerhalb des Slave-Geräts interpretiert. Alle folgenden Bytes werden
 * an aufsteigende Adressen geschrieben.
 *
 * @param[in] slave_addr  7-Bit Slave-Adresse (links ausgerichtet, d.h. das
 *                        LSB wird von der Hardware ignoriert).
 * @param[in] data        Pointer auf den Sendepuffer.
 * @param[in] length      Anzahl der zu sendenden Bytes (≥ 1).
 *
 * @note Die Funktion blockiert die CPU durch Eintritt in den LPM3. Die Kontrolle
 *       wird zurückgegeben, sobald die STOP-Bedingung oder NACK gesendet wurde.
 */
void I2C_write(uint8_t slave_addr, char data[], uint8_t length);

/**
 * @brief Liest ein Byte aus einem gegebenen Register eines Slaves.
 *
 * Eine Schreib-dann-Lese-Transaktion wird durchgeführt:
 *   1.  Registeradresse senden (kein STOP).
 *   2.  Repeated START gefolgt von einem Datenbyte-Empfang.
 *
 * @param[in] slave_addr  7-Bit Slave-Adresse (links ausgerichtet).
 * @param[in] reg_addr    Registeradresse innerhalb des Slaves.
 *
 * @return Der im angeforderten Register gespeicherte Wert.
 *
 * @warning Die Routine ist blockierend und wird LPM0 betreten bis das Byte
 *          empfangen wurde.
 */
char I2C_read_reg(uint8_t slave_addr, uint8_t reg_addr);

#endif /* I2C_I2C_H_ */
