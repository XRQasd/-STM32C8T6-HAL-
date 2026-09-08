#include "setTime.h"
#include "kk_rtc.h"
#include "key.h"
#include "oled.h"
#include "rtc.h"
#include "main.h"
#include "clock_UI.h"
extern uint8_t KeyNum;
void show_setTime_first_UI(void)
{
    char time[32];
    struct tm*now=KK_RTC_GetTime();
     OLED_PrintString(0,0,"<=",&font16x16_settings, OLED_COLOR_NORMAL);
     sprintf(time,"年:%4d",now->tm_year+1900);
     OLED_PrintString(0,16,time,&font16x16_time, OLED_COLOR_NORMAL);
     sprintf(time,"月:%2d",now->tm_mon+1);
     OLED_PrintString(0,32,time,&font16x16_time, OLED_COLOR_NORMAL);
     sprintf(time,"日:%2d",now->tm_mday);
     OLED_PrintString(0,48,time,&font16x16_time, OLED_COLOR_NORMAL);
}

void show_setTime_second_UI(void)
{
    char time[32];
    struct tm*now=KK_RTC_GetTime();
     sprintf(time,"时:%2d",now->tm_hour);
     OLED_PrintString(0,0,time,&font16x16_time, OLED_COLOR_NORMAL);
     sprintf(time,"分:%2d",now->tm_min);
     OLED_PrintString(0,16,time,&font16x16_time, OLED_COLOR_NORMAL);
     sprintf(time,"秒:%2d",now->tm_sec);
     OLED_PrintString(0,32,time,&font16x16_time, OLED_COLOR_NORMAL);
}

int SetYear(void)
{
    while(1)
    {
        KeyNum=Key_GetNum();
        if(KeyNum==1)//数值加1
        {
            change_rtc_time(0,1);
        }
        else if(KeyNum==2)//数值减1
        {
            change_rtc_time(0,0);
        }
        else if(KeyNum==3)//确认，保存并退出
        {
            return 0;
        }
        show_setTime_first_UI();
        OLED_InvertArea(24,16,32,16);
        OLED_ShowFrame();
    }
}

int SetMonth(void)
{
    while(1)
    {
        KeyNum=Key_GetNum();
        if(KeyNum==1)//数值加1
        {
            change_rtc_time(1,1);

        }
        else if(KeyNum==2)//数值减1
        {
            change_rtc_time(1,0);
        }
        else if(KeyNum==3)//确认，保存并退出
        {
            return 0;
        }
        show_setTime_first_UI();
        OLED_InvertArea(24,32,16,16);
        OLED_ShowFrame();
    }
}

int SetDay(void)
{
    while(1)
    {
        KeyNum=Key_GetNum();
        if(KeyNum==1)//数值加1
        {
            change_rtc_time(2,1);
        }
        else if(KeyNum==2)//数值减1
        {
            change_rtc_time(2,0);
        }
        else if(KeyNum==3)//确认，保存并退出
        {
            return 0;
        }
        show_setTime_first_UI();
        OLED_InvertArea(24,48,16,16);
        OLED_ShowFrame();
    }
}

int SetHour(void)
{
    while(1)
    {
        KeyNum=Key_GetNum();
        if(KeyNum==1)//数值加1
        {
            change_rtc_time(3,1);
        }
        else if(KeyNum==2)//数值减1
        {
            change_rtc_time(3,0);
        }
        else if(KeyNum==3)//确认，保存并退出
        {
            return 0;
        }
        show_setTime_second_UI();
        OLED_InvertArea(24,0,16,16);
        OLED_ShowFrame();
    }
}


int SetMinute(void)
{
    while(1)
    {
        KeyNum=Key_GetNum();
        if(KeyNum==1)//数值加1
        {
            change_rtc_time(4,1);
        }
        else if(KeyNum==2)//数值减1
        {
            change_rtc_time(4,0);
        }
        else if(KeyNum==3)//确认，保存并退出
        {
            return 0;
        }
        show_setTime_second_UI();
        OLED_InvertArea(24,16,16,16);
        OLED_ShowFrame();
    }
}

int SetSecond(void)
{
    while(1)
    {
        KeyNum=Key_GetNum();
        if(KeyNum==1)//数值加1
        {
            change_rtc_time(5,1);
        }
        else if(KeyNum==2)//数值减1
        {
            change_rtc_time(5,0);       
        }
        else if(KeyNum==3)//确认，保存并退出
        {
            return 0;
        }
        show_setTime_second_UI();
        OLED_InvertArea(24,32,16,16);
        OLED_ShowFrame();   
    }
}


