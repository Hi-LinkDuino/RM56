/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <gtest/gtest.h>
#include "hi3516_audio_driver_test.h"
#include "hdf_uhdf_test.h"

using namespace testing::ext;

namespace {
class Hi3516DaiOpsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void Hi3516DaiOpsTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void Hi3516DaiOpsTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void Hi3516DaiOpsTest::SetUp()
{
}

void Hi3516DaiOpsTest::TearDown()
{
}

// dai driver test
HWTEST_F(Hi3516DaiOpsTest, Hi3516DaiOpsTest_DaiDeviceInit, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTDAIDEVICEINIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516DaiOpsTest, Hi3516DaiOpsTest_DaiStartup, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTDAISTARTUP, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}


HWTEST_F(Hi3516DaiOpsTest, Hi3516DaiOpsTest_DaiHwParams, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTDAIHWPARAMS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516DaiOpsTest, Hi3516DaiOpsTest_Trigger, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTDAITRIGGER, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}
