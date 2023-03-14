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
class AudioSapmTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AudioSapmTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void AudioSapmTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void AudioSapmTest::SetUp()
{
}

void AudioSapmTest::TearDown()
{
}

HWTEST_F(AudioSapmTest, AudioSapmTest_AudioSapmNewComponents, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOSAPMNEWCOMPONENTS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioSapmTest, AudioSapmTest_AudioSapmAddRoutes, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOSAPMADDROUTES, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioSapmTest, AudioSapmTest_AudioSapmNewControls, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOSAPMNEWCONTROLS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioSapmTest, AudioSapmTest_AudioSapmSleep, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOSAPMSLEEP, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioSapmTest, AudioSapmTest_AudioSampPowerUp, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOSAMPPOWERUP, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioSapmTest, AudioSapmTest_AudioSampSetPowerMonitor, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOSAMPSETPOWERMONITOR, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioSapmTest, AudioSapmTest_AudioCodecSapmSetCtrlOps, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOCODECSAPMSETCTRLOPS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioSapmTest, AudioSapmTest_AudioCodecSapmGetCtrlOps, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOCODECSAPMGETCTRLOPS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioSapmTest, AudioSapmTest_AudioAccessorySapmSetCtrlOps, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOACCESSORYSAPMSETCTRLOPS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioSapmTest, AudioSapmTest_AudioAccessorySapmGetCtrlOps, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOACCESSORYSAPMGETCTRLOPS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}
