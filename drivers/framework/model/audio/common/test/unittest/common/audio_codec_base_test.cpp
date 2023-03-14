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
class AudioCodecBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AudioCodecBaseTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void AudioCodecBaseTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void AudioCodecBaseTest::SetUp()
{
}

void AudioCodecBaseTest::TearDown()
{
}

HWTEST_F(AudioCodecBaseTest, AudioCodecBaseTest_GetServiceName, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTGETSERVICENAME, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCodecBaseTest, AudioCodecBaseTest_GetDaiName, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTGETDAINAME, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCodecBaseTest, AudioCodecBaseTest_GetConfigInfo, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTGETCONFIGINFO, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCodecBaseTest, AudioCodecBaseTest_SetConfigInfo, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTSETCONFIGINFO, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioCodecBaseTest, AudioCodecBaseTest_SetCtlFunc, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTSETCTLFUNC, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}
