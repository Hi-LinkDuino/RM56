/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_common_test.h"
#include <gtest/gtest.h>
#include "hdf_uhdf_test.h"

using namespace testing::ext;
namespace {
class AudioCoreTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AudioCoreTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void AudioCoreTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void AudioCoreTest::SetUp()
{
}

void AudioCoreTest::TearDown()
{
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioDeviceReadReg, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIODEVICEREADREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioDeviceWriteReg, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIODEVICEWRITEREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioSocRegisterPlatform, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOSOCREGISTERPLATFORM, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioSocRegisterDai, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOSOCREGISTERDAI, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioRegisterDsp, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOREGISTERDSP, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioRegisterCodec, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOREGISTERCODEC, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioRegisterAccessory, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOREGISTERACCESSORY, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioBindDaiLink, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOBINDDAILINK, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioUpdateCodecRegBits, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOUPDATECODECREGBITS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioUpdateAccessoryRegBits, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOUPDATEACCESSORYREGBITS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioUpdateDaiRegBits, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOUPDATEDAIREGBITS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioKcontrolGetCpuDai, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOKCONTROLGETCPUDAI, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioKcontrolGetCodec, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOKCONTROLGETCODEC, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioKcontrolGetAccessory, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOKCONTROLGETACCESSORY, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioAddControls, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOADDCONTROLS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioAddControl, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOADDCONTROL, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioGetCtrlOpsRReg, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOGETCTRLOPSRREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioGetCtrlOpsReg, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOGETCTRLOPSREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioSetCtrlOpsReg, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOSETCTRLOPSREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioSetCtrlOpsRReg, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOSETCTRLOPSRREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioDaiReadReg, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIODAIREADREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioDaiWriteReg, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIODAIWRITEREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioCodecReadReg, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOCODECREADREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioCodecWriteReg, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOCODECWRITEREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioAccessoryReadReg, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOACCESSORYREADREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioAccessoryWriteReg, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOACCESSORYWRITEREG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioInfoCtrlOps, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOINFOCTRLOPS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioCodecGetCtrlOps, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOCODECGETCTRLOPS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioCodecSetCtrlOps, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOCODECSETCTRLOPS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioAccessoryGetCtrlOps, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOACCESSORYGETCTRLOPS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioAccessorySetCtrlOpsTset, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOACCESSORYSETCTRLOPS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioCpuDaiSetCtrlOps, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOCPUDAISETCTRLOPS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCoreTest, AudioCoreTest_AudioCpuDaiGetCtrlOps, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOCPUDAIGETCTRLOPS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}
