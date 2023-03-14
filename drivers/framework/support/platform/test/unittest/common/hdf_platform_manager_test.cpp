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
#include "platform_manager_test.h"

using namespace testing::ext;

class HdfPlatformManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfPlatformManagerTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void HdfPlatformManagerTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void HdfPlatformManagerTest::SetUp()
{
}

void HdfPlatformManagerTest::TearDown()
{
}

/**
  * @tc.name: HdfPlatformManagerTestAddDevice001
  * @tc.desc: platform manager function test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfPlatformManagerTest, HdfPlatformManagerTestAddDevice001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_MANAGER_TYPE, PLAT_MANAGER_TEST_ADD_DEVICE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdfPlatformManagerTestGetDevice001
  * @tc.desc: platform manager function test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfPlatformManagerTest, HdfPlatformManagerTestGetDevice001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_MANAGER_TYPE, PLAT_MANAGER_TEST_GET_DEVICE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdfPlatformManagerTestReliability001
  * @tc.desc: platform manager function test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfPlatformManagerTest, HdfPlatformManagerTestReliability001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_MANAGER_TYPE, PLAT_MANAGER_TEST_RELIABILITY, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

