/**
 * @file test_imu.c
 * @author Suresh Kumar
 * @brief Unit tests for LSM6DSO mock driver (Unity framework).
 */

#include "unity.h"
#include "imu_driver.h"
#include "mock_i2c.h"

static void prime_one_sample(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz)
{
    uint8_t gbuf[6] = {
        gx & 0xFF, gx >> 8,
        gy & 0xFF, gy >> 8,
        gz & 0xFF, gz >> 8};
    mock_i2c_set_read_block(LSM6DSO_REG_OUTX_L_G, gbuf, 6);

    uint8_t abuf[6] = {
        ax & 0xFF, ax >> 8,
        ay & 0xFF, ay >> 8,
        az & 0xFF, az >> 8};
    mock_i2c_set_read_block(LSM6DSO_REG_OUTX_L_A, abuf, 6);
}

void setUp(void) { mock_i2c_reset(); }
void tearDown(void) {}

void test_initialization_sequence(void)
{
    TEST_ASSERT_EQUAL_INT(0, imu_init());
    TEST_ASSERT_EQUAL_INT(2, mock_i2c_write_count);
    TEST_ASSERT_EQUAL_UINT8(LSM6DSO_REG_CTRL1_XL, mock_i2c_writes[0].reg);
    TEST_ASSERT_EQUAL_UINT8(LSM6DSO_CTRL1_XL_104HZ_4G, mock_i2c_writes[0].val);
    TEST_ASSERT_EQUAL_UINT8(LSM6DSO_REG_CTRL2_G, mock_i2c_writes[1].reg);
    TEST_ASSERT_EQUAL_UINT8(LSM6DSO_CTRL2_G_104HZ_2000, mock_i2c_writes[1].val);
}

void test_scaling(void)
{
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 1998.848f, imu_convert_ax_mg(16384));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 70.0f, imu_convert_gx_dps(1000));
}

void test_reading_and_filtering(void)
{
    TEST_ASSERT_EQUAL_INT(0, imu_init());

    imu_data_t out = {0};

    for (int i = 1; i <= 5; ++i)
    {
        prime_one_sample(1000 * i, 2000 * i, 3000 * i,
                         100 * i, 200 * i, 300 * i);
        TEST_ASSERT_EQUAL_INT(0, imu_read_filtered(&out));
    }

    // Assert the average after 5 samples
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 366.0f, out.ax_mg);  // 3000 * 0.122
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 732.0f, out.ay_mg);  // 6000 * 0.122
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 1098.0f, out.az_mg); // 9000 * 0.122
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 21.0f, out.gx_dps); // 300 * 0.07
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 42.0f, out.gy_dps); // 600 * 0.07
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 63.0f, out.gz_dps); // 900 * 0.07
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_initialization_sequence);
    RUN_TEST(test_scaling);
    RUN_TEST(test_reading_and_filtering);
    return UNITY_END();
}
