#ifndef __I2C_DEV_H__
#define __I2C_DEV_H__

#include "hal_i2c.h"
#include <stdint.h>

// #define CONFIG_I2C_HW
#define CONFIG_I2C_SW

#ifdef CONFIG_I2C_HW
struct i2c_dev {
    const char *name;
    uint32_t addr;
    enum HAL_I2C_ID_T bus;
    struct HAL_I2C_CONFIG_T cfg;
    int inited;
};

int i2c_init(struct i2c_dev *dev);

void i2c_deinit(struct i2c_dev *dev);

uint32_t i2c_write(struct i2c_dev *dev, const uint8_t *txbuf, uint16_t txlen);

uint32_t i2c_read(struct i2c_dev *dev, uint8_t *rxbuf, uint16_t rxlen);
#endif

#endif /* __I2C_DEV_H__ */
