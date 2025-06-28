/*
 * lcd1602_manager.c
 *
 *  Created on: 22.06.2025
 *      Author: raachl
 */


#include "lcd1602.h"
#include "lcd1602_manager.h"
#include <stdbool.h>
#include "timer/timer.h"
#include "I2C/I2C.h"

volatile static COLOR detected_color = UNKNOWN;
volatile static uint8_t current_count_all = 0;
volatile static uint8_t current_count_blue = 0;
volatile static uint8_t current_count_green = 0;
volatile static uint8_t current_count_red = 0;

extern lcd1602_res_t lcd1602_init(void);
extern lcd1602_res_t lcd1602_write(uint16_t lines, char* text);
extern lcd1602_res_t lcd1602_clear(void);
extern lcd1602_res_t lcd1602_backlight(bool on);
extern bool          lcd1602_getBacklightState(void);

void writeReady(void) {
    char ready_text1[17] = "Sortiermaschine ";
    char ready_text2[17] = "ist bereit      ";

    lcd1602_clear();
    timer_sleep_ms(5);
    lcd1602_backlight(true);
    timer_sleep_ms(5);
    lcd1602_write(1, ready_text1);
    timer_sleep_ms(5);
    lcd1602_write(2, ready_text2);
    timer_sleep_ms(5);

    return;
}

void writeCurrentCount(uint8_t current_count_all, uint8_t current_count_blue, 
uint8_t current_count_green, uint8_t current_count_red) {
    char color_text[17] = "Aktuell sortiert";
    char color_count[17] = "A:  R:  B:  G:  ";
    uint8_t da = (current_count_all * 205) >> 11;
    uint8_t ma = current_count_all - da * 10;

    uint8_t dr = (current_count_red * 205) >> 11;
    uint8_t mr = current_count_red - dr * 10;

    uint8_t db = (current_count_blue * 205) >> 11;
    uint8_t mb = current_count_blue - db * 10;

    uint8_t dg = (current_count_green * 205) >> 11;
    uint8_t mg = current_count_green - dg * 10;

    color_count[2]  = '0' + da;
    color_count[3]  = '0' + ma;


    color_count[6]  = '0' + dr;
    color_count[7]  = '0' + mr;

    color_count[10] = '0' + db;
    color_count[11] = '0' + mb;

    color_count[14] = '0' + dg;
    color_count[15] = '0' + mg;

    lcd1602_clear();
    timer_sleep_ms(5);
    lcd1602_write(1, color_text);
    timer_sleep_ms(5);
    lcd1602_write(2, color_count);
    timer_sleep_ms(5);
}

void writeDetectedColor(COLOR color) {
    char detected_color_text1[17] = "Erkannte Farbe: ";
    char detected_color_text2[17] = "                ";
    memcpy(&detected_color_text2[6], "Rot", 3);
    switch (color) {
        case RED: {
            memcpy(&detected_color_text2[6], "Rot", 3);
            break;
        }
        case BLUE: {
            memcpy(&detected_color_text2[6], "Blau", 4);
            break;
        }
        case GREEN: {   
            memcpy(&detected_color_text2[6], "Gruen", 5);
            break;
        }
        default: {
            memcpy(&detected_color_text2[5], "Unklar", 6);
            break;
        }
    }

    lcd1602_clear();
    timer_sleep_ms(5);
    lcd1602_write(1, detected_color_text1);
    timer_sleep_ms(5);
    lcd1602_write(2, detected_color_text2);
    timer_sleep_ms(5);
    return;
}

void clearDisplayAndBacklightOff(void) {
    lcd1602_clear();
    lcd1602_backlight(false);
}