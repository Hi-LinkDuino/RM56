/*
 * Copyright (c) 2021 bestechnic (Shanghai) Technologies CO., LIMITED.
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
#ifdef CONFIG_DISPLAY_A064
#include "touch_fts.h"
#include "cmsis_os2.h"
#include "gpio_if.h"
#include "i2c_if.h"

static int tpd_interrupt_handler(uint16_t gpio, void *arg);

struct tpd_priv {
    struct touch_device *dev;
    struct i2c_client i2c_client;
    uint16_t gpio_rst;
    uint16_t gpio_int;
};

static struct tpd_priv priv = {
    .gpio_rst = 5,  // TSP_RST - GPIO05
    .gpio_int = 23, // TSP_INT - GPIO27
    .i2c_client = {
        .id = 1, // TSP_SCL/SDA - I2C1 = GPIO06/GPIO07
        .addr = 0x38,
    }};

/*****************************************************************************
 * i2c related
 *****************************************************************************/
#define I2C_RETRY_NUMBER 3
static int fts_i2c_read(struct i2c_client *client, uint8_t *writebuf, int writelen, uint8_t *readbuf, int readlen)
{
    int ret = 0;
    int i = 0;

    if (client == NULL) {
        HDF_LOGE("[IIC][%s]i2c_client==NULL!", __func__);
        return -EINVAL;
    }

    if (readlen > 0) {
        if (writelen > 0) {
            struct I2cMsg msgs[] = {
                {
                    .addr = client->addr,
                    .flags = 0,
                    .len = writelen,
                    .buf = writebuf,
                },
                {
                    .addr = client->addr,
                    .flags = I2C_FLAG_READ,
                    .len = readlen,
                    .buf = readbuf,
                },
            };
            for (i = 0; i < I2C_RETRY_NUMBER; i++) {
                ret = I2cTransfer(client->handle, msgs, 2);
                if (ret < 0) {
                    HDF_LOGE("[IIC]: i2c_transfer(write) error, ret=%d!!", ret);
                } else
                    break;
            }
        } else {
            struct I2cMsg msgs[] = {
                {
                    .addr = client->addr,
                    .flags = I2C_FLAG_READ,
                    .len = readlen,
                    .buf = readbuf,
                },
            };
            for (i = 0; i < I2C_RETRY_NUMBER; i++) {
                ret = I2cTransfer(client->handle, msgs, 1);
                if (ret < 0) {
                    HDF_LOGE("[IIC]: i2c_transfer(read) error, ret=%d!!", ret);
                } else
                    break;
            }
        }
    }

    return ret;
}

static int fts_i2c_write(struct i2c_client *client, uint8_t *writebuf, int writelen)
{
    int ret = 0;
    int i = 0;

    if (client == NULL) {
        HDF_LOGE("[IIC][%s]i2c_client==NULL!", __func__);
        return -EINVAL;
    }

    if (writelen > 0) {
        struct I2cMsg msgs[] = {
            {
                .addr = client->addr,
                .flags = 0,
                .len = writelen,
                .buf = writebuf,
            },
        };
        for (i = 0; i < I2C_RETRY_NUMBER; i++) {
            ret = I2cTransfer(client->handle, msgs, 1);
            if (ret < 0) {
                HDF_LOGE("[IIC]: i2c_transfer(write) error, ret=%d!!", ret);
            } else
                break;
        }
    }

    return ret;
}

static int fts_i2c_write_reg(struct i2c_client *client, uint8_t regaddr, uint8_t regvalue)
{
    uint8_t buf[2] = {0};

    buf[0] = regaddr;
    buf[1] = regvalue;
    return fts_i2c_write(client, buf, sizeof(buf));
}

static int fts_i2c_read_reg(struct i2c_client *client, uint8_t regaddr, uint8_t *regvalue)
{
    return fts_i2c_read(client, &regaddr, 1, regvalue, 1);
}

static void fts_i2c_hid2std(struct i2c_client *client)
{
    int ret = 0;
    uint8_t buf[3] = {0xeb, 0xaa, 0x09};

    ret = fts_i2c_write(client, buf, 3);
    if (ret < 0)
        HDF_LOGE("hid2std cmd write fail");
    else {
        osDelay(10);
        buf[0] = buf[1] = buf[2] = 0;
        ret = fts_i2c_read(client, NULL, 0, buf, 3);
        if (ret < 0)
            HDF_LOGE("hid2std cmd read fail");
        else if ((0xeb == buf[0]) && (0xaa == buf[1]) && (0x08 == buf[2])) {
            HDF_LOGE("hidi2c change to stdi2c successful");
        } else {
            HDF_LOGE("hidi2c change to stdi2c fail");
        }
    }
}

