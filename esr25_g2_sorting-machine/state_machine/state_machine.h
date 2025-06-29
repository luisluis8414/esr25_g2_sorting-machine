/* ========================================================================== */
/* state_machine.h                                                               */
/* ========================================================================== */
/**
 * @file      state_machine.h
 * @author    raachl, wehrberger
 * @date      24.06.2025
 *
 * @brief     Implementierung der State Machine für die Sortieranlage.
 *
 * Dieses Modul implementiert eine Finite State Machine (FSM), die das Verhalten
 * der Sortieranlage steuert.
 * Die Machine hat fünf States:
 *   - OFF_STATE            - Maschine ist ausgeschaltet
 *   - MODE_SELECTION_STATE - User wählt zwischen Auto und manuellem Modus
 *   - AUTO_SORT_STATE      - Automatischer Sortiermodus
 *   - MANUAL_SORT_STATE    - Manueller Sortiermodus
 *   - DISPLAY_STATE        - Anzeige der aktuellen Sortierstatistik
 *
 * Events werden über ein bit basiertes Event System verarbeitet, wobei jedes
 * Event durch ein Bit in der eventBits Variable dargestellt wird.
 */

#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Events für die State Machine.
 *
 * Events werden als Bits in 16 Bit dargestellt,
 * um mehrere gleichzeitig ausstehende Events zu ermöglichen.
 */
#define EVT_NO_EVENT 0x00        /**< Kein Event ausstehend */
#define EVT_SYSTEM_TICK BIT0     /**< System Tick für periodische Checks */
#define EVT_S1 BIT1              /**< Button S1 wurde gedrückt */
#define EVT_S2 BIT2              /**< Button S2 wurde gedrückt */
#define EVT_OBJECT_DETECTED BIT3 /**< Object durch Color Sensor erkannt */

/**
 * @brief States der Sortieranlage.
 */
typedef enum
{
    OFF_STATE,            /**< Machine ist ausgeschaltet */
    MODE_SELECTION_STATE, /**< User wählt Operation Mode */
    AUTO_SORT_STATE,      /**< Automatic Sort Mode aktiv */
    MANUAL_SORT_STATE,    /**< Manual Sort Mode aktiv */
    DISPLAY_STATE         /**< Display der Sort Statistik */
} State_t;

/** @brief Typdefinition für Events mit 16 Bit */
typedef uint16_t Event_t;

/** @brief Global Event Bits Variable für Event Handling */
extern Event_t eventBits;

/**
 * @brief Handled State Transitions und Actions basierend auf Events.
 *
 * Diese Funktion implementiert die State Machine Logic, verarbeitet Events
 * und führt die entsprechenden Aktionen für jede State Event Kombination aus.
 *
 * @param[in,out] currentState Pointer zum Current State
 * @param[in] event Das zu verarbeitende Event
 */
void handleEvent_FSM(State_t *currentState, Event_t event);

/**
 * @brief Extrahiert das nächste pending Event aus den Event Bits.
 *
 * Verarbeitet die Event Bits von MSB zu LSB und gibt das Event mit der
 * höchsten Priority zurück und cleared dessen Bit.
 *
 * @param[in,out] event Pointer zu den Event Bits
 * @return Das nächste zu verarbeitende Event oder EVT_NO_EVENT wenn keine Events pending sind
 */
Event_t getEvent(Event_t *event);

#endif /* STATE_MACHINE_H_ */
