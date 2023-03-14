/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "sensor_platform_if.h"
#include <securec.h>
#include "osal_io.h"
#include "osal_time.h"

#define HDF_LOG_TAG    hdf_sensor_commom

#define I2C_READ_MSG_NUM           2
#define I2C_READ_MSG_ADDR_IDX      0
#define I2C_READ_MSG_VALUE_IDX     1

#define I2C_WRITE_MSG_NUM  1
#define I2C_REG_BUF_LEN    4
#define I2C_BYTE_MASK      0xFF
#define I2C_BYTE_OFFSET    8

#define SENSOR_STACK_SIZE  0x2000    // 4k buffer

int32_t ReadSensor(struct SensorBusCfg *busCfg, uint16_t regAddr, uint8_t *data, uint16_t dataLen)
{
    int index = 0;
    unsigned char regBuf[I2C_REG_BUF_LEN] = {0};
    struct I2cMsg msg[I2C_READ_MSG_NUM];

    CHECK_NULL_PTR_RETURN_VALUE(busCfg, HDF_FAILURE);
    CHECK_NULL_PTR_RETURN_VALUE(data, HDF_FAILURE);

    if (busCfg->busType == SENSOR_BUS_I2C) {
        CHECK_NULL_PTR_RETURN_VALUE(busCfg->i2cCfg.handle, HDF_FAILURE);

        msg[I2C_READ_MSG_ADDR_IDX].addr = busCfg->i2cCfg.devAddr;
        msg[I2C_READ_MSG_ADDR_IDX].flags = 0;
        msg[I2C_READ_MSG_ADDR_IDX].len = busCfg->i2cCfg.regWidth;
        msg[I2C_READ_MSG_ADDR_IDX].buf = regBuf;

        if (busCfg->i2cCfg.regWidth == SENSOR_ADDR_WIDTH_1_BYTE) {
            regBuf[index++] = regAddr & I2C_BYTE_MASK;
        } else if (busCfg->i2cCfg.regWidth == SENSOR_ADDR_WIDTH_2_BYTE) {
            regBuf[index++] = (regAddr >> I2C_BYTE_OFFSET) & I2C_BYTE_MASK;
            regBuf[index++] = regAddr & I2C_BYTE_MASK;
        } else {
            HDF_LOGE("%s: i2c regWidth[%u] failed", __func__, busCfg->i2cCfg.regWidth);
            return HDF_FAILURE;
        }

        msg[I2C_READ_MSG_VALUE_IDX].addr = busCfg->i2cCfg.devAddr;
        msg[I2C_READ_MSG_VALUE_IDX].flags = I2C_FLAG_READ;
        msg[I2C_READ_MSG_VALUE_IDX].len = dataLen;
        msg[I2C_READ_MSG_VALUE_IDX].buf = data;

        if (I2cTransfer(busCfg->i2cCfg.handle, msg, I2C_READ_MSG_NUM) != I2C_READ_MSG_NUM) {
            HDF_LOGE("%s: i2c[%u] read failed", __func__, busCfg->i2cCfg.busNum);
            return HDF_FAILURE;
        }

#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_SPI) || defined(CONFIG_DRIVERS_HDF_PLATFORM_SPI)
    } else if (busCfg->busType == SENSOR_BUS_SPI) {
        if (SpiRead(busCfg->spiCfg.handle, data, dataLen) != HDF_SUCCESS) {
            HDF_LOGE("%s: spi read failed", __func__);
            return HDF_FAILURE;
        }
#endif
    }

    return HDF_SUCCESS;
}

int32_t WriteSensor(struct SensorBusCfg *busCfg, uint8_t *writeData, uint16_t dataLen)
{
    struct I2cMsg msg[I2C_WRITE_MSG_NUM];

    CHECK_NULL_PTR_RETURN_VALUE(busCfg, HDF_FAILURE);
    CHECK_NULL_PTR_RETURN_VALUE(writeData, HDF_FAILURE);

    if (busCfg->busType == SENSOR_BUS_I2C) {
        CHECK_NULL_PTR_RETURN_VALUE(busCfg->i2cCfg.handle, HDF_FAILURE);

        msg[0].addr = busCfg->i2cCfg.devAddr;
        msg[0].flags = 0;
        msg[0].len = dataLen;
        msg[0].buf = writeData;

        if (I2cTransfer(busCfg->i2cCfg.handle, msg, I2C_WRITE_MSG_NUM) != I2C_WRITE_MSG_NUM) {
            HDF_LOGE("%s: i2c[%u] write failed", __func__, busCfg->i2cCfg.busNum);
            return HDF_FAILURE;
        }

#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_SPI) || defined(CONFIG_DRIVERS_HDF_PLATFORM_SPI)
    } else if (busCfg->busType == SENSOR_BUS_SPI) {
        if (SpiWrite(busCfg->spiCfg.handle, writeData, dataLen) != HDF_SUCCESS) {
            HDF_LOGE("%s: spi write failed", __func__);
            return HDF_FAILURE;
        }
#endif
    }

    return HDF_SUCCESS;
}

int32_t SetSensorPinMux(uint32_t regAddr, int32_t regSize, uint32_t regValue)
{
    uint8_t *base = NULL;
    if (regAddr == 0) {
        HDF_LOGE("%s: regAddr invalid", __func__);
        HDF_LOGE("%s: regSize = %d, regValue = %u", __func__, regSize, regValue);
        return HDF_FAILURE;
    }

#if !defined(CONFIG_ARCH_ROCKCHIP)
    base = OsalIoRemap(regAddr, regSize);
    if (base == NULL) {
        HDF_LOGE("%s: ioremap fail", __func__);
        return HDF_FAILURE;
    }

    OSAL_WRITEL(regValue, base);
    OsalIoUnmap((void *)base);
#endif

    return HDF_SUCCESS;
}
