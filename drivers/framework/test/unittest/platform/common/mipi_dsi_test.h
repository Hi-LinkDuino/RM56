/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MIPI_DSI_TEST_H
#define MIPI_DSI_TEST_H

#include "hdf_device_desc.h"
#include "mipi_dsi_if.h"

enum MipiDsiTestCmd {
    MIPI_DSI_TEST_SET_CFG = 0,
    MIPI_DSI_TEST_GET_CFG = 1,
    MIPI_DSI_TEST_TX_RX = 2,
    MIPI_DSI_TEST_TO_LP_TO_HS = 3,
    MIPI_DSI_TEST_MAX = 4,
};

struct MipiDsiTest {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    int32_t (*doTest)(struct MipiDsiTest *test, int32_t cmd);
    DevHandle handle;
    struct DsiCmdDesc msgs;
    struct MipiCfg cfg;
    uint8_t devNo;
    uint32_t total;
    uint32_t fails;
};

static inline struct MipiDsiTest *MipiDsiTestServiceGet(void)
{
    return (struct MipiDsiTest *)DevSvcManagerClntGetService("PLATFORM_MIPI_DSI_TEST");
}
#endif /* MIPI_DSI_TEST_H */
