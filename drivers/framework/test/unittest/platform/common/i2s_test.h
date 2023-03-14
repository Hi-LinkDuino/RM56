/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef I2S_TEST_H
#define I2S_TEST_H

#include "hdf_device_desc.h"
#include "platform_if.h"

enum I2sTestCmd {
    I2S_SET_CFG_TEST,
    I2S_GET_CFG_TEST,
    I2S_OPEN_TEST,
    I2S_CLOSE_TEST,
    I2S_ENABLE_TEST,
    I2S_DISABLE_TEST,
    I2S_READ_START_TEST,
    I2S_WRITE_START_TEST,
    I2S_WRITE_TEST,
    I2S_WRITE_STOP_TEST,
    I2S_READ_TEST,
    I2S_READ_STOP_TEST,
    I2S_RELIABILITY_TEST,
    I2S_RECORD_TEST,
    I2S_PLAY_TEST,
};

struct I2sTest {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    int32_t (*TestEntry)(struct I2sTest *test, int32_t cmd);
    DevHandle handle;
    uint8_t sampleRate;     /**< I2S sample rate, 8k,16k,32k... */
    uint8_t type;         /**< I2S protocol type */
    uint8_t channelMode;   /**< I2S mode,data channel */
    uint8_t samplePrecision;          /**< bit */
    uint8_t channelIfMode;
    uint8_t writeChannel;
    uint8_t i2slFsSel;
    uint32_t mclk;                     /**< KHZ */
    uint32_t bclk;                     /**< KHZ */
    uint32_t len;
    uint8_t *wbuf;
    uint8_t *rbuf;
};

static inline struct I2sTest *GetI2sTest(void)
{
    return (struct I2sTest *)DevSvcManagerClntGetService("I2S_TEST");
}

#endif /* I2S_TEST_H */
