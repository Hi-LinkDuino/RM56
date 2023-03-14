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
#include "rtc_test.h"

using namespace testing::ext;

class HdfRtcTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfRtcTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void HdfRtcTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void HdfRtcTest::SetUp()
{
}

void HdfRtcTest::TearDown()
{
}

/**
  * @tc.name: testRtcReadWriteTime001
  * @tc.desc: rtc function test
  * @tc.type: FUNC
  * @tc.require: AR000F868I
  */
HWTEST_F(HdfRtcTest, testRtcReadWriteTime001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_RTC_TYPE, RTC_TEST_CMD_WR_TIME, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, RtcTestExecute(RTC_TEST_CMD_WR_TIME));
}

/**
  * @tc.name: testRtcReadWriteMaxTime001
  * @tc.desc: rtc function test
  * @tc.type: FUNC
  * @tc.require: AR000F868I
  */
HWTEST_F(HdfRtcTest, testRtcReadWriteMaxTime001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_RTC_TYPE, RTC_TEST_CMD_WR_MAX_TIME, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, RtcTestExecute(RTC_TEST_CMD_WR_MAX_TIME));
}

/**
  * @tc.name: testRtcReadWriteMinTime001
  * @tc.desc: rtc function test
  * @tc.type: FUNC
  * @tc.require: AR000F868I
  */
HWTEST_F(HdfRtcTest, testRtcReadWriteMinTime001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_RTC_TYPE, RTC_TEST_CMD_WR_MIN_TIME, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, RtcTestExecute(RTC_TEST_CMD_WR_MIN_TIME));
}

/**
  * @tc.name: testRtcReadWriteAlarmTime001
  * @tc.desc: rtc function test
  * @tc.type: FUNC
  * @tc.require: AR000F868I
  */
HWTEST_F(HdfRtcTest, testRtcReadWriteAlarmTime001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_RTC_TYPE, RTC_TEST_CMD_WR_ALARM_TIME, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, RtcTestExecute(RTC_TEST_CMD_WR_ALARM_TIME));
}

/**
  * @tc.name: testRtcReadWriteAlarmMaxTime001
  * @tc.desc: rtc function test
  * @tc.type: FUNC
  * @tc.require: AR000F868I
  */
HWTEST_F(HdfRtcTest, testRtcReadWriteAlarmMaxTime001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_RTC_TYPE, RTC_TEST_CMD_WR_ALARM_MAX_TIME, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, RtcTestExecute(RTC_TEST_CMD_WR_ALARM_MAX_TIME));
}
/**
  * @tc.name: testRtcReadWriteAlarmMinTime001
  * @tc.desc: rtc function test
  * @tc.type: FUNC
  * @tc.require: AR000F868I
  */
HWTEST_F(HdfRtcTest, testRtcReadWriteAlarmMinTime001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_RTC_TYPE, RTC_TEST_CMD_WR_ALARM_MIN_TIME, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, RtcTestExecute(RTC_TEST_CMD_WR_ALARM_MIN_TIME));
}

#ifndef HDF_LITEOS_TEST
/**
  * @tc.name: testRtcAlarmEnable001
  * @tc.desc: rtc function test
  * @tc.type: FUNC
  * @tc.require: AR000F868I
  */
HWTEST_F(HdfRtcTest, testRtcAlarmEnable001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_RTC_TYPE, RTC_TEST_CMD_RTC_ALARM_ENABLE, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, RtcTestExecute(RTC_TEST_CMD_RTC_ALARM_ENABLE));
}
#endif

#if defined(HDF_LITEOS_TEST)

/**
  * @tc.name: testRtcAlarmIrq001
  * @tc.desc: rtc function test
  * @tc.type: FUNC
  * @tc.require: AR000EKRKU
  */
HWTEST_F(HdfRtcTest, testRtcAlarmIrq001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_RTC_TYPE, RTC_TEST_CMD_RTC_ALARM_IRQ, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, RtcTestExecute(RTC_TEST_CMD_RTC_ALARM_IRQ));
}

/**
  * @tc.name: testRtcRegCallback001
  * @tc.desc: rtc function test
  * @tc.type: FUNC
  * @tc.require: AR000EKRKU
  */
