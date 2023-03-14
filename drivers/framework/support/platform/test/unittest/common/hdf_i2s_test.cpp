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
#include "hdf_uhdf_test.h"
#include "hdf_io_service_if.h"

using namespace testing::ext;

enum HdfLiteI2sTestCmd {
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

class HdfLiteI2sTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfLiteI2sTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void HdfLiteI2sTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void HdfLiteI2sTest::SetUp()
{
}

void HdfLiteI2sTest::TearDown()
{
}

/**
 * @tc.name: I2sSetCfgTest001
 * @tc.desc: i2s function test
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(HdfLiteI2sTest, I2sSetCfgTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_I2S_TYPE, I2S_SET_CFG_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: I2sReadStartTest001
  * @tc.desc: i2s function test
  * @tc.type: FUNC
  * @tc.requir: 
  */
HWTEST_F(HdfLiteI2sTest, I2sReadStartTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_I2S_TYPE, I2S_READ_START_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: I2sReadTest001
  * @tc.desc: i2s function test
  * @tc.type: FUNC
  * @tc.require: 
  */
HWTEST_F(HdfLiteI2sTest, I2sReadTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_I2S_TYPE, I2S_READ_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: I2sReadStopTest001
  * @tc.desc: i2s function test
  * @tc.type: FUNC
  * @tc.requir: 
  */
HWTEST_F(HdfLiteI2sTest, I2sReadStopTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_I2S_TYPE, I2S_READ_STOP_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: I2sWriteStartTest001
  * @tc.desc: i2s function test
  * @tc.type: FUNC
  * @tc.requir: 
  */
HWTEST_F(HdfLiteI2sTest, I2sWriteStartTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_I2S_TYPE, I2S_WRITE_START_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}


/**
  * @tc.name: I2sWriteTest001
  * @tc.desc: i2s function test
  * @tc.type: FUNC
  * @tc.require: 
  */
HWTEST_F(HdfLiteI2sTest, I2sWriteTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_I2S_TYPE, I2S_WRITE_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}


/**
  * @tc.name: I2sWriteStopTest001
  * @tc.desc: i2s function test
  * @tc.type: FUNC
  * @tc.require: 
  */
HWTEST_F(HdfLiteI2sTest, I2sWriteStopTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_I2S_TYPE, I2S_WRITE_STOP_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: I2sGetCfgTest001
 * @tc.desc: i2s function test
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(HdfLiteI2sTest, I2sGetCfgTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_I2S_TYPE, I2S_GET_CFG_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: I2sReliabilityTest001
 * @tc.desc: i2s function test
 * @tc.type: FUNC
 * @tc.require: 
 */
HWTEST_F(HdfLiteI2sTest, I2sReliabilityTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_I2S_TYPE, I2S_RELIABILITY_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
