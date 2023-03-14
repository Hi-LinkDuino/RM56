/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hi3516_audio_driver_test.h"
#include <gtest/gtest.h>
#include "hdf_uhdf_test.h"

using namespace testing::ext;

namespace {
class Hi3516CodecOpsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void Hi3516CodecOpsTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void Hi3516CodecOpsTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void Hi3516CodecOpsTest::SetUp()
{
}

void Hi3516CodecOpsTest::TearDown()
{
}

HWTEST_F(Hi3516CodecOpsTest, Hi3516CodecOpsTestt_CodecDeviceInit, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTCODECDEVICEINIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecOpsTest, Hi3516CodecOpsTest_CodecDaiDeviceInit, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTCODECDAIDEVICEINIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecOpsTest, Hi3516CodecOpsTest_CodecDaiHwParams, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTCODECDAIHWPARAMS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecOpsTest, Hi3516CodecOpsTest_CodecDaiStartup, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTCODECDAISTARTUP, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}