HWTEST_F(HdfRtcTest, testRtcRegCallback001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_RTC_TYPE, RTC_TEST_CMD_RTC_REGISTER_CALLBACK, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, RtcTestExecute(RTC_TEST_CMD_RTC_REGISTER_CALLBACK));
}

/**
  * @tc.name: testRtcRegCallbackNull001
  * @tc.desc: rtc function test
  * @tc.type: FUNC
  * @tc.require: AR000EKRKU
  */
HWTEST_F(HdfRtcTest, testRtcRegCallbackNull001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_RTC_TYPE, RTC_TEST_CMD_RTC_REGISTER_CALLBACK_NULL, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, RtcTestExecute(RTC_TEST_CMD_RTC_REGISTER_CALLBACK_NULL));
}

/**
  * @tc.name: testRtcFreq001
  * @tc.desc: rtc function test
  * @tc.type: FUNC
  * @tc.require: AR000EKRKU
  */
HWTEST_F(HdfRtcTest, testRtcFreq001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_RTC_TYPE, RTC_TEST_CMD_RTC_WR_FREQ, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, RtcTestExecute(RTC_TEST_CMD_RTC_WR_FREQ));
}

/**
  * @tc.name: testRtcMaxFreq001
  * @tc.desc: rtc function test
  * @tc.type: FUNC
  * @tc.require: AR000EKRKU
  */
HWTEST_F(HdfRtcTest, testRtcMaxFreq001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_RTC_TYPE, RTC_TEST_CMD_RTC_WR_MAX_FREQ, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, RtcTestExecute(RTC_TEST_CMD_RTC_WR_MAX_FREQ));
}

/**
  * @tc.name: testRtcMinFreq001
  * @tc.desc: rtc function test
  * @tc.type: FUNC
  * @tc.require: AR000EKRKU
  */
HWTEST_F(HdfRtcTest, testRtcMinFreq001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_RTC_TYPE, RTC_TEST_CMD_RTC_WR_MIN_FREQ, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, RtcTestExecute(RTC_TEST_CMD_RTC_WR_MIN_FREQ));
}

/**
  * @tc.name: testRtcUserReg001
  * @tc.desc: rtc function test
  * @tc.type: FUNC
  * @tc.require: AR000EKRKU
  */
HWTEST_F(HdfRtcTest, testRtcUserReg001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_RTC_TYPE, RTC_TEST_CMD_RTC_WR_USER_REG, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, RtcTestExecute(RTC_TEST_CMD_RTC_WR_USER_REG));
}

/**
  * @tc.name: testRtcUserRegMaxIndex001
  * @tc.desc: rtc function test
  * @tc.type: FUNC
  * @tc.require: AR000EKRKU
  */
HWTEST_F(HdfRtcTest, testRtcUserRegMaxIndex001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_RTC_TYPE, RTC_TEST_CMD_RTC_WR_USER_REG_MAX_INDEX, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, RtcTestExecute(RTC_TEST_CMD_RTC_WR_USER_REG_MAX_INDEX));
}
#endif
/**
  * @tc.name: testRtcReliability001
  * @tc.desc: rtc function test
  * @tc.type: FUNC
  * @tc.require: AR000EKRKU
  */
HWTEST_F(HdfRtcTest, testRtcReliability001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_RTC_TYPE, RTC_TEST_CMD_RTC_WR_RELIABILITY, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, RtcTestExecute(RTC_TEST_CMD_RTC_WR_RELIABILITY));
}

#if defined(HDF_LITEOS_TEST)
/**
  * @tc.name: testRtcModule001
  * @tc.desc: rtc function test
  * @tc.type: FUNC
  * @tc.require: AR000EKRKU
  */
HWTEST_F(HdfRtcTest, testRtcModule001, TestSize.Level1)
{
    struct HdfTestMsg msg = { TEST_PAL_RTC_TYPE, RTC_TEST_CMD_RTC_FUNCTION_TEST, -1 };
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));

    EXPECT_EQ(0, RtcTestExecute(RTC_TEST_CMD_RTC_FUNCTION_TEST));
}
#endif