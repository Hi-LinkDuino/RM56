/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <gtest/gtest.h>
#include "hdf_uhdf_test.h"

using namespace testing::ext;

/**
 * @brief Mipi csi test case number.
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
    MIPI_CSI_TEST_MAX = 12
};

class HdfLiteMipiCsiTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfLiteMipiCsiTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void HdfLiteMipiCsiTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void HdfLiteMipiCsiTest::SetUp()
{
}

void HdfLiteMipiCsiTest::TearDown()
{
}

/**
 * @tc.name: MipiCsiSetHsModeTest001
 * @tc.desc: mipi csi function test
 * @tc.type: FUNC
 * @tc.require: NA
 */
HWTEST_F(HdfLiteMipiCsiTest, MipiCsiSetHsModeTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_MIPI_CSI_TYPE, MIPI_CSI_TEST_SET_HS_MODE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: MipiCsiDisableMipiClockTest001
 * @tc.desc: mipi csi function test
 * @tc.type: FUNC
 * @tc.require: NA
 */
HWTEST_F(HdfLiteMipiCsiTest, MipiCsiDisableMipiClockTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_MIPI_CSI_TYPE, MIPI_CSI_TEST_DISABLE_MIPI_CLOCK, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: MipiCsiEnableMipiClockTest001
 * @tc.desc: mipi csi function test
 * @tc.type: FUNC
 * @tc.require: NA
 */
HWTEST_F(HdfLiteMipiCsiTest, MipiCsiEnableMipiClockTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_MIPI_CSI_TYPE, MIPI_CSI_TEST_ENABLE_MIPI_CLOCK, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: MipiCsiResetRxTest001
 * @tc.desc: mipi csi function test
 * @tc.type: FUNC
 * @tc.require: NA
 */
HWTEST_F(HdfLiteMipiCsiTest, MipiCsiResetRxTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_MIPI_CSI_TYPE, MIPI_CSI_TEST_RESET_RX, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: MipiCsiDisableSensorClockTest001
 * @tc.desc: mipi csi function test
 * @tc.type: FUNC
 * @tc.require: NA
 */
HWTEST_F(HdfLiteMipiCsiTest, MipiCsiDisableSensorClockTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_MIPI_CSI_TYPE, MIPI_CSI_TEST_DISABLE_SENSOR_CLOCK, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: MipiCsiEnableSensorClockTest001
 * @tc.desc: mipi csi function test
 * @tc.type: FUNC
 * @tc.require: NA
 */
HWTEST_F(HdfLiteMipiCsiTest, MipiCsiEnableSensorClockTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_MIPI_CSI_TYPE, MIPI_CSI_TEST_ENABLE_SENSOR_CLOCK, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: MipiCsiResetSensorTest001
 * @tc.desc: mipi csi function test
 * @tc.type: FUNC
 * @tc.require: NA
 */
HWTEST_F(HdfLiteMipiCsiTest, MipiCsiResetSensorTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_MIPI_CSI_TYPE, MIPI_CSI_TEST_RESET_SENSOR, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: MipiCsiSetComboDevAttrTest001
 * @tc.desc: mipi csi function test
 * @tc.type: FUNC
 * @tc.require: NA
 */
HWTEST_F(HdfLiteMipiCsiTest, MipiCsiSetComboDevAttrTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_MIPI_CSI_TYPE, MIPI_CSI_TEST_SET_DEV_ATTR, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: MipiCsiSetPhyCmvmodeTest001
 * @tc.desc: mipi csi function test
 * @tc.type: FUNC
 * @tc.require: NA
 */
HWTEST_F(HdfLiteMipiCsiTest, MipiCsiSetPhyCmvmodeTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_MIPI_CSI_TYPE, MIPI_CSI_TEST_SET_PHY_CMVMODE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: MipiCsiSetExtDataTypeTest001
 * @tc.desc: mipi csi function test
 * @tc.type: FUNC
 * @tc.require: NA
 */
HWTEST_F(HdfLiteMipiCsiTest, MipiCsiSetExtDataTypeTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_MIPI_CSI_TYPE, MIPI_CSI_TEST_SET_EXT_DATA_TYPE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: MipiCsiUnresetRxTest001
 * @tc.desc: mipi csi function test
 * @tc.type: FUNC
 * @tc.require: NA
 */
HWTEST_F(HdfLiteMipiCsiTest, MipiCsiUnresetRxTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_MIPI_CSI_TYPE, MIPI_CSI_TEST_UNRESET_RX, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: MipiCsiUnresetSensorTest001
 * @tc.desc: mipi csi function test
 * @tc.type: FUNC
 * @tc.require: NA
 */
HWTEST_F(HdfLiteMipiCsiTest, MipiCsiUnresetSensorTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_MIPI_CSI_TYPE, MIPI_CSI_TEST_UNRESET_SENSOR, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
