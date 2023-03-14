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
class AudioDspBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AudioDspBaseTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void AudioDspBaseTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void AudioDspBaseTest::SetUp()
{
}

void AudioDspBaseTest::TearDown()
{
}

HWTEST_F(AudioDspBaseTest, AudioDspBaseTest_DspGetServiceName, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDSPGETSERVICENAME, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioDspBaseTest, AudioDspBaseTest_DspGetDaiName, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDSPGETDAINAME, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}
