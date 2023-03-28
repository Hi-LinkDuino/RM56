/***************************************************************************
 *
 * Copyright 2015-2021 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include "hal_timer.h"
#include "hal_trace.h"
#include "i2c_dev.h"

#ifdef CONFIG_I2C_HW
#define mdelay(ms) hal_sys_timer_delay(MS_TO_TICKS(ms))

#define AHT10_ADDR 0x38 /* slave address */

static uint8_t ath10_calibrate_cmd[] = {0xE1, 0x08, 0x00};
static uint8_t ath10_measure_cmd[] = {0xAC, 0x33, 0x00};
static uint8_t ath10_reset_cmd[] = {0xBA};

/**
 * default I2C0_IOMUX_INDEX 4, SCL - PIN04, SDA - PIN05
 * default I2C1_IOMUX_INDEX 22, SCL - PIN22, SDA - PIN23
 * default I2C2_IOMUX_INDEX 10, SCL - PIN10, SDA - PIN11
 */
static struct i2c_dev aht10_dev = {
    .name = "aht10",
    .addr = AHT10_ADDR,
    .bus = HAL_I2C_ID_0,
    .cfg = {
        .speed = 100000,
        .mode = HAL_I2C_API_MODE_SIMPLE,
        .use_dma = 0,
        .use_sync = 1,
        .as_master = 1,
    },
};

static void aht10_init(struct i2c_dev *dev)
{
    i2c_write(dev, ath10_calibrate_cmd, sizeof(ath10_calibrate_cmd));
    mdelay(40);
}

static void ath10_reset(struct i2c_dev *dev)
{
    i2c_write(dev, ath10_reset_cmd, sizeof(ath10_reset_cmd));
    mdelay(20);
}

static void aht10_read_temperature_humidity(struct i2c_dev *dev, double *temperature, double *humidity)
{
    uint8_t data[6] = {0};
    i2c_write(dev, ath10_measure_cmd, sizeof(ath10_measure_cmd));
    mdelay(80); // You'd better wait for aht10 measurement completed
    i2c_read(dev, data, 6);
    printf("rx: %02X %02X %02X %02X %02X %02X\r\n", data[0], data[1], data[2], data[3], data[4], data[5]);
    *humidity = (data[1] << 12 | data[2] << 4 | (data[3] & 0xf0) >> 4) * 100.0 / (1 << 20);
    *temperature = ((data[3] & 0xf) << 16 | data[4] << 8 | data[5]) * 200.0 / (1 << 20) - 50;
}

void i2c_test(void)
{
    int i;
    double temperature, humidity;
    i2c_init(&aht10_dev);
    ath10_reset(&aht10_dev);
    aht10_init(&aht10_dev);
    for (i = 0; i < 30; i++) {
        temperature = 0.0;
        humidity = 0.0;
        aht10_read_temperature_humidity(&aht10_dev, &temperature, &humidity);
        printf("aht10 temperature %.2f¡æ, humidity %.0f%%\r\n", temperature, humidity);
        mdelay(1000);
    }
}
#endif