/*****************************************************************************
 * chip related
 *****************************************************************************/
#define INTERVAL_READ_REG 100 /* interval time per read reg unit:ms */
#define TIMEOUT_READ_REG 1000 /* timeout of read reg unit:ms */
#define FTS_I2C_SLAVE_ADDR 0x38

static struct fts_ts_data *fts_data;

static int fts_get_chip_types(struct fts_ts_data *ts_data, uint8_t id_h, uint8_t id_l, bool fw_valid)
{
    int i = 0;
    struct ft_chip_t ctype[] = FTS_CHIP_TYPE_MAPPING;
    uint32_t ctype_entries = sizeof(ctype) / sizeof(struct ft_chip_t);

    if ((0x0 == id_h) || (0x0 == id_l)) {
        HDF_LOGE("id_h/id_l is 0");
        return -EINVAL;
    }

    HDF_LOGI("verify id:0x%02x%02x", id_h, id_l);
    for (i = 0; i < ctype_entries; i++) {
        if (VALID == fw_valid) {
            if ((id_h == ctype[i].chip_idh) && (id_l == ctype[i].chip_idl))
                break;
        } else {
            if (((id_h == ctype[i].rom_idh) && (id_l == ctype[i].rom_idl)) || ((id_h == ctype[i].pb_idh) && (id_l == ctype[i].pb_idl)) || ((id_h == ctype[i].bl_idh) && (id_l == ctype[i].bl_idl)))
                break;
        }
    }

    if (i >= ctype_entries) {
        return -ENODATA;
    }

    ts_data->ic_info.ids = ctype[i];
    return 0;
}

static int fts_get_ic_information(struct fts_ts_data *ts_data)
{
    int ret = 0;
    int cnt = 0;
    uint8_t chip_id[2] = {0};
    uint8_t id_cmd[4] = {0};
    uint32_t id_cmd_len = 0;
    struct i2c_client *client = ts_data->client;

    ts_data->ic_info.is_incell = FTS_CHIP_IDC;
    ts_data->ic_info.hid_supported = FTS_HID_SUPPORTTED;
    do {
        ret = fts_i2c_read_reg(client, FTS_REG_CHIP_ID, &chip_id[0]);
        ret = fts_i2c_read_reg(client, FTS_REG_CHIP_ID2, &chip_id[1]);
        if ((ret < 0) || (0x0 == chip_id[0]) || (0x0 == chip_id[1])) {
            HDF_LOGD("i2c read invalid, read:0x%02x%02x", chip_id[0], chip_id[1]);
        } else {
            ret = fts_get_chip_types(ts_data, chip_id[0], chip_id[1], VALID);
            if (!ret)
                break;
            else
                HDF_LOGD("TP not ready, read:0x%02x%02x", chip_id[0], chip_id[1]);
        }

        cnt++;
        osDelay(INTERVAL_READ_REG);
    } while ((cnt * INTERVAL_READ_REG) < TIMEOUT_READ_REG);

    if ((cnt * INTERVAL_READ_REG) >= TIMEOUT_READ_REG) {
        HDF_LOGI("fw is invalid, need read boot id");
        if (ts_data->ic_info.hid_supported) {
            fts_i2c_hid2std(client);
        }

        id_cmd[0] = FTS_CMD_START1;
        id_cmd[1] = FTS_CMD_START2;
        ret = fts_i2c_write(client, id_cmd, 2);
        if (ret < 0) {
            HDF_LOGE("start cmd write fail");
            return ret;
        }

        osDelay(FTS_CMD_START_DELAY);
        id_cmd[0] = FTS_CMD_READ_ID;
        id_cmd[1] = id_cmd[2] = id_cmd[3] = 0x00;
        if (ts_data->ic_info.is_incell)
            id_cmd_len = FTS_CMD_READ_ID_LEN_INCELL;
        else
            id_cmd_len = FTS_CMD_READ_ID_LEN;
        ret = fts_i2c_read(client, id_cmd, id_cmd_len, chip_id, 2);
        if ((ret < 0) || (0x0 == chip_id[0]) || (0x0 == chip_id[1])) {
            HDF_LOGE("read boot id fail");
            return -EIO;
        }
        ret = fts_get_chip_types(ts_data, chip_id[0], chip_id[1], 0);
        if (ret < 0) {
            HDF_LOGE("can't get ic informaton");
            return ret;
        }
    }

    HDF_LOGI("get ic information, chip id = 0x%02x%02x",
             ts_data->ic_info.ids.chip_idh, ts_data->ic_info.ids.chip_idl);

    return 0;
}

