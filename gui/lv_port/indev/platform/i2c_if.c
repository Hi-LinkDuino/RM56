#include "cmsis_os2.h"
#include "hal_i2c.h"
#include "hal_iomux.h"
#include "i2c_if.h"

struct I2cDevice {
    enum HAL_I2C_ID_T id;
    struct HAL_I2C_CONFIG_T cfg;
    osMutexId_t mutex;
};

/**
 * default I2C0_IOMUX_INDEX 4, SCL - PIN04, SDA - PIN05
 * default I2C1_IOMUX_INDEX 22, SCL - PIN22, SDA - PIN23
 * default I2C2_IOMUX_INDEX 10, SCL - PIN10, SDA - PIN11
 */
static struct I2cDevice devs[] = {
    {
        .id = HAL_I2C_ID_0,
        .cfg = {
            .speed = 100000,
            .mode = HAL_I2C_API_MODE_SIMPLE, // or HAL_I2C_API_MODE_TASK
            .use_dma = 0,
            .use_sync = 1,
            .as_master = 1,
        },
    },
#if (CHIP_HAS_I2C >= 2)
    {
        .id = HAL_I2C_ID_1,
        .cfg = {
            .speed = 200000,
            .mode = HAL_I2C_API_MODE_SIMPLE,
            .use_dma = 0,
            .use_sync = 1,
            .as_master = 1,
        },
    },
#endif
};

static void I2cMuxCfg(enum HAL_I2C_ID_T id)
{
    switch (id) {
    case HAL_I2C_ID_0:
        hal_iomux_set_i2c0();
        break;
#if (CHIP_HAS_I2C >= 2)
    case HAL_I2C_ID_1:
        hal_iomux_set_i2c1();
        break;
#endif
    default:
        break;
    }
}

DevHandle I2cOpen(int16_t id)
{
    RETURN_VAL_IF_FAIL(id < P_ARRAY_SIZE(devs), NULL);
    struct I2cDevice *dev = &devs[id];
    if (dev->mutex) {
        return (DevHandle)dev;
    }
    dev->mutex = osMutexNew(NULL);
    DBG_ASSERT(dev->mutex != NULL);
    osMutexAcquire(dev->mutex, osWaitForever);
    I2cMuxCfg(dev->id);
    if (hal_i2c_open(dev->id, &dev->cfg) != 0) {
        osMutexRelease(dev->mutex);
        LOG_E("hal_i2c_open failed");
        return NULL;
    }
    osMutexRelease(dev->mutex);
    return (DevHandle)dev;
}

void I2cClose(DevHandle handle)
{
    RETURN_IF_FAIL(handle != NULL);
    struct I2cDevice *dev = (struct I2cDevice *)handle;
    if (dev->mutex) {
        hal_i2c_close(dev->id);
        osMutexDelete(dev->mutex);
        dev->mutex = NULL;
    }
}

static uint32_t I2cWrite(const struct I2cDevice *dev, uint16_t addr, const uint8_t *txbuf, uint16_t txlen)
{
    uint32_t ret;
    if (dev->cfg.mode == HAL_I2C_API_MODE_SIMPLE)
        ret = hal_i2c_simple_send(dev->id, addr, txbuf, txlen);
    else
        ret = hal_i2c_task_send(dev->id, addr, txbuf, txlen, 0, NULL);

    if (ret) {
        LOG_E("i2c addr 0x%x, ret 0x%x", addr, ret);
    }
    return ret;
}

static uint32_t I2cRead(const struct I2cDevice *dev, uint16_t addr, uint8_t *rxbuf, uint16_t rxlen)
{
    uint32_t ret;
    if (dev->cfg.mode == HAL_I2C_API_MODE_SIMPLE)
        ret = hal_i2c_simple_recv(dev->id, addr, NULL, 0, rxbuf, rxlen);
    else
        ret = hal_i2c_task_recv(dev->id, addr, NULL, 0, rxbuf, rxlen, 0, NULL);

    if (ret) {
        LOG_E("i2c addr 0x%x, ret 0x%x", addr, ret);
    }
    return ret;
}

int32_t I2cTransfer(DevHandle handle, struct I2cMsg *msgs, int16_t count)
{
    RETURN_VAL_IF_FAIL(handle != NULL && msgs != NULL && count > 0, -1);
    struct I2cDevice *dev = (struct I2cDevice *)handle;
    RETURN_VAL_IF_FAIL(dev->mutex != NULL, -1);
    osMutexAcquire(dev->mutex, osWaitForever);
    int32_t cnt = 0;
    for (int16_t i = 0; i < count; i++) {
        struct I2cMsg *msg = &msgs[i];
        if (msg->flags & I2C_FLAG_READ) {
            if (I2cRead(dev, msg->addr, msg->buf, msg->len)) {
                LOG_E("I2cRead failed");
                break;
            }
            cnt++;
        } else {
            if (I2cWrite(dev, msg->addr, msg->buf, msg->len)) {
                LOG_E("I2cWrite failed");
                break;
            }
            cnt++;
        }
        osDelay(1);
    }
    osMutexRelease(dev->mutex);
    return cnt;
}