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
#include "platform_device_test.h"

using namespace testing::ext;

class HdfPlatformDeviceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfPlatformDeviceTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void HdfPlatformDeviceTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void HdfPlatformDeviceTest::SetUp()
{
}

void HdfPlatformDeviceTest::TearDown()
{
}

/**
  * @tc.name: HdfPlatformDeviceTestSetName001
  * @tc.desc: platform device function test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfPlatformDeviceTest, HdfPlatformDeviceTestSetName001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_DEVICE_TYPE, PLAT_DEVICE_TEST_SET_NAME, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdfPlatformDeviceTestGetDevice001
  * @tc.desc: platform device function test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfPlatformDeviceTest, HdfPlatformDeviceTestGetDevice001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_DEVICE_TYPE, PLAT_DEVICE_TEST_GET_DEVICE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdfPlatformDeviceTestWaitEvent001
  * @tc.desc: platform device function test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfPlatformDeviceTest, HdfPlatformDeviceTestWaitEvent001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_DEVICE_TYPE, PLAT_DEVICE_TEST_WAIT_EVENT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdfPlatformDeviceTestAddDevice001
  * @tc.desc: platform device function test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfPlatformDeviceTest, HdfPlatformDeviceTestAddDevice001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_DEVICE_TYPE, PLAT_DEVICE_TEST_ADD_DEVICE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdfPlatformDeviceTestCreateService001
  * @tc.desc: platform device function test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfPlatformDeviceTest, HdfPlatformDeviceTestCreateService001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_DEVICE_TYPE, PLAT_DEVICE_TEST_CREATE_SERVICE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdfPlatformDeviceTestBindDevice001
  * @tc.desc: platform device function test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfPlatformDeviceTest, HdfPlatformDeviceTestBindDevice001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_DEVICE_TYPE, PLAT_DEVICE_TEST_BIND_DEVICE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdfPlatformDeviceTestReliability001
  * @tc.desc: platform device function test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfPlatformDeviceTest, HdfPlatformDeviceTestReliability001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_DEVICE_TYPE, PLAT_DEVICE_TEST_RELIABILITY, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
