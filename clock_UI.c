#include "main.h"
#include "i2c.h"
#include "rtc.h"
#include "usart.h"
#include "gpio.h"
#include "oled.h"
#include "string.h"
#include "font.h"
#include "kk_rtc.h"
#include <stdio.h>
#include "clock_UI.h"
#include "key.h"
#include "setTime.h"
#include "mpu6050.h"
#include "math.h"

#define MENU_COUNT 7


//==================================首页时钟==================================
void show_clock_UI()
{
    struct tm *time = KK_RTC_GetTime();
    char time_str[16];
    //格式化：年‑月‑日
    snprintf(time_str, sizeof(time_str), "%04d %02d %02d",
             time->tm_year + 1900,
             time->tm_mon + 1,
             time->tm_mday);

    OLED_PrintASCIIString(0, 0, time_str, &afont12x6, OLED_COLOR_NORMAL);
    //格式化：时‑分‑秒
    snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d",
             time->tm_hour,
             time->tm_min,
             time->tm_sec);
    OLED_PrintASCIIString(16, 16, time_str, &afont24x12, OLED_COLOR_NORMAL);
// 用 font30x40 显示中文"菜单设置"
OLED_PrintString(0,48, "菜单", &font16x16_menu, OLED_COLOR_NORMAL);
OLED_PrintString(96,48, "设置", &font16x16_menu, OLED_COLOR_NORMAL);

}


//==================================首页按键==================================
int clkflag=1;//get choce
uint8_t KeyNum;
int First_Page_Clock(void)
{
    while(1)
    {
        KeyNum=Key_GetNum();
        if(KeyNum==1)//上一项
        {
            clkflag--;
            if(clkflag<=0)clkflag=2;
        }
        else if(KeyNum==2)//下一项
        {
            clkflag++;
            if(clkflag>=3)clkflag=1;
        }
        else if(KeyNum==3)//确认
        {
           OLED_NewFrame();
           OLED_ShowFrame();
            return clkflag;
        }
        switch(clkflag)
        {
            case 1:
                show_clock_UI();
                OLED_InvertArea(0,48,32,16);
                OLED_ShowFrame();
                break;
                
            case 2:
                 show_clock_UI();
                OLED_InvertArea(96,48,32,16);
                OLED_ShowFrame();
                break;
        }
    }
}


//==================================设置界面==================================
void show_setting_UI(){
    OLED_PrintString(0,0,"<=",&font16x16_settings, OLED_COLOR_NORMAL);
    OLED_PrintString(0,30,"日期时间",&font16x16_setting, OLED_COLOR_NORMAL);
    
}
int settingflags=1;
int settingpage(void){
while(1)
    {
        KeyNum=Key_GetNum();
        uint8_t setflag_temp=0;
        if(KeyNum==1)//上一项
        {
            settingflags--;
            if(settingflags<=0)settingflags=2;
        }
        else if(KeyNum==2)//下一项
        {
            settingflags++;
            if(settingflags>=3)settingflags=1;
        }
        else if(KeyNum==3)//确认
        {
           OLED_NewFrame();
           OLED_ShowFrame();
           setflag_temp=settingflags;
        }

        if(setflag_temp==1){return 0;}
        else if(setflag_temp==2){setTime();}

        switch(settingflags )
        {
            case 1:
                show_setting_UI();
                OLED_InvertArea(0,0,32,16);
                OLED_ShowFrame();
                break;
                
            case 2:
                 show_setting_UI();
                OLED_InvertArea(0,30,64,16);
                OLED_ShowFrame();
                break;
        }
    }

}
//==================================滑动菜单界面==================================
uint8_t menuflag=1;
int8_t pre_selection; //本次选择的选项
int8_t target_selection; //目标选项
uint8_t x_pre=48; //本次选项的x坐标
uint8_t Speed=4; //速度
uint8_t move_flag; //开始移动的标志位，1表示开始移动，0表示不移动


