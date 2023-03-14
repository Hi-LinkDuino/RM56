
/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "timer_test.h"
#include "hdf_io_service_if.h"

using namespace testing::ext;

class HdfLiteTimerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfLiteTimerTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void HdfLiteTimerTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void HdfLiteTimerTest::SetUp()
{
}

void HdfLiteTimerTest::TearDown()
{
}
/**
  * @tc.name: TimerTestSet001
  * @tc.desc: timer set test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteTimerTest, TimerTestSet001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_TIMER_TYPE, TIMER_TEST_SET, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    EXPECT_EQ(0, TimerTestExecute(TIMER_TEST_SET));
}

/**
  * @tc.name: TimerTestSetOnce001
  * @tc.desc: timer set once test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteTimerTest, TimerTestSetOnce001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_TIMER_TYPE, TIMER_TEST_SETONCE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    EXPECT_EQ(0, TimerTestExecute(TIMER_TEST_SETONCE));
}

/**
  * @tc.name: TimerTestGet001
  * @tc.desc: timer get test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteTimerTest, TimerTestGet001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_TIMER_TYPE, TIMER_TEST_GET, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    EXPECT_EQ(0, TimerTestExecute(TIMER_TEST_GET));
}

/**
  * @tc.name: TimerTestStart001
  * @tc.desc: timer start test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteTimerTest, TimerTestStart001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_TIMER_TYPE, TIMER_TEST_START, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    EXPECT_EQ(0, TimerTestExecute(TIMER_TEST_START));
}

/**
  * @tc.name: TimerTestStop001
  * @tc.desc: timer stop test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteTimerTest, TimerTestStop001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_TIMER_TYPE, TIMER_TEST_STOP, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    EXPECT_EQ(0, TimerTestExecute(TIMER_TEST_STOP));
}

/**
  * @tc.name: TimerTestMultiThread001
  * @tc.desc: timer multi thread test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteTimerTest, TimerTestMultiThread001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_TIMER_TYPE, TIMER_MULTI_THREAD_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    EXPECT_EQ(0, TimerTestExecute(TIMER_MULTI_THREAD_TEST));
}

/**
  * @tc.name: TimerTestReliability001
  * @tc.desc: timer reliability test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteTimerTest, TimerTestReliability001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_TIMER_TYPE, TIMER_RELIABILITY_TEST, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
    EXPECT_EQ(0, TimerTestExecute(TIMER_RELIABILITY_TEST));
}

/**
  * @tc.name: TimerTestIfPerformance001
  * @tc.desc: timer user if performance test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfLiteTimerTest, TimerTestIfPerformance001, TestSize.Level1)
{
    EXPECT_EQ(0, TimerTestExecute(TIMER_IF_PERFORMANCE_TEST));
}
