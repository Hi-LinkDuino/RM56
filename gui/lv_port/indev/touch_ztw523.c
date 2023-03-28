/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifdef CONFIG_DISPLAY_RM69330
#include "touch_ztw523.h"
#include "gpio_if.h"
#include "i2c_if.h"
#include "lv_port_indev.h"
#include "platform_if.h"

#define CONFIG_ROTATION
#define IIC_RETRY_NUM 2

struct ts_zinitix_coord {
    uint16_t x;
    uint16_t y;
    uint8_t width;
    uint8_t sub_status;
};

struct ts_zinitix_point_info {
    uint16_t status;
#if TOUCH_POINT_MODE
    uint16_t event_flag;
#else
    uint8_t finger_cnt;
    uint8_t time_stamp;
#endif
    struct ts_zinitix_coord coord[MAX_SUPPORTED_FINGER_NUM];
};

struct ztw_priv {
    struct touch_device *dev;
    DevHandle i2c;
    uint16_t i2c_id;
    uint16_t i2c_addr;
    uint16_t gpio_rst;
    uint16_t gpio_int;
};

/**
 * touch pins:
 * TSP_RST - GPIO12 = 10
 * TSP_INT - GPIO27 = 23
 * TSP_SCL/SDA - I2C0 = GPIO04/GPIO05
 */
static struct ztw_priv priv = {
    .gpio_rst = 10,
    .gpio_int = 23,
    .i2c_id = 0,
    .i2c_addr = 0x20,
};

#define DelayMs(ms) osDelay(ms)
#define DelayUs(us) osDelay(((us) <= 1000) ? 1 : (us) / 1000)

static int ztw_read(struct ztw_priv *priv, uint16_t reg_addr, void *buffer, uint16_t length)
{
    int ret = -1;
    int retries = 0;

    struct I2cMsg msgs[] = {
        {
            .addr = priv->i2c_addr,
            .flags = 0,
            .len = sizeof(reg_addr),
            .buf = (uint8_t *)&reg_addr,
        },
        {
            .addr = priv->i2c_addr,
            .flags = I2C_FLAG_READ,
            .len = length,
            .buf = (uint8_t *)buffer,
        },
    };

    while (retries < IIC_RETRY_NUM) {
        ret = I2cTransfer(priv->i2c, msgs, 2);
        if (ret == 2)
            break;
        retries++;
        DelayMs(1);
    }

    if (retries >= IIC_RETRY_NUM) {
        LOG_E("i2c transfer failed ret %d", ret);
        return -1;
    }
    return (ret == 2) ? 0 : -1;
}

static int ztw_write(struct ztw_priv *priv, uint16_t reg_addr, const void *buffer, uint16_t length)
{
    uint8_t send_buffer[length + 2];
    send_buffer[0] = reg_addr & 0xff;
    send_buffer[1] = (reg_addr >> 8) & 0xff;
    memcpy(&send_buffer[2], buffer, length);

    struct I2cMsg msg = {
        .addr = priv->i2c_addr,
        .flags = 0,
        .len = length + 2,
        .buf = send_buffer,
    };

    int ret = I2cTransfer(priv->i2c, &msg, 1);
    return (ret == 1) ? 0 : -1;
}

static int ztw_write_reg(struct ztw_priv *priv, uint16_t reg_addr, uint16_t data)
{
    return ztw_write(priv, reg_addr, (uint8_t *)&data, 2);
}

static int ztw_read_reg(struct ztw_priv *priv, uint16_t reg_addr, uint16_t *data)
{
    return ztw_read(priv, reg_addr, (uint8_t *)data, 2);
}

static int ztw_write_cmd(struct ztw_priv *priv, uint16_t cmd)
{
    uint16_t data = 1;
    return ztw_write_reg(priv, cmd, data);
}

static int ztw_get_point(struct touch_device *dev, struct touch_msg *msg)
{
    struct ts_zinitix_point_info touch_info;
    if (ztw_read(&priv, ZINITIX_POINT_STATUS_REG, (uint8_t *)&touch_info, sizeof(touch_info)) < 0) {
        LOG_E("failed to read ZINITIX_POINT_STATUS_REG");
        return -1;
    }
    ztw_write_cmd(&priv, ZINITIX_CLEAR_INT_STATUS_CMD);
    if (touch_info.status == 0) {
        LOG_E("invalid touch_info");
        return -1;
    }
    for (uint8_t i = 0; i < MAX_SUPPORTED_FINGER_NUM; i++) {
        uint8_t sub_status = touch_info.coord[i].sub_status;
        if (zinitix_bit_test(sub_status, SUB_BIT_DOWN) || zinitix_bit_test(sub_status, SUB_BIT_MOVE)) {
            msg->event = TOUCH_EVENT_DOWN;
#ifdef CONFIG_ROTATION
            msg->x = TPD_RES_MAX_X - touch_info.coord[i].x;
            msg->y = TPD_RES_MAX_Y - touch_info.coord[i].y;
#else
            msg->x = touch_info.coord[i].x;
            msg->y = touch_info.coord[i].y;
#endif
        } else if (zinitix_bit_test(sub_status, SUB_BIT_UP)) {
            msg->event = TOUCH_EVENT_UP;
#ifdef CONFIG_ROTATION
            msg->x = TPD_RES_MAX_X - touch_info.coord[i].x;
            msg->y = TPD_RES_MAX_Y - touch_info.coord[i].y;
#else
            msg->x = touch_info.coord[i].x;
            msg->y = touch_info.coord[i].y;
#endif
        } else {
            LOG_E("unknown status:%d", sub_status);
            return -1;
        }
        LOG_D("EVENT:%d X:%d Y:%d", msg->event, msg->x, msg->y);
    }
    return 0;
}

