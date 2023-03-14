/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <string>
#include <unistd.h>
#include <gtest/gtest.h>
#include "hdf_uhdf_test.h"
#include "hdf_io_service_if.h"
#include "watchdog_test.h"

using namespace testing::ext;

class HdfLiteWatchdogTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfLiteWatchdogTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void HdfLiteWatchdogTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void HdfLiteWatchdogTest::SetUp()
{
}

void HdfLiteWatchdogTest::TearDown()
{
}

/**
  * @tc.name: HdfLiteWatchdogTestSetGetTimeout001
  * @tc.desc: watchdog function test
  * @tc.type: FUNC
  * @tc.require: AR000F868G
  */
HWTEST_F(HdfLiteWatchdogTest, HdfLiteWatchdogTestSetGetTimeout001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_WDT_TYPE, WATCHDOG_TEST_SET_GET_TIMEOUT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    EXPECT_EQ(0, WatchdogTestExecute(WATCHDOG_TEST_SET_GET_TIMEOUT));
}

/**
  * @tc.name: HdfLiteWatchdogTestStartStop001
  * @tc.desc: watchdog function test
  * @tc.type: FUNC
  * @tc.require: AR000F868G
  */
HWTEST_F(HdfLiteWatchdogTest, HdfLiteWatchdogTestStartStop001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_WDT_TYPE, WATCHDOG_TEST_START_STOP, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    EXPECT_EQ(0, WatchdogTestExecute(WATCHDOG_TEST_START_STOP));
}

/**
  * @tc.name: HdfLiteWatchdogTestFeed001
  * @tc.desc: watchdog function test
  * @tc.type: FUNC
  * @tc.require: AR000F868G
  */
HWTEST_F(HdfLiteWatchdogTest, HdfLiteWatchdogTestFeed001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_WDT_TYPE, WATCHDOG_TEST_FEED, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    EXPECT_EQ(0, WatchdogTestExecute(WATCHDOG_TEST_FEED));
}

/**
  * @tc.name: HdfLiteWatchdogTestReliability001
  * @tc.desc: watchdog function test
  * @tc.type: FUNC
  * @tc.require: AR000F868G
  */
HWTEST_F(HdfLiteWatchdogTest, HdfLiteWatchdogTestReliability001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_WDT_TYPE, WATCHDOG_TEST_RELIABILITY, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    EXPECT_EQ(0, WatchdogTestExecute(WATCHDOG_TEST_RELIABILITY));
}

/**
  * @tc.name: HdfLiteWatchdogTestIfPerformance001
  * @tc.desc: watchdog user if performance test
  * @tc.type: FUNC
  * @tc.require:
  */
HWTEST_F(HdfLiteWatchdogTest, HdfLiteWatchdogTestIfPerformance001, TestSize.Level1)
{
    EXPECT_EQ(0, WatchdogTestExecute(WATCHDOG_IF_PERFORMANCE_TEST));
}