static int fts_reset_proc(int hdelayms)
{
    GpioWrite(priv.gpio_rst, 0);
    osDelay(5);
    GpioWrite(priv.gpio_rst, 1);
    if (hdelayms) {
        osDelay(hdelayms);
    }
    return 0;
}

static int fts_read_touchdata(struct fts_ts_data *data)
{
    int ret = 0;
    int i = 0;
    uint8_t pointid;
    int base;
    struct ts_event *events = data->events;
    int max_touch_num = FTS_TOUCH_POINT_NUM;
    uint8_t *buf = data->point_buf;

    data->point_num = 0;
    data->touch_point = 0;

    memset(buf, 0xFF, data->pnt_buf_size);
    buf[0] = 0x00;

    ret = fts_i2c_read(data->client, buf, 1, buf, data->pnt_buf_size);
    if (ret < 0) {
        HDF_LOGE("read touchdata failed, ret:%d", ret);
        return ret;
    }
    data->point_num = buf[FTS_TOUCH_POINT_NUM] & 0x0F;

    if (data->ic_info.is_incell) {
        if ((data->point_num == 0x0F) && (buf[1] == 0xFF) && (buf[2] == 0xFF) && (buf[3] == 0xFF) &&
            (buf[4] == 0xFF) && (buf[5] == 0xFF) && (buf[6] == 0xFF)) {
            HDF_LOGI("touch buff is 0xff, need recovery state");
            return -EIO;
        }
    }
    if (data->point_num > max_touch_num) {
        HDF_LOGI("invalid point_num(%d)", data->point_num);
        return -EIO;
    }

    for (i = 0; i < max_touch_num; i++) {
        base = FTS_ONE_TCH_LEN * i;

        pointid = (buf[FTS_TOUCH_ID_POS + base]) >> 4;
        if (pointid >= FTS_MAX_ID)
            break;
        else if (pointid >= max_touch_num) {
            HDF_LOGE("ID(%d) beyond max_touch_number", pointid);
            return -EINVAL;
        }

        data->touch_point++;

        events[i].x = ((buf[FTS_TOUCH_X_H_POS + base] & 0x0F) << 8) +
                      (buf[FTS_TOUCH_X_L_POS + base] & 0xFF);
        events[i].y = ((buf[FTS_TOUCH_Y_H_POS + base] & 0x0F) << 8) +
                      (buf[FTS_TOUCH_Y_L_POS + base] & 0xFF);
        events[i].flag = buf[FTS_TOUCH_EVENT_POS + base] >> 6;
        events[i].id = buf[FTS_TOUCH_ID_POS + base] >> 4;
        events[i].area = buf[FTS_TOUCH_AREA_POS + base] >> 4;
        events[i].p = buf[FTS_TOUCH_PRE_POS + base];

        if (EVENT_DOWN(events[i].flag) && (data->point_num == 0)) {
            HDF_LOGI("abnormal touch data from fw");
            return -EIO;
        }
    }
    if (data->touch_point == 0) {
        HDF_LOGI("no touch point information");
        return -EIO;
    }

    return 0;
}

static int tpd_irq_registration(struct fts_ts_data *ts_data)
{
    GpioSetDir(priv.gpio_int, GPIO_DIR_IN); // GPIO_DIR_IN_PULLUP
    GpioSetIrq(priv.gpio_int, GPIO_IRQ_TRIGGER_FALLING, tpd_interrupt_handler, NULL);
    GpioDisableIrq(priv.gpio_int);
    HDF_LOGI("IRQ request succussfully, irq:%d", priv.gpio_int);
    return 0;
}

