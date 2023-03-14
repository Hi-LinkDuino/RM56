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
#include <string>
#include <unistd.h>
#include <gtest/gtest.h>
#include "hdf_uhdf_test.h"
#include "platform_event_test.h"

using namespace testing::ext;

class HdfPlatformEventTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfPlatformEventTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void HdfPlatformEventTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void HdfPlatformEventTest::SetUp()
{
}

void HdfPlatformEventTest::TearDown()
{
}

/**
  * @tc.name: HdfPlatformEventTestInitAndUninit001
  * @tc.desc: platform event function test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfPlatformEventTest, HdfPlatformEventTestInitAndUninit001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_EVENT_TYPE, PLAT_EVENT_TEST_INIT_AND_UNINIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdfPlatformEventTestPostAndWait001
  * @tc.desc: platform event function test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfPlatformEventTest, HdfPlatformEventTestPostAndWait001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_EVENT_TYPE, PLAT_EVENT_TEST_POST_AND_WAIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdfPlatformEventTestListenAndUnlisten001
  * @tc.desc: platform event function test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfPlatformEventTest, HdfPlatformEventTestListenAndUnlisten001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_EVENT_TYPE, PLAT_EVENT_TEST_LISTEN_AND_UNLISTEN, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdfPlatformEventTestReliability001
  * @tc.desc: platform event function test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfPlatformEventTest, HdfPlatformEventTestReliability001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_EVENT_TYPE, PLAT_EVENT_TEST_RELIABILITY, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

