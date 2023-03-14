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
class Hi3516AiaoImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void Hi3516AiaoImplTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void Hi3516AiaoImplTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void Hi3516AiaoImplTest::SetUp()
{
}

void Hi3516AiaoImplTest::TearDown()
{
}
// aiao driver test

HWTEST_F(Hi3516AiaoImplTest, Hi3516AiaoImplTest_AiopRegCfg, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAIOPREGCFG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516AiaoImplTest, Hi3516AiaoImplTest_AiaoHalSysInit, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAIAOHALSYSINIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
HWTEST_F(Hi3516AiaoImplTest, Hi3516AiaoImplTest_AiaoClockReset, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAIAOCLOCKRESET, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516AiaoImplTest, Hi3516AiaoImplTest_AiaoHalReadReg, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAIAOHALREADREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516AiaoImplTest, Hi3516AiaoImplTest_AopHalSetBuffRptr, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAOPHALSETBUFFRPTR, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516AiaoImplTest, Hi3516AiaoImplTest_AopHalSetBuffWptr, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAOPHALSETBUFFWPTR, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516AiaoImplTest, Hi3516AiaoImplTest_AipHalSetBuffWptr, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAIPHALSETBUFFWPTR, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516AiaoImplTest, Hi3516AiaoImplTest_AipHalSetBuffRptr, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAIPHALSETBUFFRPTR, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516AiaoImplTest, Hi3516AiaoImplTest_AopHalSetTxStart, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAOPHALSETTXSTART, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516AiaoImplTest, Hi3516AiaoImplTest_AipHalSetRxStart, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAIPHALSETRXSTART, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516AiaoImplTest, Hi3516AiaoImplTest_AopHalDevEnable, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAOPHALDEVENABLE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516AiaoImplTest, Hi3516AiaoImplTest_AiaoDeviceInit, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAIAODEVICEINIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516AiaoImplTest, Hi3516AiaoImplTest_AudioAoInit, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAUDIOAOINIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516AiaoImplTest, Hi3516AiaoImplTest_AudioAiInit, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAUDIOAIINIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(Hi3516AiaoImplTest, Hi3516AiaoImplTest_AiaoSysPinMux, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAIAOSYSPINMUX, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
    HWTEST_F(Hi3516AiaoImplTest, Hi3516AiaoImplTest_AiaoGetMclk, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAIAOGETMCLK, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
    HWTEST_F(Hi3516AiaoImplTest, Hi3516AiaoImplTest_AiaoSetSysCtlRegValue, TestSize.Level1)
{
    struct HdfTestMsg msg = {G_TEST_HI3516_AUDIO_DRIVER_TYPE, TESTAIAOSETSYSCTLREGVALUE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}
