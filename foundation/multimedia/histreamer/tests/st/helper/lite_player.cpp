/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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
#ifdef OHOS_LITE
#include "test_player.hpp"

#include <iostream>
#include <thread>
#include <chrono>

#include "scene/lite/hiplayer.h"
#include "foundation/log.h"
#include "foundation/cpp_ext/memory_ext.h"

using namespace OHOS::Media;

namespace OHOS::Media::Test {
static bool g_playFinished = false;
static bool g_seekFinished = false;

class PlayerCallbackImpl : public PlayerCallback {
    void OnPlaybackComplete() override
    {
        g_playFinished = true;
    }

    void OnError(int32_t errorType, int32_t errorCode) override
    {
    }

    void OnInfo(int type, int extra) override
    {
    }

    void OnVideoSizeChanged(int width, int height) override
    {
    }

    void OnRewindToComplete() override
    {
        g_seekFinished = true;
    }
};

static std::shared_ptr<PlayerCallback> gCallback = std::make_shared<PlayerCallbackImpl>();

class TestPlayerImpl : public TestPlayer {
public:
    explicit TestPlayerImpl(std::shared_ptr<Media::PlayerInterface> player) : player_(std::move(player)) {}
    ~TestPlayerImpl() override
    {
        MEDIA_LOG_I("TestPlayerImpl dtor called.");
    }
    int32_t SetSource(const TestSource& source) override;
    int32_t SetSingleLoop(bool loop) override;
    bool IsPlaying() override;
    int32_t Prepare() override;
    int32_t Play() override;
    int32_t Pause() override;
    int32_t Stop() override;
    int32_t Reset() override;
    int32_t Seek(int64_t timeMs) override;
    int32_t GetCurrentTime(int64_t& currentMS) override;
    int32_t GetDuration(int64_t& durationMs) override;
    int32_t Release();
    int32_t SetVolume(float leftVolume, float rightVolume);
private:
    std::shared_ptr<Media::PlayerInterface> player_;
};

std::unique_ptr<TestPlayer> TestPlayer::Create()
{
    auto player = OHOS::Media::CreateHiPlayer();
    FALSE_LOG(player->Init() == 0);
    player->SetPlayerCallback(gCallback);
    g_playFinished = false;
    return CppExt::make_unique<TestPlayerImpl>(player);
}

int32_t TestPlayerImpl::SetSource(const TestSource& source)
{
    OHOS::Media::Source sourceParam(source.url_);
    return player_->SetSource(sourceParam);
}

int32_t TestPlayerImpl::SetSingleLoop(bool loop)
{
    return player_->SetLoop(loop);
}

bool TestPlayerImpl::IsPlaying()
{
    return !g_playFinished;
}

int32_t TestPlayerImpl::Prepare()
{
    return player_->Prepare();
}

int32_t TestPlayerImpl::Play()
{
    g_playFinished = false;
    return player_->Play();
}

int32_t TestPlayerImpl::Pause()
{
    return player_->Pause();
}

int32_t TestPlayerImpl::Release()
{
    player_ = nullptr;
    return 0;
}

int32_t TestPlayerImpl::Stop()
{
    return player_->Stop();
}

int32_t TestPlayerImpl::Reset()
{
    return player_->Reset();
}

int32_t TestPlayerImpl::Seek(int64_t timeMs)
{
    int32_t ret = player_->Rewind(timeMs, 0);
    NZERO_RETURN(ret);
    while (!g_seekFinished) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // 50
    }
    FALSE_RETURN_V(g_seekFinished, false);
    g_seekFinished = false;
    return ret;
}

int32_t TestPlayerImpl::GetCurrentTime(int64_t& currentMS)
{
    return player_->GetCurrentPosition(currentMS);
}

int32_t TestPlayerImpl::GetDuration(int64_t& durationMs)
{
    return player_->GetDuration(durationMs);
}

int32_t TestPlayerImpl::SetVolume(float leftVolume, float rightVolume)
{
    int32_t ret = player_->SetVolume(leftVolume, rightVolume);
    return ret;
}
}
#endif