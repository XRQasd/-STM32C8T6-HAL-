
#ifndef __CLOCK_UI_H__
#define __CLOCK_UI_H__
#include "main.h"
#include "i2c.h"
#include "rtc.h"

#include "gpio.h"
#include "oled.h"
#include "string.h"
#include "font.h"
#include "kk_rtc.h"
#include <stdio.h>
void show_clock_UI();
int First_Page_Clock(void);
int settingpage(void);
void show_setting_UI();
extern int settingflags;
int menu(void);
extern uint8_t menuflag;
void menu_animation(void);
void StopWatch_Tick(void);
int stopwatch(void);
void show_stopwatch_UI();
int led(void);
void show_oled_UI(void);
int mpu6050(void);
int level(void);
void show_level_UI(void);


extern uint8_t start_timing_flag;




#endif
