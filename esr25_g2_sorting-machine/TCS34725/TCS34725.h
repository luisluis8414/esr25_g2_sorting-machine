/* ========================================================================== */
/* TCS34725.h                                                                 */
/* ========================================================================== */
/**
 * @file      TCS34725.h
 * @author    qunaku, wehrberger
 * @date      10.06.2025
 *
 * @brief     TCS34725 RGB-Farbsensor Treiber für Embedded-Anwendungen.
 *
 * Dieses Modul bietet eine einfache Schnittstelle zum TCS34725 RGB-Farbsensor
 * über I²C-Kommunikation. Der Treiber bietet grundlegende Funktionalität für:
 *   - tcs_init()         – Sensor mit 100ms Integration, 4x Verstärkung initialisieren
 *   - tcs_read_rgbc()    – Rohe 16-Bit RGBC-Werte vom Sensor lesen
 *   - tcs_get_rgb888()   – Rohe Werte zu 8-Bit RGB für Farberkennung konvertieren
 *
 * Die Konvertierung zu 8-Bit RGB ist vereinfacht und für Farbunterscheidung
 * optimiert, nicht für akkurate Farbwiedergabe. Sie erhält relative Farbverhältnisse
 * bei und stellt sicher, dass alle Werte in den 8-Bit Bereich passen ohne Divisionen.
 *
 * @note Dieser Treiber benötigt eine funktionsfähige I²C-Schnittstelle und nimmt an,
 *       dass der Sensor am Standard-I²C-Bus angeschlossen ist.
 */

#ifndef TCS_H_
#define TCS_H_

#include <stdint.h>

/* ========================================================================== */
/* TCS34725 Konfigurationskonstanten                                          */
/* ========================================================================== */

/** @brief I²C-Slave-Adresse des TCS34725 Sensors. */
#define TCS34725_ADDRESS     0x29

/** @brief Command-Bit erforderlich für alle Registerzugriffe. */
#define TCS34725_COMMAND_BIT 0x80

/* ========================================================================== */
/* TCS34725 Registeradressen                                                  */
/* ========================================================================== */

/** @brief Enable-Register – Stromsteuerung und Funktionsaktivierung. */
#define TCS34725_ENABLE      0x00

/** @brief ATIME-Register – ADC-Integrationszeitkonfiguration. */
#define TCS34725_ATIME       0x01

/** @brief Control-Register – Analoge Verstärkungseinstellung. */
#define TCS34725_CONTROL     0x0F

/** @brief Clear-Kanal Datenregister (Low-Byte). */
#define TCS34725_CDATAL      0x14

/** @brief Rot-Kanal Datenregister (Low-Byte). */
#define TCS34725_RDATAL      0x16

/** @brief Grün-Kanal Datenregister (Low-Byte). */
#define TCS34725_GDATAL      0x18

/** @brief Blau-Kanal Datenregister (Low-Byte). */
#define TCS34725_BDATAL      0x1A

/* ========================================================================== */
/* Hilfsmakros                                                                */
/* ========================================================================== */

/**
 * @brief Generiert Command-Byte für Registerzugriff.
 *
 * Kombiniert die Registeradresse mit dem obligatorischen Command-Bit für
 * ordnungsgemäße I²C-Kommunikation mit dem TCS34725.
 *
 * @param reg Registeradresse für Zugriff.
 * @return Command-Byte bereit für I²C-Übertragung.
 */
#define CMD(reg) (TCS34725_COMMAND_BIT | (reg))

/* ========================================================================== */
/* Funktionen                                                                 */
/* ========================================================================== */

/**
 * @brief Initialisiert den TCS34725 Farbsensor.
 *
 * Konfiguriert den Sensor mit Standardeinstellungen:
 *   - Integrationszeit: 100ms (ausgewogene Genauigkeit und Geschwindigkeit)
 *   - Verstärkung: 4x (moderate Empfindlichkeit)
 *   - Strom: EIN mit aktiviertem ADC
 *
 * @note 2,4ms + Integrationszeit nach der Initialisierung bevor
 *       gültige Messungen gelesen werden.
 */
void TCS_init(void);

/**
 * @brief Liest einen 16-Bit Wert vom TCS aus.
 *
 * Liest zwei aufeinanderfolgende Register und kombiniert sie zu einem 16-Bit Wert.
 * Little-Endian Reihenfolge
 *
 * @param[in] reg Startregisteradresse (Low-Byte).
 * @return Kombinierter 16-Bit Wert aus dem Registerpaar.
 */
uint16_t TCS_read_16bit_reg(uint8_t reg);

/**
 * @brief Liest rohe RGBC-Werte vom TCS34725 Sensor.
 *
 * Ruft alle vier raw 16-Bit Farbkanalwerte vom Sensor ab.
 * Werte repräsentieren Lichtintensität und hängen von Integrationszeit
 * und Verstärkungseinstellungen ab.
 *
 * @param[out] r Pointer zum Speichern des Rot-Kanal Werts.
 * @param[out] g Pointer zum Speichern des Grün-Kanal Werts.
 * @param[out] b Pointer zum Speichern des Blau-Kanal Werts.
 * @param[out] c Pointer zum Speichern des Clear-Kanal Werts.
 */
void tcs_read_rgbc(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c);

/**
 * @brief Konvertiert rohe Sensorwerte zu 8-Bit RGB für Farberkennung.
 *
 * Führt eine vereinfachte Konvertierung von 16 Bit Rohwerten zu 8 Bit RBG Werten durch.
 * Dies ist KEINE akkurate RGB-Konvertierung!
 * Aber ausreichend für grundlegende Farbunterscheidung (rot vs grün vs blau).
 *
 * Die Konvertierung:
 *   - Skaliert alle Kanäle proportional um Overflow zu verhindern
 *   - Verwendet Bit-Verschiebungen statt Division für Effizienz
 *
 * @param[out] r8 Pointer zum Speichern des 8-Bit Rot-Werts.
 * @param[out] g8 Pointer zum Speichern des 8-Bit Grün-Werts.
 * @param[out] b8 Pointer zum Speichern des 8-Bit Blau-Werts.
 *
 * @warning Dies ist nicht geeignet für akkurate Farbbestitmmung.
 */
void TCS_get_rgb888(uint8_t *r8, uint8_t *g8, uint8_t *b8);

#endif /* TCS_H_ */