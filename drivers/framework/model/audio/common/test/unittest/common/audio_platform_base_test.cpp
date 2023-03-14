/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <gtest/gtest.h>
#include "audio_common_test.h"
#include "hdf_uhdf_test.h"

using namespace testing::ext;

namespace {
class AudioPlatformBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AudioPlatformBaseTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void AudioPlatformBaseTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void AudioPlatformBaseTest::SetUp()
{
}

void AudioPlatformBaseTest::TearDown()
{
}

HWTEST_F(AudioPlatformBaseTest, AudioPlatformBaseTest_PlatformDataFromCardTest, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDATAFROMCARD, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPlatformBaseTest_AudioBytesToFramesTest, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTBYTESTOFRAMES, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPlatformBaseTest_AudioDataBigEndianChangeTest, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDATABIGENDIANCHANGE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPlatformBaseTest_AudioFramatToBitWidthTest, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTFRAMATTOBITWIDTH, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPlatformBaseTest_AudioSetPcmInfoTest, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTSETPCMINFO, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPlatformBaseTest_AudioSetRenderBufInfoTest, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTSETRENDERBUFINFO, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPlatformBaseTest_AudioSetCaptureBufInfoTest, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTSETCAPTUREBUFINFO, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPlatformBaseTest_AudioPcmWriteTest, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTPCMWRITE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPlatformBaseTest_AudioPcmReadTest, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTPCMREAD, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPlatformBaseTest_AudioPcmMmapWriteTest, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTPCMMMAPWRITE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPlatformBaseTest_AudioPcmMmapReadTest, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTPCMMMAPREAD, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPlatformBaseTest_AudioRenderOpenTest, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTRENDEROPEN, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPlatformBaseTest_AudioCaptureOpenTest, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTCAPTUREOPEN, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPlatformBaseTest_AudioRenderCloseTest, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTRENDERCLOSE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPlatformBaseTest_AudioPcmPointerTest, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTPCMPOINTER, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPlatformBaseTest_AudioCaptureCloseTest, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTCAPTURECLOSE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPlatformBaseTest_AudioHwParamsTest, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTHWPARAMS, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPlatformBaseTest_AudioRenderPrepareTest, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTRENDERPREPARE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPlatformBaseTest_AudioCapturePrepareTest, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTCAPTUREPREPARE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPlatformBaseTest_AudioRenderTriggerTest, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTRENDERTRIGGER, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioPlatformBaseTest, AudioPlatformBaseTest_AudioCaptureTriggerTest, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTCAPTURETRIGGER, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}
