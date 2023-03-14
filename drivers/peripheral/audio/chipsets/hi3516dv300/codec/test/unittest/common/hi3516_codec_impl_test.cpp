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
class Hi3516CodecImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void Hi3516CodecImplTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void Hi3516CodecImplTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void Hi3516CodecImplTest::SetUp()
{
}

void Hi3516CodecImplTest::TearDown()
{
}


HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_CodecHalSysInit, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTCODECHALSYSINIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_AcodecRegBitsRead, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTACODECREGBITSREAD, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_CodecRegBitsUpdate, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTCODECREGBITSUPDATE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_CodecRegDefaultInit, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTCODECREGDEFAULTINIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_CodecSetAdcTuneEnable, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTCODECSETADCTUNEENABLE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_CodecDaiParamsUpdate, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTCODECDAIPARAMSUPDATE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_AudioCodecAiaoSetCtrlOps, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAUDIOCODECAIAOSETCTRLOPS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516CodecImplTest, Hi3516CodecImplTest_AudioCodecAiaoGetCtrlOps, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAUDIOCODECAIAOGETCTRLOPS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}
