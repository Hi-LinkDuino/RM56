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
class AudioDaiBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AudioDaiBaseTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void AudioDaiBaseTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void AudioDaiBaseTest::SetUp()
{
}

void AudioDaiBaseTest::TearDown()
{
}

HWTEST_F(AudioDaiBaseTest, AudioDaiBaseTest_DataFromCard, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDAIDATAFROMCARD, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioDaiBaseTest, AudioDaiBaseTest_GetConfigInfo, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDAIGETCONFIGINFO, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioDaiBaseTest, AudioDaiBaseTest_CheckSampleRate, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDAICHECKSAMPLERATE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioDaiBaseTest, AudioDaiBaseTest_SetConfigInfo, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDAISETCONFIGINFO, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}
