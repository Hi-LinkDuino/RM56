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

class HdfLiteRegulatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfLiteRegulatorTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void HdfLiteRegulatorTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void HdfLiteRegulatorTest::SetUp()
{
}

void HdfLiteRegulatorTest::TearDown()
{
}
/**
  * @tc.name: RegulatorTestEnable001
  * @tc.desc: regulator enable test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteRegulatorTest, RegulatorTestEnable001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_REGULATOR_TYPE, REGULATOR_ENABLE_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
/**
  * @tc.name: RegulatorTestDisable001
  * @tc.desc: regulator disable test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteRegulatorTest, RegulatorTestDisable001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_REGULATOR_TYPE, REGULATOR_DISABLE_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
/**
  * @tc.name: RegulatorTestForceDisable001
  * @tc.desc: regulator force disable test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteRegulatorTest, RegulatorTestForceDisable001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_REGULATOR_TYPE, REGULATOR_FORCE_DISABLE_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
/**
  * @tc.name: RegulatorTestSetVoltage001
  * @tc.desc: regulator set voltage test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteRegulatorTest, RegulatorTestSetVoltage001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_REGULATOR_TYPE, REGULATOR_SET_VOLTAGE_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
/**
  * @tc.name: RegulatorTestGetVoltage001
  * @tc.desc: regulator get voltage test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteRegulatorTest, RegulatorTestGetVoltage001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_REGULATOR_TYPE, REGULATOR_GET_VOLTAGE_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
/**
  * @tc.name: RegulatorTestSetCurrent001
  * @tc.desc: regulator set current test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteRegulatorTest, RegulatorTestSetCurrent001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_REGULATOR_TYPE, REGULATOR_SET_CURRENT_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
/**
  * @tc.name: RegulatorTestGetCurrent001
  * @tc.desc: regulator get current test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteRegulatorTest, RegulatorTestGetCurrent001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_REGULATOR_TYPE, REGULATOR_GET_CURRENT_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
/**
  * @tc.name: RegulatorTestGetCurrent001
  * @tc.desc: regulator get status test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteRegulatorTest, RegulatorTestGetStatus001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_REGULATOR_TYPE, REGULATOR_GET_STATUS_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
/**
  * @tc.name: RegulatorTestMultiThread001
  * @tc.desc: regulator multi thread test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteRegulatorTest, RegulatorTestMultiThread001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_REGULATOR_TYPE, REGULATOR_MULTI_THREAD_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
/**
  * @tc.name: RegulatorTestReliability001
  * @tc.desc: regulator Reliability test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteRegulatorTest, RegulatorTestReliability001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_REGULATOR_TYPE, REGULATOR_RELIABILITY_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
