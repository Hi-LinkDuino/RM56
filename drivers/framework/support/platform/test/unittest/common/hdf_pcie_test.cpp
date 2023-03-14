/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <string>
#include <unistd.h>
#include "hdf_io_service_if.h"
#include "hdf_uhdf_test.h"
#include "pcie_if.h"

using namespace testing::ext;

const uint32_t PCIE_DISABLE_ADDR = 0xB7;
const uint32_t PCIE_UPPER_ADDR = 0x28;
const uint32_t PCIE_CMD_ADDR = 0x04;

enum PcieTestCmd {
    PCIE_READ_AND_WRITE_01 = 0,
};

class HdfPcieTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfPcieTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void HdfPcieTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void HdfPcieTest::SetUp()
{
}

void HdfPcieTest::TearDown()
{
}

static void PcieUserTest(void)
{
    DevHandle handle = NULL;
    int32_t ret;
    uint8_t disable;
    uint32_t upper;
    uint16_t cmd;

    handle = PcieOpen(0);
    if (handle == NULL) {
        printf("PcieOpen fail.");
    }

    ret = PcieRead(handle, PCIE_DISABLE_ADDR, &disable, sizeof(disable));
    if (ret != HDF_SUCCESS) {
        printf("PcieRead failed ret = %d.", ret);
        return;
    }
    printf("disable is %d", disable);
    ret = PcieWrite(handle, PCIE_DISABLE_ADDR, &disable, sizeof(disable));
    if (ret != HDF_SUCCESS) {
        printf("PcieWrite failed ret = %d.", ret);
        return;
    }

    ret = PcieRead(handle, PCIE_UPPER_ADDR, (uint8_t *)&upper, sizeof(upper));
    if (ret != HDF_SUCCESS) {
        printf("PcieRead failed ret = %d.", ret);
        return;
    }
    printf("upper is 0x%x", upper);
    ret = PcieWrite(handle, PCIE_UPPER_ADDR, (uint8_t *)&upper, sizeof(upper));
    if (ret != HDF_SUCCESS) {
        printf("PcieWrite failed ret = %d.", ret);
        return;
    }

    ret = PcieRead(handle, PCIE_CMD_ADDR, (uint8_t *)&cmd, sizeof(cmd));
    if (ret != HDF_SUCCESS) {
        printf("PcieRead failed ret = %d.", ret);
        return;
    }
    printf("cmd is 0x%x", cmd);
    ret = PcieWrite(handle, PCIE_CMD_ADDR, (uint8_t *)&cmd, sizeof(cmd));
    if (ret != HDF_SUCCESS) {
        printf("PcieWrite failed ret = %d.", ret);
        return;
    }
    PcieClose(handle);
}

/**
  * @tc.name: PcieReadAndWrite001
  * @tc.desc: test PcieRead/PcieWrite interface in kernel status.
  * @tc.type: FUNC
  * @tc.require:
  */
HWTEST_F(HdfPcieTest, PcieReadAndWrite001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_PCIE_TYPE, PCIE_READ_AND_WRITE_01, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: PcieUserTest001
  * @tc.desc: test pcie all interface in user status.
  * @tc.type: FUNC
  * @tc.require:
  */
HWTEST_F(HdfPcieTest, PcieUserTest001, TestSize.Level1)
{
    PcieUserTest();
}