//i:0年，1月，2日，3时，4分，5秒
//flag=1 加1，flag=0 减1
void change_rtc_time(uint8_t i,uint8_t flag)
{
    struct tm t_buf = *(KK_RTC_GetTime());

    if(i == 0)        //年
    {
        if(flag == 1) t_buf.tm_year += 1;
        else          t_buf.tm_year -= 1;
    }
    else if(i == 1)   //月
    {
        if(flag == 1) t_buf.tm_mon += 1;
        else          t_buf.tm_mon -= 1;
        if(t_buf.tm_mon < 0)  {t_buf.tm_mon = 11; t_buf.tm_year -=1;}
        if(t_buf.tm_mon > 11) {t_buf.tm_mon = 0;  t_buf.tm_year +=1;}
    }
    else if(i == 2)   //日
    {
        if(flag == 1) t_buf.tm_mday += 1;
        else          t_buf.tm_mday -= 1;
    }
    else if(i == 3)   //时
    {
        if(flag == 1) t_buf.tm_hour += 1;
        else          t_buf.tm_hour -= 1;
        if(t_buf.tm_hour >=24) t_buf.tm_hour = 0;
        if(t_buf.tm_hour < 0)  t_buf.tm_hour = 23;
    }
    else if(i == 4)   //分
    {
        if(flag == 1) t_buf.tm_min += 1;
        else          t_buf.tm_min -= 1;
        if(t_buf.tm_min >=60) {t_buf.tm_min =0; t_buf.tm_hour++;}
        if(t_buf.tm_min <0)   {t_buf.tm_min =59;t_buf.tm_hour--;}
    }
    else if(i == 5)   //秒
    {
        if(flag == 1) t_buf.tm_sec += 1;
        else          t_buf.tm_sec -= 1;
        if(t_buf.tm_sec >=60) {t_buf.tm_sec =0; t_buf.tm_min++;}
        if(t_buf.tm_sec <0)   {t_buf.tm_sec =59;t_buf.tm_min--;}
    }

    KK_RTC_SetTime(&t_buf);
}




int set_time_flag=1;

int setTime(void){
    while(1)
    {
        KeyNum=Key_GetNum();
        uint8_t set_time_flag_temp=0;
        if(KeyNum==1)//上一项
        {
            set_time_flag--;
            if(set_time_flag<=0)set_time_flag=7;
        }
        else if(KeyNum==2)//下一项
        {
            set_time_flag++;
            if(set_time_flag>=8)set_time_flag=1;
        }
        else if(KeyNum==3)//确认
        {
           OLED_NewFrame();
           OLED_ShowFrame();
           set_time_flag_temp=set_time_flag;
        }

if(set_time_flag_temp==1) {return 0;}
else if(set_time_flag_temp==2) {SetYear();} //年
else if(set_time_flag_temp==3) {SetMonth();} //月
else if(set_time_flag_temp==4) {SetDay();} //日
else if(set_time_flag_temp==5) {SetHour();} //时
else if(set_time_flag_temp==6) {SetMinute();} //分
else if(set_time_flag_temp==7) {SetSecond();} //秒

        switch(set_time_flag )
        {
            case 1:
            OLED_NewFrame();
                show_setTime_first_UI();
                OLED_InvertArea(0,0,32,16);
                OLED_ShowFrame();
                break;
                
            case 2:
            OLED_NewFrame();
                 show_setTime_first_UI();
                OLED_InvertArea(0,16,16,16);
                OLED_ShowFrame();
                break;

                case 3:
            OLED_NewFrame();
                 show_setTime_first_UI();
                OLED_InvertArea(0,32,16,16);
                OLED_ShowFrame();
                break;

                case 4:
            OLED_NewFrame();
                 show_setTime_first_UI();
                OLED_InvertArea(0,48,16,16);
                OLED_ShowFrame();
                break;

                case 5:
            OLED_NewFrame();
                 show_setTime_second_UI();
                OLED_InvertArea(0,0,16,16);
                OLED_ShowFrame();
                break;
                
                case 6:
            OLED_NewFrame();
                 show_setTime_second_UI();
                OLED_InvertArea(0,16,16,16);
                OLED_ShowFrame();
                break;
                
                case 7:
            OLED_NewFrame();
                 show_setTime_second_UI();
                OLED_InvertArea(0,32,16,16);
                OLED_ShowFrame();
                break;
        }
    }
}