/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "components/ui_video.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    const int8_t STATE_PLAY = 1;
    const int8_t STATE_PAUSE = 2;
    const int8_t STATE_STOP = 3;
    const int8_t STATE_COMPLETE = 4;
    const int8_t STATE_REWIND = 5;
    const int8_t STATE_ERROR = 6;
    const int8_t STATE_INFO = 7;
    const int8_t STATE_RESIZE = 8;
}
static int8_t g_state = 0;

class TestVideoListener : public UIVideo::VideoPlayerListener {
public:
    TestVideoListener() {}
    ~TestVideoListener() {}

    void OnRewindToComplete() override
    {
        EXPECT_EQ(g_state, STATE_REWIND);
    }

    void OnPlaybackPause() override
    {
        EXPECT_EQ(g_state, STATE_PAUSE);
    }

    void OnPlaybackPlay() override
    {
        EXPECT_EQ(g_state, STATE_PLAY);
    }

    void OnPlaybackStop() override
    {
        EXPECT_EQ(g_state, STATE_STOP);
    }

    void OnPlaybackComplete() override
    {
        EXPECT_EQ(g_state, STATE_ERROR);
    }

    void OnError(int32_t errorType, int32_t errorCode) override
    {
        EXPECT_EQ(g_state, STATE_COMPLETE);
    }

    void OnInfo(int type, int extra) override
    {
        EXPECT_EQ(g_state, STATE_INFO);
    }

    void OnVideoSizeChanged(int width, int height) override
    {
        EXPECT_EQ(g_state, STATE_RESIZE);
    }
};

class UIVideoTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static UIVideo* video_;
    static TestVideoListener* listener_;
    const char* videoPath_ = "/user/data/video.mp4";
};

UIVideo* UIVideoTest::video_ = nullptr;
TestVideoListener* UIVideoTest::listener_ = nullptr;

void UIVideoTest::SetUpTestCase(void)
{
    if (listener_ == nullptr) {
        listener_ = new TestVideoListener();
    }
}

void UIVideoTest::TearDownTestCase(void)
{
    if (video_ != nullptr) {
        delete video_;
        video_ = nullptr;
    }
    if (listener_ != nullptr) {
        delete listener_;
        listener_ = nullptr;
    }
}

/**
 * @tc.name: UIVideoSetSrc_001
 * @tc.desc: Verify SetSrc function, abnormal situation.
 * @tc.type: FUNC
 * @tc.require: SR000F3PEN
 */
HWTEST_F(UIVideoTest, UIVideoSetSrc_001, TestSize.Level1)
{
    if (video_ == nullptr) {
        video_ = new UIVideo();
    }

    const char* src = nullptr;
    EXPECT_EQ(video_->SetSrc(src), false);

    src = "not exit path";
    EXPECT_EQ(video_->SetSrc(src), false);
    EXPECT_EQ(video_->GetSrc(), src);

    video_->Reset();
    delete video_;
    video_ = nullptr;
}

/**
 * @tc.name: VideoSetSrc_002
 * @tc.desc: Verify SetSrc function, normal situation.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5Q
 */
HWTEST_F(UIVideoTest, UIVideoSetSrc_002, TestSize.Level1)
{
    if (video_ == nullptr) {
        video_ = new UIVideo();
    }

    if (video_->SetSrc(videoPath_)) {
        EXPECT_EQ(video_->GetSrc(), videoPath_);
    }
    video_->Reset();
    delete video_;
    video_ = nullptr;
}

/**
 * @tc.name: VideoPrepare_001
 * @tc.desc: Verify Prepare function, abnormal situation.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5Q
 */
HWTEST_F(UIVideoTest, UIVideoPrepare_001, TestSize.Level1)
{
    if (video_ == nullptr) {
        video_ = new UIVideo();
    }

    video_->SetSrc(nullptr);
    EXPECT_EQ(video_->Prepare(), false);
    video_->SetSrc("not exist path");
    EXPECT_EQ(video_->Prepare(), false);

    video_->Reset();
    delete video_;
    video_ = nullptr;
}

/**
 * @tc.name: VideoPrepare_002
 * @tc.desc: Verify Prepare function, normal situation.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5Q
 */
HWTEST_F(UIVideoTest, UIVideoPrepare_002, TestSize.Level1)
{
    if (video_ == nullptr) {
        video_ = new UIVideo();
    }

    if (!video_->SetSrc(videoPath_)) {
        return;
    }
    if (listener_ != nullptr) {
        video_->SetVideoPlayerListener(listener_);
    }

    g_state = STATE_RESIZE;
    EXPECT_EQ(video_->Prepare(), true);

    video_->Reset();
    delete video_;
    video_ = nullptr;
}

/**
 * @tc.name: VideoPlay_001
 * @tc.desc: Verify Play function, abnormal situation.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5Q
 */
HWTEST_F(UIVideoTest, UIVideoPlay_001, TestSize.Level1)
{
    if (video_ == nullptr) {
        video_ = new UIVideo();
    }

    video_->SetSrc("not exist path");
    video_->Prepare();
    EXPECT_EQ(video_->Play(), false);

    video_->Reset();
    delete video_;
    video_ = nullptr;
}

/**
 * @tc.name: VideoPlay_002
 * @tc.desc: Verify Play function, normal situation.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5Q
 */
