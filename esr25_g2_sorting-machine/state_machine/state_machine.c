/*
 * timer.c
 *
 *  Created on: 22.06.2025
 *      Author: raachl
 */
 /* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//*****************************************************************************
// This example shows how to configure the I2C module as a master for
// single byte transmission in interrupt mode. The address of the slave
// module that the master is communicating with also set in this example.
//
//  TI sample project msp430fr235x_tb0_04.c
//  Modified by S. Steddin
//  2024-06-09
//******************************************************************************
/*
 * Allgemeine Zweckbestimmung:
 * Demonstration, wie eine einfache ereignisgesteuerte finite state machine implementiert
 * werden kann. Durch Drcken von S1 wird der ON-state aktiviert: Die grne LED wird aktiviert
 * und ein TimerB0 generiert in regelmigen Abstnden einen Interrupt (system tick). Durch Drcken
 * von S2 wird in den OFF-state gewechselt und die system tick wird unterbrochen.
 * Hat die event loop alle per ISR signalisierten Events abgearbeitet, so (Taste S1 oder S2 oder system tick)
 * so wird eine idle Funktion aufgerufen, bevor sich das System in den LPM3 begibt und erst wieder beim
 * Eintreffen eines weiteren Interrupts aufwacht.
 *
 * Die Vorlage kann mit einer beliebigen Anzahl von states erweitert werden, die jeweils auf bis zu 15
 * unterschiedliche events reagieren knnen.
 *
 * Die durch die Interrupts ausgelsten Aktionen werden in der main-loop abgearbeitet, so dass das System
 * jederzeit weitere Interrupts entgegennehmen kann. Alle Aktionen werden im Modus "run-to-completion"
 * abgearbeitet (kooperatives Multitasking)
 *
 * Das system tick Modul kann in beliebiege andere Anwendungen bernommen werden.
 *
 * Version 0.1
 * System funktioniert, jedoch ist der Code noch nicht ausreichend formatiert und kommentiert.
 *
 * Bestimmungsgemer Gebrauch
 * 1. bersetzen des Projektes und bertragen auf das Launchpad
 * 2. Starten des Blinkmode (ON-state) durch Drcken von S1: grne LED leuchtet, rote LED blinkt
 * 3. Stoppen des Blinkmode (Wechsel in OFF-state) durch Drcken von S2: keine LED leuchtet
 *
 * Funktionale Anforderungen
 * FA-001: rote LED soll timergesteuert blinken
 * FA-002: Durch S1 soll der Blinkmode gestartet werden
 * FA-003: Durch S2 soll der Blinkmode gestoppt werden
 * FA-004: Im OFF-state soll der system tick deaktiviert sein.
 *
 * Nichtfunktionale Anforderungen
 * NFA-001: Timer fr system tick soll ber Uhrenquarz getaktet werden, um den LPM3 nutzen zu knnen.
 * NFA-002: system tick soll ber ein API steuerbar sein
 * NFA-003: system tick API soll im Modul system_tick.c angelegt werden
 * NFA-004: system tick soll ber TimerB0 im up-Mode implementiert werden
 *
 * Hinweise: S1 und S2 knnen prellen. In diesem Programm muss keine Entprellung stattfinden,
 * da der Wechsel in einen anderen state, in dem jeweils nur 1 Taste eine Aktion auslst, gegen
 * Tastenprellen unempfindlich ist. Anderfalls msste die sleep Funktion verwendet werden, um
 * nach der Auslsung eines Tasteninterrupts eine Wartezeit zu erzwingen.
 *
 * Verdrahtung:
 *                MSP430FR2355
 *              -----------------
 *          S1-|P2.3             |
 *          S2-|P4.1             |
 *             |                 |
 *             |                 |
 *   LEDred<---|P1.0             |
 *   LEDgreen<-|P6.6             |
 *
 *   S1 und S2 werden ber interne pull up Widerstände aktiviert.
 *
 *   TODO:
 *   - doxygen Kommentare ergnzen und Doku erzeugen
 *   - Bilder und UML-Diagramme erzeugen
 *   - gleiches Projekt mittels DriverLib erstellen
 *
 */
//  ACLK = TBCLK = 32768Hz, MCLK = SMCLK = default DCODIV ~1MHz


#include <msp430.h> 
#include <stdint.h>
#include <stdbool.h>

#include "state_machine/state_machine.h"
#include "lcd1602_display/lcd1602.h"
#include "lcd1602_display/lcd1602_manager.h"

extern void writeCurrentCount(uint8_t current_count_all, uint8_t current_count_blue, uint8_t current_count_green, uint8_t current_count_red);
extern void writeReady(void); 
extern void writeDetectedColor(COLOR color);
Event_t eventBits = EVT_NO_EVENT;

Event_t getEvent (Event_t *event) {
    // if more than 7 different events are required, all 16 bits can be used
    // within the event variable; with 8 bits only, the function consumes less time
    uint16_t ii = 8;
    Event_t bitMask = 0x0080;

    if (*event != 0) {
        while (ii>0) {
            if ((bitMask & *event) > 0) {
                *event &= ~bitMask;
                return(bitMask);
            } else {
                bitMask >>= 1;
            }
            ii--;
        }
    }
    return 0;
}


void handleEvent_FSM(State_t *currentState, Event_t event) {

    State_t state = *currentState;

    switch (state) {
        case OFF_STATE:
            switch (event) {
                case EVT_NO_EVENT:
                    break;
                case EVT_S1:
                    writeReady();
                    // Other configurations?
                    *currentState = ON_STATE;
                    break;
                case EVT_S2:
                    writeCurrentCount(10, 4, 3, 3);
                    *currentState = DISPLAY_STATE;
                    break;
                // case EVENT_COLOR_DETECTION -> do nothing
                // case TIMER_UP -> nothing         
                default:
                    break;
            }
            break;
        case DISPLAY_STATE:
            switch (event) {
                case EVT_NO_EVENT:
                    break;
                case EVT_S1:
                    writeReady();
                    // DisplayShowSortReady
                    // Other configurations
                    *currentState = ON_STATE;
                    break;
                case EVT_S2:
                    // Reset Timer
                    break;
                // case EVENT_COLOR_DETECTION -> do nothing or do something??
                // case TIMER_UP -> clear display, go offline display, *currentState = OFF_STATE;   
                default:
                    break;
            }
            break;
        case ON_STATE:
            switch (event) {
                case EVT_NO_EVENT:
                    break;
                case EVT_S1:
                    // Timer reset
                    // Other stuff?
                    break;
                case EVT_S2:
                    writeCurrentCount(10, 4, 3, 3);
                    *currentState = DISPLAY_STATE;                
                    break;
                // case EVENT_COLOR_DETECTION -> all the doing writeDetectedColor(RED);
                // case TIMER_UP -> clear display, go offline display *currentState = OFF_STATE;     
                default:
                    break;
            }
            break;
    }
}


void idleTask() {
    return;
}
