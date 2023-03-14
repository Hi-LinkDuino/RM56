/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PCIE_TEST_H
#define PCIE_TEST_H

#include "hdf_device_desc.h"
#include "platform_if.h"

enum PcieTestCmd {
    PCIE_READ_AND_WRITE_01 = 0,
};

struct PcieTester {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    int32_t (*TestEntry)(struct PcieTester *tester, int32_t cmd);
    uint32_t busNum;
    DevHandle handle;
};

static inline struct PcieTester *GetPcieTester(void)
{
    return (struct PcieTester *)DevSvcManagerClntGetService("PCIE_TEST");
}

#endif /* PCIE_TEST_H */
