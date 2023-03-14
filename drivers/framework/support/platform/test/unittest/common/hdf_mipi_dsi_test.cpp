/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_uhdf_test.h"
#include <gtest/gtest.h>

using namespace testing::ext;

// pal mipi dsi test case number
enum MipiDsiTestCmd {
    MIPI_DSI_TEST_SET_CFG = 0,
    MIPI_DSI_TEST_GET_CFG = 1,
    MIPI_DSI_TEST_TX_RX = 2,
    MIPI_DSI_TEST_TO_LP_TO_HS = 3,
    MIPI_DSI_TEST_ENTER_ULPS_EXIT_ULPS = 4,
    MIPI_DSI_TEST_POWER_CONTROL = 5,
    MIPI_DSI_TEST_MAX = 6,
};

class HdfLiteMipiDsiTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfLiteMipiDsiTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void HdfLiteMipiDsiTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void HdfLiteMipiDsiTest::SetUp()
{
}

void HdfLiteMipiDsiTest::TearDown()
{
}

static void MipiDsiTest(enum MipiDsiTestCmd cmd)
{
    struct HdfTestMsg msg = {TEST_PAL_MIPI_DSI_TYPE, (uint8_t)cmd, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: MipiDsiSetCfgTest001
  * @tc.desc: mipi dsi function test
  * @tc.type: FUNC
  * @tc.require: AR000F868F
  */
HWTEST_F(HdfLiteMipiDsiTest, MipiDsiSetCfgTest001, TestSize.Level1)
{
    MipiDsiTest(MIPI_DSI_TEST_SET_CFG);
}

/**
  * @tc.name: MipiDsiGetCfgTest001
  * @tc.desc: mipi dsi function test
  * @tc.type: FUNC
  * @tc.require: AR000F868F
  */
HWTEST_F(HdfLiteMipiDsiTest, MipiDsiGetCfgTest001, TestSize.Level1)
{
    MipiDsiTest(MIPI_DSI_TEST_GET_CFG);
}

/**
  * @tc.name: MipiDsiTxRxTest001
  * @tc.desc: mipi dsi function test
  * @tc.type: FUNC
  * @tc.require: AR000F868F
  */
HWTEST_F(HdfLiteMipiDsiTest, MipiDsiTxRxTest001, TestSize.Level1)
{
    MipiDsiTest(MIPI_DSI_TEST_TX_RX);
}

/**
  * @tc.name: MipiDsiLpHsTest001
  * @tc.desc: mipi dsi function test
  * @tc.type: FUNC
  * @tc.require: AR000F868F
  */
HWTEST_F(HdfLiteMipiDsiTest, MipiDsiLpHsTest001, TestSize.Level1)
{
    MipiDsiTest(MIPI_DSI_TEST_TO_LP_TO_HS);
}

