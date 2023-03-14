/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MIPI_CSI_TEST_H
#define MIPI_CSI_TEST_H

#include "hdf_device_desc.h"
#include "mipi_csi_if.h"

/**
 * @brief Pal mipi csi test case number.
 *
 * The sequence number is related to the test logic, which is different from the actual call.
 * 1, 4, 8 and 9 need to be called according to the actual situation.
 *
 * @since 1.0
 */
enum MipiCsiTestCmd {
    MIPI_CSI_TEST_SET_HS_MODE = 0,
    MIPI_CSI_TEST_DISABLE_MIPI_CLOCK = 1, /* for test */
    MIPI_CSI_TEST_ENABLE_MIPI_CLOCK = 2,
    MIPI_CSI_TEST_RESET_RX = 3,
    MIPI_CSI_TEST_DISABLE_SENSOR_CLOCK = 4, /* for test */
    MIPI_CSI_TEST_ENABLE_SENSOR_CLOCK = 5,
    MIPI_CSI_TEST_RESET_SENSOR = 6,
    MIPI_CSI_TEST_SET_DEV_ATTR = 7,
    MIPI_CSI_TEST_SET_PHY_CMVMODE = 8, /* for test */
    MIPI_CSI_TEST_SET_EXT_DATA_TYPE = 9, /* for test */
    MIPI_CSI_TEST_UNRESET_RX = 10,
    MIPI_CSI_TEST_UNRESET_SENSOR = 11,
    MIPI_CSI_TEST_MAX = 12,
};

struct MipiCsiTest {
    struct IDeviceIoService service;
    struct HdfDeviceObject *device;
    int32_t (*doTest)(struct MipiCsiTest *test, uint8_t cmd);
    DevHandle handle;
    uint32_t total;
    uint32_t fails;

    uint8_t cntlrId;
    ComboDevAttr attr;
    uint8_t devno;
    PhyCmvMode cmvMode;
    ExtDataType dataType;
    LaneDivideMode laneDivideMode;
    uint8_t comboDev;
    uint8_t snsClkSource;
    uint8_t snsResetSource;
};

static inline struct MipiCsiTest *MipiCsiTestServiceGet(void)
{
    return (struct MipiCsiTest *)DevSvcManagerClntGetService("PLATFORM_MIPI_CSI_TEST");
}
#endif /* MIPI_CSI_TEST_H */
