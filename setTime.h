
#ifndef __SETTIME_H__
#define __SETTIME_H__
#include "main.h"
#include "oled.h"
int setTime(void);
extern int set_time_flag;

void show_setTime_first_UI(void);
void show_setTime_second_UI(void);  
int SetYear(void);
int SetMonth(void);
int SetDay(void);
int SetHour(void);  
int SetMinute(void);
int SetSecond(void);

void change_rtc_time(uint8_t i,uint8_t flag);




#endif