HWTEST_F(UIVideoTest, UIVideoPlay_002, TestSize.Level0)
{
    if (video_ == nullptr) {
        video_ = new UIVideo();
    }

    if (!video_->SetSrc(videoPath_)) {
        return;
    }
    if (listener_ != nullptr) {
        video_->SetVideoPlayerListener(listener_);
    }

    g_state = STATE_RESIZE;
    video_->Prepare();

    g_state = STATE_PLAY;
    EXPECT_EQ(video_->Play(), true);

    g_state = STATE_PAUSE;
    video_->Pause();

    g_state = STATE_PLAY;
    EXPECT_EQ(video_->Play(), true);
    g_state = STATE_STOP;
    video_->Stop();
    g_state = STATE_PLAY;
    EXPECT_EQ(video_->Play(), false);

    video_->Reset();
    delete video_;
    video_ = nullptr;
}

/**
 * @tc.name: VideoIsPlaying_001
 * @tc.desc: Verify IsPlaying function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5Q
 */
HWTEST_F(UIVideoTest, UIVideoIsPlaying_001, TestSize.Level1)
{
    if (video_ == nullptr) {
        video_ = new UIVideo();
    }

    if (!video_->SetSrc(videoPath_)) {
        return;
    }
    video_->Prepare();
    EXPECT_EQ(video_->IsPlaying(), false);

    video_->Play();
    EXPECT_EQ(video_->IsPlaying(), true);

    video_->Reset();
    delete video_;
    video_ = nullptr;
}

/**
 * @tc.name: VideoIsSingleLooping_001
 * @tc.desc: Verify IsSingleLooping function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5Q
 */
HWTEST_F(UIVideoTest, UIVideoIsSingleLooping_001, TestSize.Level1)
{
    if (video_ == nullptr) {
        video_ = new UIVideo();
    }

    if (!video_->SetSrc(videoPath_)) {
        return;
    }
    video_->Prepare();
    video_->Play();
    EXPECT_EQ(video_->IsSingleLooping(), false);
    video_->EnableSingleLooping(true);
    EXPECT_EQ(video_->IsSingleLooping(), true);

    video_->EnableSingleLooping(false);
    EXPECT_EQ(video_->IsSingleLooping(), false);
    video_->Reset();
    delete video_;
    video_ = nullptr;
}

/**
 * @tc.name: VideoRewind_001
 * @tc.desc: Verify Rewind function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5Q
 */
HWTEST_F(UIVideoTest, UIVideoRewind_001, TestSize.Level0)
{
    if (video_ == nullptr) {
        video_ = new UIVideo();
    }

    if (!video_->SetSrc(videoPath_)) {
        return;
    }
    if (listener_ != nullptr) {
        video_->SetVideoPlayerListener(listener_);
    }

    g_state = STATE_RESIZE;
    video_->Prepare();
    g_state = STATE_PLAY;
    video_->Play();
    int64_t currentTime = 0;
    video_->GetCurrentTime(currentTime);
    int64_t rewindTime = currentTime + 3000; // 3000:rewind milli second

    g_state = STATE_REWIND;
    video_->Rewind(rewindTime);
    video_->GetCurrentTime(currentTime);
    EXPECT_EQ(currentTime, rewindTime);

    video_->Reset();
    delete video_;
    video_ = nullptr;
}

/**
 * @tc.name: VideoPause_001
 * @tc.desc: Verify Pause function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5Q
 */
HWTEST_F(UIVideoTest, UIVideoPause_001, TestSize.Level0)
{
    if (video_ == nullptr) {
        video_ = new UIVideo();
    }

    if (!video_->SetSrc(videoPath_)) {
        return;
    }
    if (listener_ != nullptr) {
        video_->SetVideoPlayerListener(listener_);
    }

    g_state = STATE_RESIZE;
    video_->Prepare();
    g_state = STATE_PLAY;
    video_->Play();

    g_state = STATE_PAUSE;
    EXPECT_EQ(video_->Pause(), true);

    video_->Reset();
    EXPECT_EQ(video_->Pause(), false);

    video_->Reset();
    delete video_;
    video_ = nullptr;
}

/**
 * @tc.name: VideoStop_001
 * @tc.desc: Verify Stop function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5Q
 */
HWTEST_F(UIVideoTest, UIVideoStop_001, TestSize.Level0)
{
    if (video_ == nullptr) {
        video_ = new UIVideo();
    }

    if (!video_->SetSrc(videoPath_)) {
        return;
    }
    if (listener_ != nullptr) {
        video_->SetVideoPlayerListener(listener_);
    }

    g_state = STATE_RESIZE;
    video_->Prepare();
    g_state = STATE_PLAY;
    video_->Play();

    g_state = STATE_STOP;
    EXPECT_EQ(video_->Stop(), true);

    video_->Reset();
    EXPECT_EQ(video_->Stop(), false);

    video_->Reset();
    delete video_;
    video_ = nullptr;
}

/**
 * @tc.name: VideoReset_001
 * @tc.desc: Verify Reset function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5Q
 */
HWTEST_F(UIVideoTest, UIVideoReset_001, TestSize.Level0)
{
    if (video_ == nullptr) {
        video_ = new UIVideo();
    }

    if (!video_->SetSrc(videoPath_)) {
        return;
    }
    if (listener_ != nullptr) {
        video_->SetVideoPlayerListener(listener_);
    }

    g_state = STATE_RESIZE;
    video_->Prepare();
    g_state = STATE_PLAY;
    video_->Play();

    EXPECT_EQ(video_->Reset(), true);

    video_->Reset();
    delete video_;
    video_ = nullptr;
}
} // namespace OHOS
