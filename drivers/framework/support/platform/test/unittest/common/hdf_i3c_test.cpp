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
#include "hdf_io_service_if.h"
#include "hdf_uhdf_test.h"
#include "i3c_test.h"

using namespace testing::ext;

class HdfI3cTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfI3cTest::SetUpTestCase()
{
    struct HdfTestMsg msg = {TEST_PAL_I3C_TYPE, I3C_TEST_CMD_SETUP_ALL, -1};
    HdfTestOpenService();
    HdfTestSendMsgToService(&msg);
}

void HdfI3cTest::TearDownTestCase()
{
    struct HdfTestMsg msg = {TEST_PAL_I3C_TYPE, I3C_TEST_CMD_TEARDOWN_ALL, -1};
    HdfTestSendMsgToService(&msg);
    HdfTestCloseService();
}

void HdfI3cTest::SetUp()
{
}

void HdfI3cTest::TearDown()
{
}

/**
  * @tc.name: HdfI3cTestTransfer001
  * @tc.desc: i3c transfer test
  * @tc.type: FUNC
  * @tc.require: N/A
  */
HWTEST_F(HdfI3cTest, HdfI3cTestTransfer001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_I3C_TYPE, I3C_TEST_CMD_TRANSFER, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdfI3cTestSetConfig001
  * @tc.desc: i3c set config test
  * @tc.type: FUNC
  * @tc.require: N/A
  */
HWTEST_F(HdfI3cTest, HdfI3cTestSetConfig001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_I3C_TYPE, I3C_TEST_CMD_SET_CONFIG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdfI3cTestGetConfig001
  * @tc.desc: i3c get config test
  * @tc.type: FUNC
  * @tc.require: N/A
  */
HWTEST_F(HdfI3cTest, HdfI3cTestGetConfig001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_I3C_TYPE, I3C_TEST_CMD_GET_CONFIG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdfI3cTestRequestIbi001
  * @tc.desc: i3c request ibi test
  * @tc.type: FUNC
  * @tc.require: N/A
  */
HWTEST_F(HdfI3cTest, HdfI3cTestRequestIbi001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_I3C_TYPE, I3C_TEST_CMD_REQUEST_IBI, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdfI3cTestRreeIbi001
  * @tc.desc: i3c free ibi test
  * @tc.type: FUNC
  * @tc.require: N/A
  */
HWTEST_F(HdfI3cTest, HdfI3cTestRreeIbi001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_I3C_TYPE, I3C_TEST_CMD_FREE_IBI, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdfI3cTestMultiThread001
  * @tc.desc: i3c multithreading test
  * @tc.type: FUNC
  * @tc.require: N/A
  */
HWTEST_F(HdfI3cTest, HdfI3cTestMultiThread001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_I3C_TYPE, I3C_TEST_CMD_MULTI_THREAD, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: HdfI3cTestReliability001
  * @tc.desc: i3c reliability test
  * @tc.type: FUNC
  * @tc.require: N/A
  */
HWTEST_F(HdfI3cTest, HdfI3cTestReliability001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_I3C_TYPE, I3C_TEST_CMD_RELIABILITY, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
