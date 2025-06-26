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
 *   - TCS_init()         – Sensor mit 100ms Integration, 4x Verstärkung initialisieren
 *   - TCS_read_16bit_reg() – 16-Bit Register vom Sensor lesen
 *   - TCS_read_clear()   – Clear-Kanal-Wert lesen
 *   - TCS_get_rgbc()     – RGBC-Werte lesen und zu 8-Bit RGB konvertieren
 *   - TCS_led_on/off()   – LED-Steuerung
 *
 * Die Konvertierung zu 8-Bit RGB ist vereinfacht und für Farbunterscheidung
 * optimiert, nicht für akkurate Farbwiedergabe. Sie erhält relative Farbverhältnisse
 * und stellt sicher, dass alle Werte in den 8-Bit Bereich passen.
 *
 * @note Dieser Treiber benötigt eine funktionsfähige I²C-Schnittstelle und nimmt an,
 *       dass der Sensor am Standard-I²C-Bus angeschlossen ist. Vor Verwendung anderer
 *       Funktionen muss die TCS_init() Methode aufgerufen werden.
 */

#ifndef TCS34725_H_
#define TCS34725_H_

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
#define TCS_CMD(reg) (TCS34725_COMMAND_BIT | (reg))

/**
 * @brief Generiert ein Command-Byte mit Auto-Increment für den Registerzugriff.
 *
 * Zusätzlich zum obligatorischen Command-Bit (Bit 7) wird hier Bit 5 (TYPE = 01)
 * gesetzt. Damit aktiviert man den Auto-Increment-Modus des TCS34725:
 *
 *    Beim Schreiben werden aufeinanderfolgende Datenbytes automatisch in
 *    die nächsthöhere Registeradresse abgelegt.
 *    Beim Lesen liest der Sensor nach jedem Byte automatisch das nächste
 *    Register aus.
 *
 * @param reg  Start-Registeradresse.
 * @return     Command-Byte für Auto-Increment-Übertragungen.
 */
#define TCS_CMD_AI(reg)  (TCS34725_COMMAND_BIT | 0x20 | (reg))

/* ========================================================================== */
/* Funktionen                                                                 */
/* ========================================================================== */

/**
 * @brief Initialisiert den TCS34725 Farbsensor.
 *
 * Konfiguriert den Sensor mit Standardeinstellungen:
 *   - Integrationszeit: 100ms 
 *   - Verstärkung: 4x (moderate Empfindlichkeit)
 *   - Lichtschranken-Schwellenwerte für Interrupt-Funktionalität
 *   - Persistenz auf 1 Ereignis
 *   - Sensor wird nach Initialisierung in Schlafmodus versetzt
 *
 * @note Der Sensor wird nach der Initialisierung ausgeschaltet und muss
 *       für Messungen explizit aktiviert werden.
 */
void TCS_init(void);

/**
 * @brief Liest einen 16-Bit Wert vom TCS34725 aus.
 *
 * Liest zwei aufeinanderfolgende Register und kombiniert sie zu einem 16-Bit Wert.
 * Little-Endian Reihenfolge (Low-Byte zuerst).
 *
 * @param[in] reg Startregisteradresse (Low-Byte).
 * @return Kombinierter 16-Bit Wert aus dem Registerpaar.
 */
uint16_t TCS_read_16bit_reg(uint8_t reg);

/**
 * @brief Liest den Clear-Kanal-Wert vom TCS34725.
 *
 * Aktiviert den Sensor, führt eine Messung durch und liest den Clear-Kanal aus.
 * Der Sensor wird nach der Messung wieder ausgeschaltet.
 *
 * @param[out] clear Pointer zum Speichern des 16-Bit Clear-Kanal-Werts.
 *
 * @note Die Funktion wartet die komplette Integrationszeit (120ms) ab.
 */
void TCS_read_clear(uint16_t *clear);

/**
 * @brief Liest RGBC-Werte und konvertiert zu 8-Bit RGB für Farberkennung.
 *
 * Aktiviert den Sensor mit LED, führt eine Messung durch und konvertiert
 * die rohen 16-Bit Werte zu 8-Bit RGB-Werten. Die Konvertierung ist für
 * Farbunterscheidung optimiert, nicht für akkurate Farbwiedergabe.
 *
 * Die Konvertierung:
 *   - Skaliert alle Kanäle proportional um Overflow zu verhindern
 *   - Verwendet Bit-Verschiebungen für Effizienz
 *   - Erhält relative Farbverhältnisse
 *
 * @param[out] r8  Pointer zum Speichern des 8-Bit Rot-Werts.
 * @param[out] g8  Pointer zum Speichern des 8-Bit Grün-Werts.
 * @param[out] b8  Pointer zum Speichern des 8-Bit Blau-Werts.
 * @param[out] c16 Pointer zum Speichern des 16-Bit Clear-Kanal-Werts.
 *
 * @warning Dies ist nicht geeignet für akkurate Farbbestimmung oder
 *          Farbtemperaturmessungen.
 */
void TCS_get_rgbc(uint8_t *r8, uint8_t *g8, uint8_t *b8, uint16_t *c16);

/**
 * @brief Schaltet die TCS34725 LED ein.
 *
 * Aktiviert die weiße LED am Pin P1.7 für gleichmäßige Beleuchtung
 * während der Farbmessung.
 */
void TCS_led_on(void);

/**
 * @brief Schaltet die TCS34725 LED aus.
 *
 * Deaktiviert die weiße LED am Pin P1.7 zur Energieeinsparung.
 */
void TCS_led_off(void);

#endif /* TCS34725_H_ */
