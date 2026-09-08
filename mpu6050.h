#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "main.h"
#include "i2c.h"

/* MPU6050 I2C 地址(AD0 引脚接 GND 时为 0x68, 接 VCC 时为 0x69) */
/* HAL 接口需要 7 位地址左移 1 位 */
#define MPU6050_ADDR               (0x68 << 1)

/* 寄存器地址 */
#define MPU6050_REG_SMPLRT_DIV     0x19   /* 采样率分频 */
#define MPU6050_REG_CONFIG         0x1A   /* 数字低通滤波 */
#define MPU6050_REG_GYRO_CONFIG    0x1B   /* 陀螺仪配置 */
#define MPU6050_REG_ACCEL_CONFIG   0x1C   /* 加速度计配置 */
#define MPU6050_REG_ACCEL_XOUT_H   0x3B   /* 加速度计数据起始(共 6 字节) */
#define MPU6050_REG_TEMP_OUT_H     0x41   /* 温度(2 字节) */
#define MPU6050_REG_GYRO_XOUT_H    0x43   /* 陀螺仪数据起始(共 6 字节) */
#define MPU6050_REG_PWR_MGMT_1     0x6B   /* 电源管理 */
#define MPU6050_REG_WHO_AM_I       0x75   /* 器件 ID(默认 0x68) */

/* 灵敏度(默认量程下) */
#define MPU6050_ACCEL_LSB_PER_G    16384.0f   /* 加速度 ±2g: 16384 LSB/g */
#define MPU6050_GYRO_LSB_PER_DPS   131.0f     /* 陀螺仪 ±250dps: 131 LSB/(°/s) */

/* 6 个原始值: 加速度 XYZ + 角速度 XYZ */
typedef struct {
    int16_t Accel_X;
    int16_t Accel_Y;
    int16_t Accel_Z;
    int16_t Gyro_X;
    int16_t Gyro_Y;
    int16_t Gyro_Z;
} MPU6050_Data;//数据封装 用法：MPU6050_Date+名称 在main.c里面一次性可以调取所有数据

uint8_t MPU6050_Init(void);                       /* 初始化, 返回 0 成功 / 1 失败 */
uint8_t MPU6050_ReadID(void);                     /* 读取 WHO_AM_I, 正常为 0x68 */
void MPU6050_ReadData(MPU6050_Data *data);        /* 一次读取 6 个原始值 */
void MPU6050_ReadAccel(int16_t *ax, int16_t *ay, int16_t *az);
void MPU6050_ReadGyro(int16_t *gx, int16_t *gy, int16_t *gz);
void MPU6050_GetAccelG(float *ax, float *ay, float *az);    /* 加速度, 单位 g */
void MPU6050_GetGyroDPS(float *gx, float *gy, float *gz);   /* 角速度, 单位 °/s */

#endif /* __MPU6050_H__ */
