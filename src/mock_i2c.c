/**
 * @file mock_i2c.c
 * @author Suresh Kumar
 * @brief Unit tests for LSM6DSO mock driver (Unity framework).
 */

#include <string.h>
#include "imu_driver.h"
#include "mock_i2c.h"

i2c_write_op_t mock_i2c_writes[MOCK_I2C_MAX_WRITES];
int mock_i2c_write_count = 0;

typedef struct
{
    uint8_t start;
    uint8_t buf[32];
    uint16_t len;
} read_slot_t;

static read_slot_t slots[8];
static int slot_count = 0;

void mock_i2c_reset(void)
{
    mock_i2c_write_count = 0;
    memset(mock_i2c_writes, 0, sizeof(mock_i2c_writes));
    memset(slots, 0, sizeof(slots));
    slot_count = 0;
}

void mock_i2c_set_read_block(uint8_t start_reg, const uint8_t *data, uint16_t len)
{
    if (slot_count >= (int)(sizeof(slots) / sizeof(slots[0])) || len > sizeof(slots[0].buf))
        return;
    slots[slot_count].start = start_reg;
    slots[slot_count].len = len;
    memcpy(slots[slot_count].buf, data, len);
    slot_count++;
}

int i2c_write_reg(uint8_t dev, uint8_t reg, uint8_t val)
{
    if (mock_i2c_write_count < MOCK_I2C_MAX_WRITES)
    {
        mock_i2c_writes[mock_i2c_write_count++] = (i2c_write_op_t){dev, reg, val};
    }
    return 0;
}

int i2c_read_regs(uint8_t dev, uint8_t start_reg, uint8_t *buf, uint16_t len)
{
    (void)dev;
    for (int i = 0; i < slot_count; ++i)
    {
        if (slots[i].start == start_reg && slots[i].len == len)
        {
            memcpy(buf, slots[i].buf, len);
            for (int j = i; j < slot_count - 1; ++j)
                slots[j] = slots[j + 1];
            slot_count--;
            return 0;
        }
    }
    memset(buf, 0, len);
    return 0;
}
