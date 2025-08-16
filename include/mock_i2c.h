#ifndef MOCK_I2C_H
#define MOCK_I2C_H

#include <stdint.h>

#define MOCK_I2C_MAX_WRITES 64

typedef struct
{
    uint8_t dev, reg, val;
} i2c_write_op_t;

extern i2c_write_op_t mock_i2c_writes[MOCK_I2C_MAX_WRITES];
extern int mock_i2c_write_count;

void mock_i2c_set_read_block(uint8_t start_reg, const uint8_t *data, uint16_t len);
void mock_i2c_reset(void);

#endif