void set_selection(uint8_t move_flag_set, uint8_t target_set, uint8_t pre_set){
    if(move_flag_set == 1)
    {
        target_selection = target_set;   // ✅ 赋值给全局变量
        pre_selection = pre_set;         // ✅ 赋值给全局变量
        
    }
    menu_animation();
}

int menu(void)
{

  menuflag = 1;
    pre_selection   = menuflag - 1;   // 0 → menu_icons[0] fanhui
    target_selection = menuflag - 1;   // 0，初始不动
    move_flag = 0;
    x_pre = 48;

    // ✅ 先画初始画面！解决黑屏
    OLED_NewFrame();
    OLED_DrawImage(42, 10, &frameImg, OLED_COLOR_NORMAL);
    // 画初始中心图标和旁边的
    OLED_DrawImage(48, 16, &menu_icons[0], OLED_COLOR_NORMAL);       // fanhui
    OLED_DrawImage(48+48, 16, &menu_icons[1], OLED_COLOR_NORMAL);   // time
    OLED_DrawImage(48+96, 16, &menu_icons[2], OLED_COLOR_NORMAL);   // light
    OLED_ShowFrame();
    
	
       while(1)
    {
        KeyNum = Key_GetNum();
        uint8_t menuflag_temp = 0;

        // ---- 按键处理：只更新 menuflag + 动画目标 ----
      // ===== 上一项 =====
if(KeyNum == 1)
{
    pre_selection    = menuflag - 1;
    menuflag--;
    if(menuflag == 0) menuflag = 7;
    target_selection = pre_selection - 1;   // ✅ 用 pre_selection-1，保持差值=-1
    move_flag = 1;
    x_pre = 48;
}
// ===== 下一项 =====
else if(KeyNum == 2)
{
    pre_selection    = menuflag - 1;
    menuflag++;
    if(menuflag > 7) menuflag = 1;
    target_selection = pre_selection + 1;   // ✅ 用 pre_selection+1，保持差值=+1
    move_flag = 1;
    x_pre = 48;
}
        else if(KeyNum == 3)  // 确认
        {
            OLED_NewFrame();
            OLED_ShowFrame();
            menuflag_temp = menuflag;
        }

        if(menuflag_temp==1){return 0;}
        else if(menuflag_temp==2){stopwatch();
            // 子页面返回后重绘菜单初始画面 —— 解决黑屏
            OLED_NewFrame();
            OLED_DrawImage(42, 10, &frameImg, OLED_COLOR_NORMAL);
            OLED_DrawImage(48, 16, &menu_icons[(menuflag-1 + MENU_COUNT*2) % MENU_COUNT], OLED_COLOR_NORMAL);
            OLED_DrawImage(48+48, 16, &menu_icons[(menuflag     + MENU_COUNT*2) % MENU_COUNT], OLED_COLOR_NORMAL);
            OLED_DrawImage(48+96, 16, &menu_icons[(menuflag+1 + MENU_COUNT*2) % MENU_COUNT], OLED_COLOR_NORMAL);
            pre_selection   = menuflag - 1;
            target_selection = menuflag - 1;
            move_flag = 0;
            x_pre = 48;
            OLED_ShowFrame();}
        else if(menuflag_temp==3){led();
         OLED_NewFrame();
            OLED_DrawImage(42, 10, &frameImg, OLED_COLOR_NORMAL);
            OLED_DrawImage(48, 16, &menu_icons[(menuflag-1 + MENU_COUNT*2) % MENU_COUNT], OLED_COLOR_NORMAL);
            OLED_DrawImage(48+48, 16, &menu_icons[(menuflag     + MENU_COUNT*2) % MENU_COUNT], OLED_COLOR_NORMAL);
            OLED_DrawImage(48+96, 16, &menu_icons[(menuflag+1 + MENU_COUNT*2) % MENU_COUNT], OLED_COLOR_NORMAL);
            pre_selection   = menuflag - 1;
            target_selection = menuflag - 1;
            move_flag = 0;
            x_pre = 48;
            OLED_ShowFrame(); }
        else if(menuflag_temp==4){mpu6050();
         OLED_NewFrame();
            OLED_DrawImage(42, 10, &frameImg, OLED_COLOR_NORMAL);
            OLED_DrawImage(48, 16, &menu_icons[(menuflag-1 + MENU_COUNT*2) % MENU_COUNT], OLED_COLOR_NORMAL);
            OLED_DrawImage(48+48, 16, &menu_icons[(menuflag     + MENU_COUNT*2) % MENU_COUNT], OLED_COLOR_NORMAL);
            OLED_DrawImage(48+96, 16, &menu_icons[(menuflag+1 + MENU_COUNT*2) % MENU_COUNT], OLED_COLOR_NORMAL);
            pre_selection   = menuflag - 1;
            target_selection = menuflag - 1;
            move_flag = 0;
            x_pre = 48;
            OLED_ShowFrame();}
        else if(menuflag_temp==5){}
        else if(menuflag_temp==6){}
        else if(menuflag_temp==7){}

      if(move_flag == 1)
        {
            menu_animation();   // 走完自动设 move_flag = 0
        }
}
}

