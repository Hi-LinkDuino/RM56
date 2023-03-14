/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef EMMC_TEST_H
#define EMMC_TEST_H

#include "hdf_device_desc.h"
#include "platform_if.h"

enum EmmcTestCmd {
    EMMC_GET_CID_01 = 0,
};

struct EmmcTester {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    int32_t (*TestEntry)(struct EmmcTester *tester, int32_t cmd);
    uint32_t busNum;
    uint32_t hostId;
    DevHandle handle;
};

static inline struct EmmcTester *GetEmmcTester(void)
{
    return (struct EmmcTester *)DevSvcManagerClntGetService("EMMC_TEST");
}

#endif /* EMMC_TEST_H */
