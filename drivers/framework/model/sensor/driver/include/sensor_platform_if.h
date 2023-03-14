/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SENSOR_PLATFORM_IF_H
#define SENSOR_PLATFORM_IF_H

#include "hdf_log.h"
#include "i2c_if.h"
#include "spi_if.h"
#include "osal_thread.h"

#define CHECK_NULL_PTR_RETURN_VALUE(ptr, ret) do { \
    if ((ptr) == NULL) { \
        HDF_LOGE("%s:line %d pointer is null and return ret", __func__, __LINE__); \
        return (ret); \
    } \
} while (0)

#define CHECK_NULL_PTR_RETURN(ptr) do { \
    if ((ptr) == NULL) { \
        HDF_LOGE("%s:line %d pointer is null and return", __func__, __LINE__); \
        return; \
    } \
} while (0)

#define CHECK_PARSER_RESULT_RETURN_VALUE(ret, str) do { \
    if (ret != HDF_SUCCESS) { \
        HDF_LOGE("%s:line %d %s fail, ret = %d!", __func__, __LINE__, str, ret); \
        return HDF_FAILURE; \
    } \
} while (0)

#define SENSOR_DATA_SHIFT_LEFT(d, s)    ((d) << (s))
#define SENSOR_DATA_SHIFT_RIGHT(d, s)   ((d) >> (s))

#define SENSOR_ADDR_WIDTH_1_BYTE        1 // 8 bit
#define SENSOR_ADDR_WIDTH_2_BYTE        2 // 16 bit
#define SENSOR_ADDR_WIDTH_4_BYTE        4 // 16 bit
#define SENSOR_DATA_WIDTH_8_BIT         8 // 8 bit
#define SENSOR_CONVERT_UNIT             1000
#define SENSOR_1K_UNIT                  1024
#define SENSOR_SPI_MAX_SPEED            115200
#define SENSOR_SECOND_CONVERT_NANOSECOND    (SENSOR_CONVERT_UNIT * SENSOR_CONVERT_UNIT * SENSOR_CONVERT_UNIT)

#define SENSOR_TIMER_MIN_TIME           20

enum SensorBusType {
    SENSOR_BUS_I2C = 0,
    SENSOR_BUS_SPI = 1,
    SENSOR_BUS_GPIO = 2,
};

enum SensorGpioNum {
    SENSOR_GPIO_NUM1 = 0,
    SENSOR_GPIO_NUM2 = 1,
    SENSOR_GPIO_NUM_MAX = 2,
};

struct SensorI2cCfg {
    DevHandle handle;
    uint16_t busNum;
    uint16_t devAddr;  // Address of the I2C device
    uint16_t regWidth; // length of the register address
};

struct SensorSpiCfg {
    DevHandle handle;
    uint32_t busNum;
    uint32_t csNum;
};

struct SensorBusCfg {
    uint8_t busType; // enum SensorBusType
    uint8_t regBigEndian;
    union {
        struct SensorI2cCfg i2cCfg;
        struct SensorSpiCfg spiCfg;
        uint32_t GpioNum[SENSOR_GPIO_NUM_MAX];
    };
};

enum SENSORConfigValueIndex {
    SENSOR_ADDR_INDEX,
    SENSOR_VALUE_INDEX,
    SENSOR_VALUE_BUTT,
};

int32_t ReadSensor(struct SensorBusCfg *busCfg, uint16_t regAddr, uint8_t *data, uint16_t dataLen);
int32_t WriteSensor(struct SensorBusCfg *busCfg, uint8_t *writeData, uint16_t len);
int32_t SetSensorPinMux(uint32_t regAddr, int32_t regSize, uint32_t regValue);

#endif /* SENSOR_PLATFORM_IF_H */