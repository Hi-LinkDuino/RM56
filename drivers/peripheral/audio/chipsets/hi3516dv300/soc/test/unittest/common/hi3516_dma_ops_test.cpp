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
class Hi3516DmaOpsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void Hi3516DmaOpsTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void Hi3516DmaOpsTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void Hi3516DmaOpsTest::SetUp()
{
}

void Hi3516DmaOpsTest::TearDown()
{
}

// dma test
HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_AudioDmaDeviceInit, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAUDIODMADEVICEINIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_Hi3516DmaBufAlloc, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTHI3516DMABUFALLOC, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_Hi3516DmaBufFree, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTHI3516DMABUFFREE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_Hi3516DmaRequestChannel, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTHI3516DMAREQUESTCHANNEL, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_Hi3516DmaConfigChannel, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTHI3516DMACONFIGCHANNEL, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_Hi3516DmaPrep, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTHI3516DMAPREP, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}


HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_Hi3516DmaSubmit, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTHI3516DMASUBMIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_Hi3516DmaPending, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTHI3516DMAPENDING, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_Hi3516DmaPause, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTHI3516DMAPAUSE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}


HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_Hi3516DmaResume, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTHI3516DMARESUME, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516DmaOpsTest, Hi3516DmaOpsTest_Hi3516DmaPointer, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTHI3516DMAPOINTER, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}
