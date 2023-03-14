/* Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "pin_test.h"

using namespace testing::ext;

class HdfPinTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfPinTest::SetUpTestCase()
{
    struct HdfTestMsg msg = {TEST_PAL_PIN_TYPE, PIN_TEST_CMD_SETUP_ALL, -1};
    HdfTestOpenService();
    HdfTestSendMsgToService(&msg);
}

void HdfPinTest::TearDownTestCase()
{
    struct HdfTestMsg msg = {TEST_PAL_PIN_TYPE, PIN_TEST_CMD_TEARDOWN_ALL, -1};
    HdfTestSendMsgToService(&msg);
    HdfTestCloseService();
}

void HdfPinTest::SetUp()
{
}

void HdfPinTest::TearDown()
{
}

/**
  * @tc.name: PinTestSetGetPinPull001
  * @tc.desc: Pin set get pin Pull test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfPinTest, PinSetGetPull001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_PIN_TYPE, PIN_TEST_CMD_SETGETPULL, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: PinTestSetGetStrength001
  * @tc.desc: Pin set get pin Strength test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfPinTest, PinSetGetStrength001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_PIN_TYPE, PIN_TEST_CMD_SETGETSTRENGTH, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
  * @tc.name: PinSetGetPinFunc001
  * @tc.desc: Pin set get pin func test
  * @tc.type: FUNC
  * @tc.require: NA
  */
HWTEST_F(HdfPinTest, PinSetGetPinFunc001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_PIN_TYPE, PIN_TEST_CMD_SETGETFUNC, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

/**
 * @tc.name: PinReliabilityTest001
 * @tc.desc: Pin function test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HdfPinTest, PinReliabilityTest001, TestSize.Level1)
{
    struct HdfTestMsg msg = {TEST_PAL_PIN_TYPE, PIN_TEST_CMD_RELIABILITY, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
