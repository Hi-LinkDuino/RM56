/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef REGULATOR_TEST_H
#define REGULATOR_TEST_H

#include "hdf_device_desc.h"
#include "platform_if.h"

#ifdef REGULATOR_TEST_PRINTK_OPEN
#define REGULATOR_TEST_PRINT_LOG_DBG(fmt, arg...) \
dprintf("[HDF]-[REGULATOR]:[%s][%d]---" fmt "\r\n", __func__, __LINE__, ##arg)
#define REGULATOR_TEST_PRINT_LOG_INF(fmt, arg...) \
dprintf("[HDF]-[REGULATOR]:[%s][%d]---" fmt "\r\n", __func__, __LINE__, ##arg)
#define REGULATOR_TEST_PRINT_LOG_ERR(fmt, arg...) \
dprintf("[HDF]-[REGULATOR]:[%s][%d]---" fmt "\r\n", __func__, __LINE__, ##arg)
#else
#define REGULATOR_TEST_PRINT_LOG_DBG(fmt, arg...) HDF_LOGD_WRAPPER("[HDF]-[REGULATOR]:" fmt "\r\n",  ##arg)
#define REGULATOR_TEST_PRINT_LOG_INF(fmt, arg...) HDF_LOGI_WRAPPER("[HDF]-[REGULATOR]:" fmt "\r\n",  ##arg)
#define REGULATOR_TEST_PRINT_LOG_ERR(fmt, arg...) HDF_LOGE_WRAPPER("[HDF]-[REGULATOR]:" fmt "\r\n",  ##arg)
#endif

enum RegulatorTestCmd {
    REGULATOR_ENABLE_TEST,
    REGULATOR_DISABLE_TEST,
    REGULATOR_FORCE_DISABLE_TEST,
    REGULATOR_SET_VOLTAGE_TEST,
    REGULATOR_GET_VOLTAGE_TEST,
    REGULATOR_SET_CURRENT_TEST,
    REGULATOR_GET_CURRENT_TEST,
    REGULATOR_GET_STATUS_TEST,
    REGULATOR_MULTI_THREAD_TEST,
    REGULATOR_RELIABILITY_TEST,
};

#define REGULATOR_TEST_STACK_SIZE    (1024 * 100)
#define REGULATOR_TEST_WAIT_TIMES    200
#define REGULATOR_TEST_WAIT_TIMEOUT  20

struct RegulatorTest {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    int32_t (*TestEntry)(struct RegulatorTest *test, int32_t cmd);
    DevHandle handle;
    const char *name;
    uint32_t maxUv;
    uint32_t minUv;
    uint32_t maxUa;
    uint32_t minUa;
    uint32_t uv;
    uint32_t ua;
    uint32_t status;
    uint8_t mode;
};

static inline struct RegulatorTest *GetRegulatorTest(void)
{
    return (struct RegulatorTest *)DevSvcManagerClntGetService("REGULATOR_TEST");
}

#endif /* REGULATOR_TEST_H */
