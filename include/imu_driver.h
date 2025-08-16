#ifndef IMU_DRIVER_H
#define IMU_DRIVER_H

#include <stdint.h>

#define LSM6DSO_I2C_ADDR 0x6A
#define LSM6DSO_REG_CTRL1_XL 0x10
#define LSM6DSO_REG_CTRL2_G 0x11
#define LSM6DSO_REG_OUTX_L_G 0x22
#define LSM6DSO_REG_OUTX_L_A 0x28

#define LSM6DSO_CTRL1_XL_104HZ_4G 0x48
#define LSM6DSO_CTRL2_G_104HZ_2000 0x4C

#define LSM6DSO_ACC_SENS_4G_MG_LSB (0.122f)
#define LSM6DSO_GYR_SENS_2000_MDPS_LSB (70.0f)

#define IMU_MA_WINDOW 5

typedef struct
{
    float ax_mg, ay_mg, az_mg;
    float gx_dps, gy_dps, gz_dps;
} imu_data_t;

int imu_init(void);
int imu_read_filtered(imu_data_t *out);
float imu_convert_ax_mg(int16_t raw);
float imu_convert_gx_dps(int16_t raw);
int i2c_write_reg(uint8_t dev, uint8_t reg, uint8_t val);
int i2c_read_regs(uint8_t dev, uint8_t start_reg, uint8_t *buf, uint16_t len);

#endif