static void ztw_reset(struct ztw_priv *priv, bool enable)
{
    if (enable) {
        DelayMs(1);
        GpioWrite(priv->gpio_rst, 1);
        DelayMs(10);
        GpioWrite(priv->gpio_rst, 0);
        DelayMs(100);
        GpioWrite(priv->gpio_rst, 1);
        DelayMs(CHIP_ON_DELAY);
    } else {
        GpioWrite(priv->gpio_rst, 0);
        DelayMs(CHIP_OFF_DELAY);
    }
}

static int ztw_power_sequence(struct ztw_priv *priv)
{
    uint16_t chip_code;
    if (ztw_write_reg(priv, ZINITIX_VENDOR_REG, 0x0001) != 0) {
        LOG_E("power sequence error (vendor cmd enable)");
        return -1;
    }
    DelayUs(10);

    if (ztw_read_reg(priv, ZINITIX_CHIP_ID_REG, &chip_code) != 0) {
        LOG_E("fail to read chip code");
        return -1;
    }
    LOG_I("chip code = 0x%x", chip_code);
    DelayUs(10);

    if (ztw_write_cmd(priv, ZINITIX_INTN_CLEAR_CMD) != 0) {
        LOG_E("power sequence error (int clear)");
        return -1;
    }
    DelayUs(10);

    if (ztw_write_reg(priv, ZINITIX_NVM_REG, 0x0001) != 0) {
        LOG_E("power sequence error (nvm init)");
        return -1;
    }
    DelayMs(2);

    if (ztw_write_reg(priv, ZINITIX_PROGRAM_START_CMD, 0x0001) != 0) {
        LOG_E("power sequence error (program start)");
        return -1;
    }
    DelayMs(FIRMWARE_ON_DELAY);
    LOG_D("ztw_power_sequence PASS.");
    return 0;
}

static void ztw_irq_enable(bool enable)
{
    if (enable) {
        GpioEnableIrq(priv.gpio_int);
    } else {
        GpioDisableIrq(priv.gpio_int);
    }
}

static void ztw_isr(uint16_t gpio, void *arg)
{
    (void)arg;
    if (gpio != priv.gpio_int)
        return;

    if (priv.dev && priv.dev->sem)
        osSemaphoreRelease(priv.dev->sem);
}

static int ztw_init(struct touch_device *dev)
{
    priv.dev = dev;

    GpioSetDir(priv.gpio_rst, GPIO_DIR_OUT);
    GpioSetDir(priv.gpio_int, GPIO_DIR_IN_PULLUP);
    GpioSetIrq(priv.gpio_int, GPIO_IRQ_TRIGGER_FALLING, (GpioIrqFunc)ztw_isr, NULL);
    GpioDisableIrq(priv.gpio_int);
    priv.i2c = I2cOpen(priv.i2c_id);
    DBG_ASSERT(priv.i2c != NULL);

    ztw_reset(&priv, true);
    if (ztw_power_sequence(&priv) != 0) {
        LOG_E("failed to power on");
        return -1;
    }

    uint8_t i;
    for (i = 0; i < 10; i++) {
        if (ztw_write_cmd(&priv, ZINITIX_SWRESET_CMD) == 0)
            break;
        DelayMs(10);
    }
    uint16_t reg_val = 0;
    zinitix_bit_set(reg_val, BIT_PT_CNT_CHANGE);
    zinitix_bit_set(reg_val, BIT_DOWN);
    zinitix_bit_set(reg_val, BIT_MOVE);
    zinitix_bit_set(reg_val, BIT_UP);
    // zinitix_bit_set(reg_val, BIT_PALM);
    // zinitix_bit_set(reg_val, BIT_PT_EXIST);

    uint16_t eeprom_info;
    if (ztw_read_reg(&priv, ZINITIX_EEPROM_INFO, &eeprom_info) != 0) {
        LOG_E("fail to read eeprom info");
        return -1;
    }
    LOG_I("eeprom_info = 0x%x", eeprom_info);

    ztw_write_reg(&priv, ZINITIX_SUPPORTED_FINGER_NUM, MAX_SUPPORTED_FINGER_NUM);
    ztw_write_reg(&priv, ZINITIX_X_RESOLUTION, (TPD_RES_MAX_X));
    ztw_write_reg(&priv, ZINITIX_Y_RESOLUTION, (TPD_RES_MAX_Y));

    ztw_write_reg(&priv, ZINITIX_INITIAL_TOUCH_MODE, TOUCH_POINT_MODE);
    ztw_write_reg(&priv, ZINITIX_TOUCH_MODE, TOUCH_POINT_MODE);
    ztw_write_reg(&priv, ZINITIX_INT_ENABLE_FLAG, reg_val);

    for (i = 0; i < 10; i++) {
        ztw_write_cmd(&priv, ZINITIX_CLEAR_INT_STATUS_CMD);
        DelayUs(10);
    }
    return 0;
}

static void ztw_deinit(void)
{
    GpioDisableIrq(priv.gpio_int);
    I2cClose(priv.i2c);
}

struct touch_device g_touch_ztw523 = {
    .name = "ztw523",
    .init = ztw_init,
    .deinit = ztw_deinit,
    .read = ztw_get_point,
    .irq_enable = ztw_irq_enable,
};
#endif