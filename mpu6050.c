/**
 * @file mpu6050.c
 * @brief MPU6050 六轴传感器驱动(基于 HAL 库 I2C)
 * @note 使用流程:
 *       1. 上电后等待 20ms, 调用 MPU6050_Init() 初始化
 *       2. 调用 MPU6050_ReadData() / MPU6050_ReadAccel() / MPU6050_ReadGyro()
 *          读取 6 个原始值
 *       3. 需要物理单位时调用 MPU6050_GetAccelG() / MPU6050_GetGyroDPS()
 */

#include "mpu6050.h"

/* I2C 总线出错时重新初始化, 解除总线卡死(避免传染给同总线的 OLED) */
static void MPU6050_I2C_Recover(void)
{
    HAL_I2C_DeInit(&hi2c1);
    MX_I2C1_Init();
}

/* 写单个寄存器 */
static uint8_t MPU6050_WriteReg(uint8_t reg, uint8_t val)
{
    if (HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, reg,
                          I2C_MEMADD_SIZE_8BIT, &val, 1, 100) != HAL_OK)
    {
        MPU6050_I2C_Recover();
        return 1;
    }
    return 0;
}

/* 连续读取多个寄存器 */
static uint8_t MPU6050_ReadRegs(uint8_t reg, uint8_t *buf, uint8_t len)
{
    if (HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, reg,
                         I2C_MEMADD_SIZE_8BIT, buf, len, 100) != HAL_OK)
    {
        MPU6050_I2C_Recover();
        return 1;
    }
    return 0;
}

/* 读取器件 ID, 正常为 0x68 */
uint8_t MPU6050_ReadID(void)
{
    uint8_t id = 0;
    MPU6050_ReadRegs(MPU6050_REG_WHO_AM_I, &id, 1);
    return id;
}

/* 初始化 MPU6050, 返回 0 成功 / 1 失败 */
uint8_t MPU6050_Init(void)
{
    uint8_t id;

    HAL_Delay(20);   /* 等待上电稳定 */

    /* 复位器件 */
    MPU6050_WriteReg(MPU6050_REG_PWR_MGMT_1, 0x80);
    HAL_Delay(100);

    /* 唤醒, 选择 PLL X 轴陀螺仪作为时钟源 */
    MPU6050_WriteReg(MPU6050_REG_PWR_MGMT_1, 0x01);
    HAL_Delay(10);

    /* 采样率分频 = 0 (陀螺仪 1kHz) */
    MPU6050_WriteReg(MPU6050_REG_SMPLRT_DIV, 0x00);
    /* 数字低通滤波: DLPF = 6 */
    MPU6050_WriteReg(MPU6050_REG_CONFIG, 0x06);
    /* 加速度计 ±2g, 陀螺仪 ±250°/s */
    MPU6050_WriteReg(MPU6050_REG_ACCEL_CONFIG, 0x00);
    MPU6050_WriteReg(MPU6050_REG_GYRO_CONFIG, 0x00);

    /* 校验器件是否正常 */
    id = MPU6050_ReadID();
    if (id == 0x68)
        return 0;
    return 1;
}

/* 一次读取 6 个原始值 */
void MPU6050_ReadData(MPU6050_Data *data)
{
    uint8_t buf[14] = {0};

    if (data == NULL)
        return;

    MPU6050_ReadRegs(MPU6050_REG_ACCEL_XOUT_H, buf, 14);

    data->Accel_X = (int16_t)((buf[0] << 8) | buf[1]);
    data->Accel_Y = (int16_t)((buf[2] << 8) | buf[3]);
    data->Accel_Z = (int16_t)((buf[4] << 8) | buf[5]);
    /* buf[6..7] 为温度, 跳过 */
    data->Gyro_X = (int16_t)((buf[8]  << 8) | buf[9]);
    data->Gyro_Y = (int16_t)((buf[10] << 8) | buf[11]);
    data->Gyro_Z = (int16_t)((buf[12] << 8) | buf[13]);
}

/* 读取加速度原始值 */
void MPU6050_ReadAccel(int16_t *ax, int16_t *ay, int16_t *az)
{
    MPU6050_Data data;
    MPU6050_ReadData(&data);
    if (ax) *ax = data.Accel_X;
    if (ay) *ay = data.Accel_Y;
    if (az) *az = data.Accel_Z;
}

/* 读取角速度原始值 */
void MPU6050_ReadGyro(int16_t *gx, int16_t *gy, int16_t *gz)
{
    MPU6050_Data data;
    MPU6050_ReadData(&data);
    if (gx) *gx = data.Gyro_X;
    if (gy) *gy = data.Gyro_Y;
    if (gz) *gz = data.Gyro_Z;
}

/* 读取加速度并换算为 g */
void MPU6050_GetAccelG(float *ax, float *ay, float *az)
{
    MPU6050_Data data;
    MPU6050_ReadData(&data);
    if (ax) *ax = data.Accel_X / MPU6050_ACCEL_LSB_PER_G;
    if (ay) *ay = data.Accel_Y / MPU6050_ACCEL_LSB_PER_G;
    if (az) *az = data.Accel_Z / MPU6050_ACCEL_LSB_PER_G;
}

/* 读取角速度并换算为 °/s */
void MPU6050_GetGyroDPS(float *gx, float *gy, float *gz)
{
    MPU6050_Data data;
    MPU6050_ReadData(&data);
    if (gx) *gx = data.Gyro_X / MPU6050_GYRO_LSB_PER_DPS;
    if (gy) *gy = data.Gyro_Y / MPU6050_GYRO_LSB_PER_DPS;
    if (gz) *gz = data.Gyro_Z / MPU6050_GYRO_LSB_PER_DPS;
}
