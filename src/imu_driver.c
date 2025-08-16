/**
 * @file imu_driver.c
 * @author Suresh Kumar
 * @brief Unit tests for LSM6DSO mock driver (Unity framework).
 */

#include "imu_driver.h"
#include <string.h>

#ifndef IMU_DRIVER_NO_DEFAULT_I2C
int i2c_write_reg(uint8_t dev, uint8_t reg, uint8_t val)
{
    (void)dev;
    (void)reg;
    (void)val;
    return 0;
}
int i2c_read_regs(uint8_t dev, uint8_t start_reg, uint8_t *buf, uint16_t len)
{
    (void)dev;
    (void)start_reg;
    for (uint16_t i = 0; i < len; ++i)
        buf[i] = 0;
    return 0;
}
#endif

typedef struct
{
    float ax[IMU_MA_WINDOW], ay[IMU_MA_WINDOW], az[IMU_MA_WINDOW];
    float gx[IMU_MA_WINDOW], gy[IMU_MA_WINDOW], gz[IMU_MA_WINDOW];
    int count;
    int idx;
} ma_state_t;

static ma_state_t s_ma = {0};

int imu_init(void)
{
    if (i2c_write_reg(LSM6DSO_I2C_ADDR, LSM6DSO_REG_CTRL1_XL, LSM6DSO_CTRL1_XL_104HZ_4G) != 0)
        return -1;
    if (i2c_write_reg(LSM6DSO_I2C_ADDR, LSM6DSO_REG_CTRL2_G, LSM6DSO_CTRL2_G_104HZ_2000) != 0)
        return -2;
    memset(&s_ma, 0, sizeof(s_ma));
    return 0;
}

static inline int16_t make_s16(uint8_t lo, uint8_t hi)
{
    return (int16_t)((uint16_t)lo | ((uint16_t)hi << 8));
}

float imu_convert_ax_mg(int16_t raw)
{
    return raw * LSM6DSO_ACC_SENS_4G_MG_LSB;
}

float imu_convert_gx_dps(int16_t raw)
{
    return (raw * LSM6DSO_GYR_SENS_2000_MDPS_LSB) / 1000.0f;
}

static void ma_push(float ax, float ay, float az, float gx, float gy, float gz)
{
    const int i = s_ma.idx;
    s_ma.ax[i] = ax;
    s_ma.ay[i] = ay;
    s_ma.az[i] = az;
    s_ma.gx[i] = gx;
    s_ma.gy[i] = gy;
    s_ma.gz[i] = gz;

    s_ma.idx = (s_ma.idx + 1) % IMU_MA_WINDOW;
    if (s_ma.count < IMU_MA_WINDOW)
        s_ma.count++;
}

static void ma_get(float *ax, float *ay, float *az, float *gx, float *gy, float *gz)
{
    float sax = 0, say = 0, saz = 0, sgx = 0, sgy = 0, sgz = 0;
    const int n = s_ma.count > 0 ? s_ma.count : 1;
    for (int i = 0; i < n; ++i)
    {
        sax += s_ma.ax[i];
        say += s_ma.ay[i];
        saz += s_ma.az[i];
        sgx += s_ma.gx[i];
        sgy += s_ma.gy[i];
        sgz += s_ma.gz[i];
    }
    *ax = sax / n;
    *ay = say / n;
    *az = saz / n;
    *gx = sgx / n;
    *gy = sgy / n;
    *gz = sgz / n;
}

int imu_read_filtered(imu_data_t *out)
{
    if (!out)
        return -1;

    uint8_t graw[6], araw[6];
    if (i2c_read_regs(LSM6DSO_I2C_ADDR, LSM6DSO_REG_OUTX_L_G, graw, 6) != 0)
        return -2;
    if (i2c_read_regs(LSM6DSO_I2C_ADDR, LSM6DSO_REG_OUTX_L_A, araw, 6) != 0)
        return -3;

    int16_t gx_raw = make_s16(graw[0], graw[1]);
    int16_t gy_raw = make_s16(graw[2], graw[3]);
    int16_t gz_raw = make_s16(graw[4], graw[5]);
    int16_t ax_raw = make_s16(araw[0], araw[1]);
    int16_t ay_raw = make_s16(araw[2], araw[3]);
    int16_t az_raw = make_s16(araw[4], araw[5]);

    float ax_mg = imu_convert_ax_mg(ax_raw);
    float ay_mg = imu_convert_ax_mg(ay_raw);
    float az_mg = imu_convert_ax_mg(az_raw);
    float gx_dps = imu_convert_gx_dps(gx_raw);
    float gy_dps = imu_convert_gx_dps(gy_raw);
    float gz_dps = imu_convert_gx_dps(gz_raw);

    ma_push(ax_mg, ay_mg, az_mg, gx_dps, gy_dps, gz_dps);
    ma_get(&out->ax_mg, &out->ay_mg, &out->az_mg, &out->gx_dps, &out->gy_dps, &out->gz_dps);
    return 0;
}
