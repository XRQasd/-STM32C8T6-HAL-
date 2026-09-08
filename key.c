//==============非阻塞式按键===============
#include "key.h"
#include "gpio.h"   
#include "main.h"   
#include "string.h"
#include "clock_UI.h"
#include "tim.h"

// 按键引脚宏定义（便于移植）
#define KEY1_PIN   GPIO_PIN_1
#define KEY1_PORT  GPIOB
#define KEY2_PIN   GPIO_PIN_6
#define KEY2_PORT  GPIOA
#define KEY3_PIN   GPIO_PIN_4
#define KEY3_PORT  GPIOA


// 按键消抖时间（毫秒）上拉输入配置，按下时为低电平
#define KEY_DEBOUNCE_TIME  20
uint8_t Key_Num;// 按键事件标志，存储按下的按键编号（1~3）
/**
  * @brief  获取按键编号（非阻塞扫描，带消抖）
  * @retval 按键编号：1~3，无按键返回0
  */
uint8_t Key_GetNum(void)
{
    uint8_t Temp;
    if (Key_Num)//等价于if(Key_Num != 0)
    {
        Temp = Key_Num;
        Key_Num = 0;
        return Temp; // 读取后清零
    }
    else
    {
        return 0;// 无按键
    }
}
//Key_Num 是一个事件标志，主循环调用此函数"取走"按键事件，取走后立即清零，避免重复响应。
uint8_t Key_GetState(void)
{
 
    // 检测按键1（PB1）
    if (HAL_GPIO_ReadPin(KEY1_PORT, KEY1_PIN) == GPIO_PIN_RESET)
    {
       
            return 1;
        }
    // 检测按键2（PA6）
    if (HAL_GPIO_ReadPin(KEY2_PORT, KEY2_PIN) == GPIO_PIN_RESET)
    {
        
            return 2;
        }
    

    // 检测按键3（PA4）
    if (HAL_GPIO_ReadPin(KEY3_PORT, KEY3_PIN) == GPIO_PIN_RESET)
    {
      
            return 3;
        
    }
else
   {
    return 0;   // 无按键
}
}
void Key_Tick(void)
{
    static uint8_t Count;//static 局部变量核心特点写在函数内部的static局部变量：只在第一次调用函数的时候初始化，函数结束，变量不会销毁，值会保留，下一次调用函数继续沿用上次的值。
    static uint8_t CurrentState, PreState;
    Count++;
    if (Count >= 20)
    {
        Count = 0;// 每 20ms 采样一次
        PreState = CurrentState;// 保存当前的按键状态
        CurrentState = Key_GetState();//
        if (PreState != 0 && CurrentState == 0)//之前是按下状态，现在是松开状态，说明按键被按下过
        {
            Key_Num = PreState;   // =============关键：记录是哪个按键被按下过
        }
    }
}
//这个函数用于检测按键是否被按下，且必须是按住后松开一次，才能被检测到



//整个key.c文件的代码用来存储按下的是哪一个按键上传给key_getnum，以及按键的当前状态和上一个状态