static int fts_input_init(struct fts_ts_data *ts_data)
{
    int point_num = 0;
    int ret = 0;

    point_num = FTS_TOUCH_POINT_NUM;
    ts_data->pnt_buf_size = point_num * FTS_ONE_TCH_LEN + 3;
    ts_data->point_buf = (uint8_t *)malloc(ts_data->pnt_buf_size);
    if (!ts_data->point_buf) {
        HDF_LOGE("failed to alloc memory for point buf!");
        ret = -ENOMEM;
        goto err_point_buf;
    }
    memset(ts_data->point_buf, 0, ts_data->pnt_buf_size);

    ts_data->events = (struct ts_event *)malloc(point_num * sizeof(struct ts_event));
    if (!ts_data->events) {
        HDF_LOGE("failed to alloc memory for point events!");
        ret = -ENOMEM;
        goto err_event_buf;
    }
    memset(ts_data->events, 0, point_num * sizeof(struct ts_event));

    return 0;

err_event_buf:
    free(ts_data->point_buf);

err_point_buf:
    return ret;
}

static int tpd_probe(struct i2c_client *client)
{
    int ret = 0;
    struct fts_ts_data *ts_data;

    ts_data = malloc(sizeof(*ts_data));
    if (!ts_data) {
        HDF_LOGE("Failed to allocate memory for fts_data");
        return -ENOMEM;
    }
    memset(ts_data, 0, sizeof(*ts_data));
    fts_data = ts_data;
    ts_data->client = client;

    if (client->addr != FTS_I2C_SLAVE_ADDR) {
        HDF_LOGI("[TPD]Change i2c addr 0x%02x to %x", client->addr, FTS_I2C_SLAVE_ADDR);
        client->addr = FTS_I2C_SLAVE_ADDR;
        HDF_LOGI("[TPD]i2c addr=0x%x\n", client->addr);
    }

    ret = fts_input_init(ts_data);
    if (ret) {
        HDF_LOGE("fts input initialize fail");
        goto err_input_init;
    }

    fts_reset_proc(100);
    ret = fts_get_ic_information(ts_data);
    if (ret) {
        HDF_LOGE("not focal IC, unregister driver");
        goto err_input_init;
    }

    HDF_LOGD("[TPD]Touch Panel Device Probe %s!", (ret < 0) ? "FAIL" : "PASS");

    /* Configure gpio to irq and request irq */
    ret = tpd_irq_registration(ts_data);
    if (ret) {
        HDF_LOGE("request irq failed");
        goto err_irq_req;
    }

    HDF_LOGD("TPD_RES_Y:%d", (int)TPD_RES_Y);
    return 0;

err_irq_req:
err_input_init:
    free(ts_data->point_buf);
    free(ts_data->events);
    free(ts_data);

    return ret;
}

static int tpd_remove()
{
    if (fts_data) {
        free(fts_data->point_buf);
        free(fts_data->events);
        free(fts_data);
    }
    return 0;
}

/*****************************************************************************
 * interfaces
 *****************************************************************************/
static int tpd_get_point(struct touch_device *dev, struct touch_msg *msg)
{
    int ret = fts_read_touchdata(fts_data);
    if (ret) {
        return -1;
    }

    struct ts_event *events = fts_data->events;
    msg->x = events[0].x;
    msg->y = events[0].y;
    msg->event = EVENT_DOWN(events[0].flag) ? TOUCH_EVENT_DOWN : TOUCH_EVENT_UP;
    return 0;
}

static int tpd_interrupt_handler(uint16_t gpio, void *arg)
{
    (void)arg;
    if (gpio != priv.gpio_int)
        return -1;

    if (priv.dev && priv.dev->sem)
        osSemaphoreRelease(priv.dev->sem);

    return 0;
}

static void tpd_irq_enable(bool enable)
{
    if (enable) {
        GpioEnableIrq(priv.gpio_int);
    } else {
        GpioDisableIrq(priv.gpio_int);
    }
}

static int tpd_init(struct touch_device *dev)
{
    priv.dev = dev;
    priv.i2c_client.handle = I2cOpen(priv.i2c_client.id);
    if (priv.i2c_client.handle == NULL) {
        return -1;
    }

    if (tpd_probe(&priv.i2c_client)) {
        return -1;
    }

    return 0;
}

static void tpd_deinit(void)
{
    GpioDisableIrq(priv.gpio_int);
    I2cClose(priv.i2c_client.handle);
    tpd_remove(&priv.i2c_client);
}

struct touch_device g_touch_fts = {
    .name = "fts_ts",
    .init = tpd_init,
    .deinit = tpd_deinit,
    .read = tpd_get_point,
    .irq_enable = tpd_irq_enable,
};
#endif