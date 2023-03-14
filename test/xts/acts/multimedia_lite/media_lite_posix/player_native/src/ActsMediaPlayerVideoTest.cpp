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


#include "ActsMediaPlayerTest.h"

namespace OHOS {
const string g_avFileName = "1080P_25fps.mp4";

static void InitSurface()
{
    OHOS::g_surface->SetUserData("region_position_x", "0");
    OHOS::g_surface->SetUserData("region_position_y", "0");
    OHOS::g_surface->SetUserData("region_width", "720");
    OHOS::g_surface->SetUserData("region_height", "540");
}

class ActsMediaPlayerVideoTest : public testing::Test {
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

static int32_t FileCheck(const string &argv)
{
    const char *ptr = argv.c_str();

    if (strlen(ptr) < FILE_PATH_LEN &&
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

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_0100
 * @tc.name      : Video Prepare Test.
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaPlayerVideoTest, player_lite_video_test_001, Level1)
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
    bool flag = g_tagTestSample.adaptr->IsPlaying();
    EXPECT_EQ(true, flag);
    g_tagTestSample.adaptr->Release();
    sleep(1);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_0200
 * @tc.name      : Video Prepare Test.
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaPlayerVideoTest, player_lite_video_test_002, Level1)
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
    sleep(1);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_0300
 * @tc.name      : Video Play Test.
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaPlayerVideoTest, player_lite_video_test_003, Level1)
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
    sleep(1);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_0400
 * @tc.name      : Video Stop Test.
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaPlayerVideoTest, player_lite_video_test_004, Level1)
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
    sleep(1);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_0500
 * @tc.name      : Video Stop Test.
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaPlayerVideoTest, player_lite_video_test_005, Level1)
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
    sleep(1);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_0600
 * @tc.name      : Video Pause Test.
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaPlayerVideoTest, player_lite_video_test_006, Level1)
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
    sleep(1);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_0700
 * @tc.name      : Video Pause Test.
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaPlayerVideoTest, player_lite_video_test_007, Level1)
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
    sleep(1);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_0800
 * @tc.name      : Video GetCurrentTime Test.
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaPlayerVideoTest, player_lite_video_test_008, Level1)
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
    sleep(1);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_0900
 * @tc.name      : Video GetCurrentTime Test.
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaPlayerVideoTest, player_lite_video_test_009, Level1)
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
    sleep(1);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_1000
 * @tc.name      : Video GetDuration Test.
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaPlayerVideoTest, player_lite_video_test_010, Level1)
{
    int32_t ret = FileCheck(g_avFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(1);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Rewind(1, PLAYER_SEEK_NEXT_SYNC);
    EXPECT_EQ(HI_SUCCESS, ret);
    int64_t currentPosition;
    ret = g_tagTestSample.adaptr->GetCurrentTime(currentPosition);
    EXPECT_EQ(HI_SUCCESS, ret);
    g_tagTestSample.adaptr->Release();
    sleep(1);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_1100
 * @tc.name      : Video Rewind Test.
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaPlayerVideoTest, player_lite_video_test_011, Level1)
{
    int32_t ret = FileCheck(g_avFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    EXPECT_EQ(HI_SUCCESS, ret);
    sleep(1);
    ret = g_tagTestSample.adaptr->Pause();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Rewind(4, PLAYER_SEEK_NEXT_SYNC);
    EXPECT_EQ(HI_SUCCESS, ret);
    g_tagTestSample.adaptr->Release();
    sleep(1);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_1200
 * @tc.name      : Video GetDuration Test.
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaPlayerVideoTest, player_lite_video_test_012, Level1)
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
    sleep(1);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_1300
 * @tc.name      : Video GetVideoSurfaceSize Test.
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaPlayerVideoTest, player_lite_video_test_013, Level1)
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
    sleep(1);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_1400
 * @tc.name      : Video GetVideoSurfaceSize Test.
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaPlayerVideoTest, player_lite_video_test_014, Level1)
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
    g_tagTestSample.adaptr->Release();
    sleep(1);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_1500
 * @tc.name      : Video GetVideoSurfaceSize Test.
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaPlayerVideoTest, player_lite_video_test_015, Level1)
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
    int32_t videoHeight;
    ret = g_tagTestSample.adaptr->GetVideoHeight(videoHeight);
    EXPECT_EQ(HI_SUCCESS, ret);
    g_tagTestSample.adaptr->Release();
    sleep(1);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_1600
 * @tc.name      : Video Reset Test.
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaPlayerVideoTest, player_lite_video_test_016, Level1)
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
    sleep(1);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_1700
 * @tc.name      : Video Reset Test.
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaPlayerVideoTest, player_lite_video_test_017, Level1)
{
    int32_t ret = FileCheck(g_avFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Reset();
    EXPECT_EQ(HI_SUCCESS, ret);
    g_tagTestSample.adaptr->Release();
    sleep(1);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_1800
 * @tc.name      : Video Release Test.
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaPlayerVideoTest, player_lite_video_test_018, Level1)
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
    sleep(1);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_1900
 * @tc.name      : Video EnableSingleLooping Test.
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaPlayerVideoTest, player_lite_video_test_019, Level1)
{
    int32_t ret = FileCheck(g_avFileName);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = CreateAndSetSource();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Prepare();
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->SetVideoSurface(g_surface);
    EXPECT_EQ(HI_SUCCESS, ret);
    bool loop = true;
    ret = g_tagTestSample.adaptr->EnableSingleLooping(loop);
    EXPECT_EQ(HI_SUCCESS, ret);
    ret = g_tagTestSample.adaptr->Play();
    sleep(1);
    EXPECT_EQ(HI_SUCCESS, ret);
    bool flag = g_tagTestSample.adaptr->IsPlaying();
    EXPECT_EQ(true, flag);
    g_tagTestSample.adaptr->Release();
    sleep(1);
}

/* *
 * @tc.number    : SUB_MEDIA_PLAYER_PLAY_2000
 * @tc.name      : Video EnableSingleLooping Test.
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(ActsMediaPlayerVideoTest, player_lite_video_test_020, Level1)
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
    bool loop = true;
    ret = g_tagTestSample.adaptr->EnableSingleLooping(loop);
    EXPECT_EQ(HI_SUCCESS, ret);
    bool flag = g_tagTestSample.adaptr->IsPlaying();
    EXPECT_EQ(true, flag);
    g_tagTestSample.adaptr->Release();
}
} // OHOS
