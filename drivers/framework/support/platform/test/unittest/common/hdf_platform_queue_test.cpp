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
#include "platform_queue_test.h"

using namespace testing::ext;

class HdfPlatformQueueTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfPlatformQueueTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void HdfPlatformQueueTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void HdfPlatformQueueTest::SetUp()
{
}

void HdfPlatformQueueTest::TearDown()
{
}

/**
  * @tc.name: HdfPlatformQueueTestAddAndWait001
  * @tc.desc: platform queue function test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfPlatformQueueTest, HdfPlatformQueueTestAddAndWait001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_QUEUE_TYPE, PLAT_QUEUE_TEST_ADD_AND_WAIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdfPlatformQueueTestReliability001
  * @tc.desc: platform queue function test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfPlatformQueueTest, HdfPlatformQueueTestReliability001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_QUEUE_TYPE, PLAT_QUEUE_TEST_RELIABILITY, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
