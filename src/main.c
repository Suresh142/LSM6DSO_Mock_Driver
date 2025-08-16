/**
 * @file main.c
 * @author Suresh Kumar
 * @brief Unit tests for LSM6DSO mock driver (Unity framework).
 */

#include <stdio.h>
#include "imu_driver.h"
#include "mock_i2c.h"

static void prime_one_sample(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz)
{
    // Gyro block (OUTX_L_G = 0x22): xL,xH,yL,yH,zL,zH
    uint8_t gbuf[6] = {
        gx & 0xFF, (uint8_t)(gx >> 8),
        gy & 0xFF, (uint8_t)(gy >> 8),
        gz & 0xFF, (uint8_t)(gz >> 8)};
    mock_i2c_set_read_block(LSM6DSO_REG_OUTX_L_G, gbuf, 6);

    // Accel block (OUTX_L_A = 0x28): xL,xH,yL,yH,zL,zH
    uint8_t abuf[6] = {
        ax & 0xFF, (uint8_t)(ax >> 8),
        ay & 0xFF, (uint8_t)(ay >> 8),
        az & 0xFF, (uint8_t)(az >> 8)};
    mock_i2c_set_read_block(LSM6DSO_REG_OUTX_L_A, abuf, 6);
}

int main(void)
{
    mock_i2c_reset();
    if (imu_init() != 0)
    {
        fprintf(stderr, "IMU init failed\n");
        return 1;
    }

    imu_data_t d = {0};

    // Feed a sequence like in tests so you’ll see non-zero filtered output
    for (int i = 1; i <= 7; ++i)
    {
        // accel raw grows by 1000, 2000, 3000... ; gyro by 100, 200, 300...
        prime_one_sample(1000 * i, 2000 * i, 3000 * i,
                         100 * i, 200 * i, 300 * i);

        if (imu_read_filtered(&d) != 0)
        {
            fprintf(stderr, "IMU read failed\n");
            return 1;
        }
        printf("Accel [mg]: X=%.2f Y=%.2f Z=%.2f\n", d.ax_mg, d.ay_mg, d.az_mg);
        printf("Gyro  [dps]: X=%.3f Y=%.3f Z=%.3f\n", d.gx_dps, d.gy_dps, d.gz_dps);
    }
    return 0;
}
