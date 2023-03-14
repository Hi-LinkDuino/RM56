/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SDIO_TEST_H
#define SDIO_TEST_H

#include "hdf_device_desc.h"
#include "platform_if.h"
#include "sdio_if.h"

enum SdioTestCmd {
    SDIO_DISABLE_FUNC_01 = 0,
    SDIO_ENABLE_FUNC_01,
    SDIO_SET_BLOCK_SIZE_01,
    SDIO_INCR_ADDR_READ_AND_WRITE_BYTES_01,
    SDIO_FIXED_ADDR_READ_AND_WRITE_BYTES_01,
    SDIO_FUNC0_READ_AND_WRITE_BYTES_01,
    SDIO_SET_AND_GET_COMMON_INFO_01,
};

struct SdioTester {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    int32_t (*TestEntry)(struct SdioTester *tester, int32_t cmd);
    struct SdioFunctionConfig config;
    uint32_t busNum;
    DevHandle handle;
};

static inline struct SdioTester *GetSdioTest(void)
{
    return (struct SdioTester *)DevSvcManagerClntGetService("SDIO_TEST");
}

#endif /* SDIO_TEST_H */
