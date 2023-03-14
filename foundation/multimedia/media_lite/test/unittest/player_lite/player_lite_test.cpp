/*
 * Copyright (C) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "player_lite_test.h"
#include <fstream>
#include <iostream>
#include <climits>
#include <unistd.h>
#include <fcntl.h>
#include "securec.h"

namespace OHOS {
const string g_avFileName = "1080P_25fps.mp4";
const string g_audioFileName = "Audiochannel_002.m4a";

const int32_t HI_SUCCESS = 0;
const int32_t HI_FAILURE = -1;

using OHOS::Media::Player;
using OHOS::Media::PlayerSeekMode;
using OHOS::Media::Source;
using OHOS::Media::Format;
using OHOS::Media::StreamSource;

static void InitSurface()
{
    OHOS::g_surface->SetUserData("region_position_x", "0");
    OHOS::g_surface->SetUserData("region_position_y", "0");
    OHOS::g_surface->SetUserData("region_width", "720");
    OHOS::g_surface->SetUserData("region_height", "540");
}

class PlayerliteTest : public testing::Test {
protected:
// SetUpTestCase:The preset action of the test suite is executed before the first TestCase
    static void SetUpTestCase(void)
    {
    }
// TearDownTestCase:The test suite cleanup action is executed after the last TestCase
    static void TearDownTestCase(void)
    {
    }
// SetUp:Execute before each test case
    virtual void SetUp()
    {
        g_tagTestSample.adaptr = std::make_shared<Player>();
        InitSurface();
    }
// TearDown:Execute after each test case
    virtual void TearDown()
    {
    }
};

class PlayerliteCallback : public Media::PlayerCallback {
public:
    PlayerliteCallback() = default;

    virtual ~PlayerliteCallback() = default;

    void OnPlaybackComplete() override;

    void OnError(int32_t errorType, int32_t errorCode) override;

    void OnInfo(int type, int extra) override;

    void OnVideoSizeChanged(int width, int height) override;

    void OnRewindToComplete() override;
};

void PlayerliteCallback::OnPlaybackComplete()
{
    cout << "PlayerTest::OnPlaybackComplete ..." << endl;
}

void PlayerliteCallback::OnError(int32_t errorType, int32_t errorCode)
{
    cout << "PlayerTest::OnError ..." << endl;
}

void PlayerliteCallback::OnInfo(int32_t type, int32_t extra)
{
    cout << "PlayerTest::OnInfo ..." << endl;
}

void PlayerliteCallback::OnVideoSizeChanged(int width, int height)
{
    cout << "PlayerTest::OnVideoSizeChanged ..." << endl;
}

void PlayerliteCallback::OnRewindToComplete()
{
    cout << "PlayerTest::OnRewindToComplete ..." << endl;
}


static int32_t FileCheck(const string &argv)
{
    const char *ptr = argv.c_str();

    if (sizeof(ptr) < sizeof(g_tagTestSample.filePath) &&
        realpath(argv.c_str(), g_tagTestSample.filePath) == nullptr) {
        printf("realpath input file failed, errno: %d!\n", errno);
        return -1;
    }
    return 0;
}

static int32_t CreateAndSetSource()
{
    std::string uri(g_tagTestSample.filePath);
    Source source(uri);
    int32_t ret = g_tagTestSample.adaptr->SetSource(source);
    string ret1 = source.GetSourceUri();
    return ret;
}

/*
 * Feature: Playerlite
 * Function: Video Prepare
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Video Prepare Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_Prepare_test_001, Level1)
{
    int32_t ret = FileCheck(g_avFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    std::string uri(g_tagTestSample.filePath);
    Source source(uri);
    ret = g_tagTestSample.adaptr->SetSource(source);
    string ret1 = source.GetSourceUri();
    source.GetSourceType();
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    bool flag = g_tagTestSample.adaptr->IsPlaying();
    EXPECT_EQ(true, flag);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Video Prepare Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_Prepare_test_002, Level1)
{
    int32_t ret = FileCheck(g_avFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Video Play Playback Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_Play_test_001, Level1)
{
    int32_t ret = FileCheck(g_avFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    sleep(1);
    EXPECT_EQ(HI_SUCCESS, ret);
    bool flag = g_tagTestSample.adaptr->IsPlaying();
    EXPECT_EQ(true, flag);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Video Stop Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_Stop_test_001, Level1)
{
    int32_t ret = FileCheck(g_avFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    std::shared_ptr<PlayerCallback> cb;
    g_tagTestSample.adaptr->SetPlayerCallback(cb);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(2);
    ret = g_tagTestSample.adaptr->Stop();
    EXPECT_EQ(HI_SUCCESS, ret);
    bool flag = g_tagTestSample.adaptr->IsPlaying();
    EXPECT_EQ(false, flag);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Video Stop Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_Stop_test_002, Level1)
{
    int32_t ret = FileCheck(g_avFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Stop();
    EXPECT_EQ(HI_FAILURE, ret);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Video Pause Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_Pause_test_001, Level1)
{
    int32_t ret = FileCheck(g_avFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(2);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
    bool flag = g_tagTestSample.adaptr->IsPlaying();
    EXPECT_EQ(false, flag);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Video Pause Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_Pause_test_002, Level1)
{
    int32_t ret = FileCheck(g_avFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_FAILURE, ret);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Video GetCurrentTime Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_GetCurrentTime_test_001, Level1)
{
    int32_t ret = FileCheck(g_avFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(1);
    int64_t currentPosition;
    ret = g_tagTestSample.adaptr->GetCurrentTime(currentPosition);
    EXPECT_EQ(HI_SUCCESS, ret);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Video GetCurrentTime Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_GetCurrentTime_test_002, Level1)
{
    int32_t ret = FileCheck(g_avFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    int64_t currentPosition;
    ret = g_tagTestSample.adaptr->GetCurrentTime(currentPosition);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(1);
    bool flag = g_tagTestSample.adaptr->IsPlaying();
    EXPECT_EQ(true, flag);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Video GetDuration Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_GetDuration_test_001, Level1)
{
    int32_t ret = FileCheck(g_avFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(2);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Rewind(1, PLAYER_SEEK_NEXT_SYNC);
    EXPECT_EQ(HI_SUCCESS, ret);
    int64_t currentPosition;
    ret = g_tagTestSample.adaptr->GetCurrentTime(currentPosition);
    EXPECT_EQ(HI_SUCCESS, ret);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Video GetDuration Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_GetDuration_test_002, Level1)
{
    int32_t ret = FileCheck(g_avFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    int64_t duration;
    ret = g_tagTestSample.adaptr->GetDuration(duration);
    EXPECT_EQ(HI_SUCCESS, ret);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Video GetVideoSurfaceSize Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_GetVideoSurfaceSize_test_001, Level1)
{
    int32_t ret = FileCheck(g_avFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    int32_t videoWidth;
    ret = g_tagTestSample.adaptr->GetVideoWidth(videoWidth);
    EXPECT_EQ(HI_SUCCESS, ret);
    int32_t videoHeight;
    ret = g_tagTestSample.adaptr->GetVideoHeight(videoHeight);
    EXPECT_EQ(HI_SUCCESS, ret);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Video Reset Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_Reset_test_001, Level1)
{
    int32_t ret = FileCheck(g_avFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(2);
    ret = g_tagTestSample.adaptr->Reset();
    EXPECT_EQ(HI_SUCCESS, ret);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Video Release Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_Release_test_001, Level1)
{
    int32_t ret = FileCheck(g_avFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(2);
    ret = g_tagTestSample.adaptr->Release();
    EXPECT_EQ(HI_SUCCESS, ret);
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Audio SetSource Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_AudioSetSource_test_001, Level1)
{
    int32_t ret = FileCheck(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(2);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Audio Format Play->Pause->Play->Stop Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_AudioPlay_test_001, Level1)
{
    int32_t ret = FileCheck(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    std::shared_ptr<PlayerliteCallback> callBack;
    callBack = std::make_shared<PlayerliteCallback>();
    g_tagTestSample.adaptr->SetPlayerCallback(callBack);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Stop();
    EXPECT_EQ(HI_SUCCESS, ret);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Audio Format Play Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_Play_test_002, Level1)
{
    int32_t ret = FileCheck(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    std::shared_ptr<PlayerliteCallback> callBack;
    callBack = std::make_shared<PlayerliteCallback>();
    g_tagTestSample.adaptr->SetPlayerCallback(callBack);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Stop();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_FAILURE, ret);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Audio Format Prepare Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_audioPrepare_test_001, Level1)
{
    int32_t ret = FileCheck(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    std::shared_ptr<PlayerliteCallback> callBack;
    callBack = std::make_shared<PlayerliteCallback>();
    g_tagTestSample.adaptr->SetPlayerCallback(callBack);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Audio Format Prepare Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_audioPrepare_test_002, Level1)
{
    int32_t ret = FileCheck(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    std::shared_ptr<PlayerliteCallback> callBack;
    callBack = std::make_shared<PlayerliteCallback>();
    g_tagTestSample.adaptr->SetPlayerCallback(callBack);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Audio Format Stop Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_audioStop_test_001, Level1)
{
    int32_t ret = FileCheck(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    std::shared_ptr<PlayerliteCallback> callBack;
    callBack = std::make_shared<PlayerliteCallback>();
    g_tagTestSample.adaptr->SetPlayerCallback(callBack);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Stop();
    EXPECT_EQ(HI_SUCCESS, ret);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Audio Format Stop Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_audioStop_test_002, Level1)
{
    int32_t ret = FileCheck(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    std::shared_ptr<PlayerliteCallback> callBack;
    callBack = std::make_shared<PlayerliteCallback>();
    g_tagTestSample.adaptr->SetPlayerCallback(callBack);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Stop();
    EXPECT_EQ(HI_SUCCESS, ret);
    bool flag = g_tagTestSample.adaptr->IsPlaying();
    EXPECT_EQ(false, flag);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Audio Format Pause Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_audioPause_test_002, Level1)
{
    int32_t ret = FileCheck(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    std::shared_ptr<PlayerliteCallback> callBack;
    callBack = std::make_shared<PlayerliteCallback>();
    g_tagTestSample.adaptr->SetPlayerCallback(callBack);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Audio Format SetVolume Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_SetVolume_test_001, Level1)
{
    int32_t ret = FileCheck(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    std::shared_ptr<PlayerliteCallback> callBack;
    callBack = std::make_shared<PlayerliteCallback>();
    g_tagTestSample.adaptr->SetPlayerCallback(callBack);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVolume(40, 40);
    sleep(10);
    EXPECT_EQ(HI_SUCCESS, ret);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Audio Format SetVolume Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_SetVolume_test_002, Level1)
{
    int32_t ret = FileCheck(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    std::shared_ptr<PlayerliteCallback> callBack;
    callBack = std::make_shared<PlayerliteCallback>();
    g_tagTestSample.adaptr->SetPlayerCallback(callBack);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVolume(0, 0);
    sleep(10);
    EXPECT_EQ(HI_SUCCESS, ret);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Audio Format SetVolume Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_SetVolume_test_003, Level1)
{
    int32_t ret = FileCheck(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    std::shared_ptr<PlayerliteCallback> callBack;
    callBack = std::make_shared<PlayerliteCallback>();
    g_tagTestSample.adaptr->SetPlayerCallback(callBack);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVolume(-1, -1);
    sleep(10);
    EXPECT_EQ(HI_FAILURE, ret);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Audio Format SetVolume Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_SetVolume_test_004, Level1)
{
    int32_t ret = FileCheck(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    std::shared_ptr<PlayerliteCallback> callBack;
    callBack = std::make_shared<PlayerliteCallback>();
    g_tagTestSample.adaptr->SetPlayerCallback(callBack);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVolume(300, 300);
    sleep(10);
    EXPECT_EQ(HI_SUCCESS, ret);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Audio Format SetVolume Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_SetVolume_test_005, Level1)
{
    int32_t ret = FileCheck(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    std::shared_ptr<PlayerliteCallback> callBack;
    callBack = std::make_shared<PlayerliteCallback>();
    g_tagTestSample.adaptr->SetPlayerCallback(callBack);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVolume(300, 301);
    sleep(10);
    EXPECT_EQ(HI_FAILURE, ret);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Audio Format GetCurrentTime Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_audioGetCurrentTime_test_001, Level1)
{
    int32_t ret = FileCheck(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    std::shared_ptr<PlayerliteCallback> callBack;
    callBack = std::make_shared<PlayerliteCallback>();
    g_tagTestSample.adaptr->SetPlayerCallback(callBack);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(3);
    int64_t currentPosition;
    ret = g_tagTestSample.adaptr->GetCurrentTime(currentPosition);
    EXPECT_EQ(HI_SUCCESS, ret);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Audio Format GetCurrentTime Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_audioGetCurrentTime_test_002, Level1)
{
    int32_t ret = FileCheck(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    std::shared_ptr<PlayerliteCallback> callBack;
    callBack = std::make_shared<PlayerliteCallback>();
    g_tagTestSample.adaptr->SetPlayerCallback(callBack);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    g_tagTestSample.adaptr->Pause();
    ret = g_tagTestSample.adaptr->Rewind(0, PLAYER_SEEK_NEXT_SYNC);
    EXPECT_EQ(HI_SUCCESS, ret);
    int64_t currentPosition;
    ret = g_tagTestSample.adaptr->GetCurrentTime(currentPosition);
    EXPECT_EQ(HI_SUCCESS, ret);
    g_tagTestSample.adaptr->Release();
}

/*
 * Feature: Playerlite
 * Function: Playback
 * SubFunction: NA
 * FunctionPoints: NA.
 * EnvConditions: NA
 * CaseDescription: Audio Format Duration Test.
 */
HWTEST_F(PlayerliteTest, medialite_player_audioGetDuration_test_001, Level1)
{
    int32_t ret = FileCheck(g_audioFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    std::shared_ptr<PlayerliteCallback> callBack;
    callBack = std::make_shared<PlayerliteCallback>();
    g_tagTestSample.adaptr->SetPlayerCallback(callBack);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    int64_t duration;
    ret = g_tagTestSample.adaptr->GetDuration(duration);
    EXPECT_EQ(HI_SUCCESS, ret);
}
} // OHOS