void menu_animation(void)
{
    OLED_NewFrame();
    OLED_DrawImage(42,10,&frameImg,OLED_COLOR_NORMAL);


if(pre_selection<target_selection)
 {
     x_pre-=Speed;
     if(x_pre==0)
     {
         pre_selection++;
         move_flag=0;
         x_pre=48;
     }
 }
 if(pre_selection>target_selection)
 {
     x_pre+=Speed;
     if(x_pre==96)
     {
         pre_selection--;
         move_flag=0;
         x_pre=48;
     }
 }

   

    // 左起第一个（x_pre - 96）
    if(pre_selection >= -1)
    {
        OLED_DrawImage(x_pre-96, 16,
            &menu_icons[(pre_selection - 2 + MENU_COUNT*2) % MENU_COUNT],
            OLED_COLOR_NORMAL);
    }
    // 左起第二个（x_pre - 48）
    if(pre_selection >= 0)
    {
        OLED_DrawImage(x_pre-48, 16,
            &menu_icons[(pre_selection - 1 + MENU_COUNT*2) % MENU_COUNT],
            OLED_COLOR_NORMAL);
    }
    // 中心（x_pre），永远画
    OLED_DrawImage(x_pre, 16,
        &menu_icons[(pre_selection + MENU_COUNT*2) % MENU_COUNT],
        OLED_COLOR_NORMAL);
    // 右侧相邻 1（x_pre + 48）
    OLED_DrawImage(x_pre+48, 16,
        &menu_icons[(pre_selection + 1) % MENU_COUNT],
        OLED_COLOR_NORMAL);
    // 右侧相邻 2（x_pre + 96）
    OLED_DrawImage(x_pre+96, 16,
        &menu_icons[(pre_selection + 2) % MENU_COUNT],
        OLED_COLOR_NORMAL);
    // ========== 替换结束 ==========

    OLED_ShowFrame();
}

//==================================秒表界面==================================111
// 修复后：加 volatile，告诉编译器"中断可能偷偷改这个变量，每次都去内存里读"
volatile uint8_t hour,min,sec;
  uint8_t start_timing_flag;//1：开始，0：停止

