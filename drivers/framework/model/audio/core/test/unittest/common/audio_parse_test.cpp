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
class AudioParseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AudioParseTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void AudioParseTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void AudioParseTest::SetUp()
{
}

void AudioParseTest::TearDown()
{
}

HWTEST_F(AudioParseTest, AudioParseTest_AudioFillConfigData, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_AUDIOFILLCONFIGDATA, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioParseTest, AudioParseTest_CodecGetRegConfig, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TEST_CODECGETREGCONFIG, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}
