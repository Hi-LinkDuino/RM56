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
class AudioAccessoryBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void AudioAccessoryBaseTest::SetUpTestCase()
{
    HdfTestOpenService();
}

void AudioAccessoryBaseTest::TearDownTestCase()
{
    HdfTestCloseService();
}

void AudioAccessoryBaseTest::SetUp()
{
}

void AudioAccessoryBaseTest::TearDown()
{
}

HWTEST_F(AudioAccessoryBaseTest, AudioAccessoryBaseTest_I2cReadWrite, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTI2CREADWRITE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioAccessoryBaseTest, AudioAccessoryBaseTest_RegBitsRead, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTREGBITSREAD, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioAccessoryBaseTest, AudioAccessoryBaseTest_RegBitsUpdate, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTREGBITSUPDATE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioAccessoryBaseTest, AudioAccessoryBaseTest_DeviceFrequencyParse, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDEVICEFREQUENCYPARSE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioAccessoryBaseTest, AudioAccessoryBaseTest_DaiParamsUpdate, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDAIPARAMSUPDATE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioAccessoryBaseTest, AudioAccessoryBaseTest_DeviceCfgGet, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDEVICECFGGET, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioAccessoryBaseTest, AudioAccessoryBaseTest_DeviceCtrlRegInit, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDEVICECTRLREGINIT, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioAccessoryBaseTest, AudioAccessoryBaseTest_DeviceRegRead, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDEVICEREGREAD, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioAccessoryBaseTest, AudioAccessoryBaseTest_DeviceRegWrite, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTDEVICEREGWRITE, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}

HWTEST_F(AudioAccessoryBaseTest, AudioAccessoryBaseTest_AccessoryGetConfigInfo, TestSize.Level1)
{
    struct HdfTestMsg msg = {g_testAudioType, TESTAGETCONFIGINFO, -1};
    EXPECT_EQ(0, HdfTestSendMsgToService(&msg));
}
}