// 秒表界面显示函数
void show_stopwatch_UI(void)
{
    char time_str[9];  // "HH:MM:SS" 占8字符+结束符

    // 1. 清屏（或只更新变化区域，此处假设外部已清屏或使用局部刷新）
    // OLED_Clear(); // 如果需要，在调用本函数前清屏
OLED_NewFrame();
    // 2. 绘制标题或返回标记（你已有的 "<=" 表示返回）
    OLED_PrintString(0, 0, "<=", &font16x16_settings, OLED_COLOR_NORMAL);

    // 3. 格式化时间字符串
    sprintf(time_str, "%02d:%02d:%02d", hour, min, sec);
    // 4. 在屏幕中央显示时间（假设屏幕128x64，使用16x16字体，居中）
    //    计算x偏移： (128 - 8*16)/2 = 0? 8个字符*16像素=128，正好占满，可居中微调
    //    若用16x16字体，8个字符宽度=128，从x=0开始即可
 
   OLED_PrintASCIIString(32, 20, time_str, &afont16x8, OLED_COLOR_NORMAL);
    OLED_PrintString(8,44,"开始",&font_stop16x16,OLED_COLOR_NORMAL);
    OLED_PrintString(48,44,"停止",&font_stop16x16,OLED_COLOR_NORMAL);
    OLED_PrintString(88,44,"清除",&font_stop16x16,OLED_COLOR_NORMAL);
}


 void StopWatch_Tick(void)
 {

static uint16_t tick_count;
tick_count++;
if(tick_count>=100)
{
    tick_count=0;
if(start_timing_flag==1)
 	{
 		sec++;
 		if(sec>=60)
 		{
 			sec=0;
 			min++;
 			if(min>=60)
 			{
 				min=0;
 				hour++;
 				if(hour>99)hour=0;
 			}
 		}
 	}
}	
 }




uint8_t stopwatch_flag=1;
int stopwatch(void)
{
    uint32_t last_tick =0;
while(1)
    {
        KeyNum=Key_GetNum();//获取按键编号
        uint8_t stopwatch_flag_temp=0;//临时变量，用于存储秒表状态
        if(KeyNum==1)//上一项
        {
            stopwatch_flag--;
            if(stopwatch_flag<=0)stopwatch_flag=4;//从0到4，形成循环
        }
        else if(KeyNum==2)//下一项
        {
            stopwatch_flag++;
            if(stopwatch_flag>=5)stopwatch_flag=1;//从5到1，形成循环
        }
       else if(KeyNum==3)
    {
        stopwatch_flag_temp=stopwatch_flag;   // 只记住选项，不清屏！
    }

    // ↓ 先画 UI（包括按确认键这次循环也会画）
    switch(stopwatch_flag)
    {
        case 1:
            show_stopwatch_UI();
            OLED_InvertArea(0,0,32,16);
            OLED_ShowFrame();
            break;
        case 2:
            show_stopwatch_UI();
            OLED_InvertArea(8,44,32,16);
            OLED_ShowFrame();
            break;
        case 3:
            show_stopwatch_UI();
            OLED_InvertArea(48,44,32,16);
            OLED_ShowFrame();
            break;
        case 4:
            show_stopwatch_UI();
            start_timing_flag=0;
            hour=min=sec=0;
            OLED_InvertArea(88,44,32,16);
            OLED_ShowFrame();
            break;
    }

    // ↓ UI 画完再判断是否要返回 / 执行动作
    if(KeyNum==3 && stopwatch_flag_temp==1){return 0;}
    if(KeyNum==3 && stopwatch_flag_temp==2){start_timing_flag=1;}
    if(KeyNum==3 && stopwatch_flag_temp==3){start_timing_flag=0;}
    if(KeyNum==3 && stopwatch_flag_temp==4){hour=min=sec=0;}
}
}

//==========================手电筒==================================
void show_oled_UI(void){
   OLED_PrintString(0, 0, "<=", &font16x16_settings, OLED_COLOR_NORMAL);
   OLED_PrintASCIIString(20, 20, "ON", &afont24x12, OLED_COLOR_NORMAL);
   OLED_PrintASCIIString(78,20,"OFF",&afont24x12,OLED_COLOR_NORMAL);
}

//==========================手电筒==================================
volatile uint8_t led_flag=1;
volatile uint8_t led_state=0;

int led(void)
{
    led_flag = 1;   // 每次进入都重置到返回按钮
    
    while(1)
    {
        KeyNum = Key_GetNum();
        uint8_t led_flag_temp = 0;   // ← 初始化为 0！
        
        if(KeyNum == 1)              // 上一项
        {
            led_flag--;
            if(led_flag <= 0) led_flag = 3;
        }
        else if(KeyNum == 2)         // 下一项
        {
            led_flag++;
            if(led_flag >= 4) led_flag = 1;
        }
        else if(KeyNum == 3)         // 确认 —— 只记选项，不直接 return
        {
            led_flag_temp = led_flag;
        }
        
        // ========= 先画 UI =========
        show_oled_UI();
        switch(led_flag)
        {
            case 1:
                OLED_InvertArea(0, 0, 32, 16);
                break;
            case 2:
                OLED_InvertArea(20, 20, 24, 24);
                break;
            case 3:
                OLED_InvertArea(78, 20, 36, 24);
                break;
        }
        OLED_ShowFrame();
        
        // ========= UI 画完再判断动作 =========
        if(KeyNum == 3 && led_flag_temp == 1) { return 0; }                 // 返回
        if(KeyNum == 3 && led_flag_temp == 2) { led_state = 0; HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET); }  // OFF
        if(KeyNum == 3 && led_flag_temp == 3) { led_state = 1; HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);    }  // ON
    }
}

//==========================mpu6050==================================
 float ax, ay, az, gx, gy, gz;            //原始传感器：三轴加速度、三轴陀螺仪（单位 g / °/s，浮点）
float roll_g,pitch_g,yaw_g;            //只用陀螺仪积分算出来欧拉角（roll横滚，pitch俯仰，yaw偏航）
float roll_a,pitch_a;                  //只用加速度计算出来的欧拉角（加速度不能算yaw）
float Roll,Pitch,Yaw;                  //【互补滤波输出】最终融合之后姿态角，这个是给界面用的
float a=0.9;                           //互补滤波系数：0.9代表相信陀螺仪90%，加速度计10%
float Delta_t=0.005;                   //采样周期 5ms → 每5ms跑一次解算函数
double pi=3.1415927;

void MPU6050_Calculation(void)
{
    HAL_Delay(5);
    MPU6050_GetAccelG(&ax, &ay, &az);
    MPU6050_GetGyroDPS(&gx, &gy, &gz);

    //通过陀螺仪解算欧拉角
    roll_g=Roll+(float)gx*Delta_t;
    pitch_g=Pitch+(float)gy*Delta_t;
    yaw_g=Yaw+(float)gz*Delta_t;

    //通过加速度计解算欧拉角
    pitch_a=atan2((-1)*ax,az)*180/pi;
    roll_a=atan2(ay,az)*180/pi;

    //通过互补滤波器进行数据融合
    Roll=a*roll_g+(1-a)*roll_a;
    Pitch=a*pitch_g+(1-a)*pitch_a;
    Yaw=a*yaw_g;
}

void show_mpu6050_UI(void)
{
    char str[ 16];
    sprintf(str,"Roll:%.2f",Roll);
    OLED_PrintString(0, 0, "<=", &font16x16_settings, OLED_COLOR_NORMAL);
    OLED_PrintASCIIString(0, 16, str, &afont16x8, OLED_COLOR_NORMAL);
    sprintf(str,"Pitch:%.2f",Pitch);
    OLED_PrintASCIIString(0, 32, str, &afont16x8, OLED_COLOR_NORMAL);
    sprintf(str,"Yaw:%.2f",Yaw);
    OLED_PrintASCIIString(0, 48, str, &afont16x8, OLED_COLOR_NORMAL);


}


int mpu6050(void)
{
    while(1)
    {
        KeyNum = Key_GetNum();
        if(KeyNum==3)
        {
            OLED_NewFrame();
            OLED_ShowFrame();
            return 0;
        }
        OLED_NewFrame();
        MPU6050_Calculation();
        show_mpu6050_UI();
        OLED_InvertArea(0, 0, 32, 16);
        OLED_ShowFrame();
    }
}