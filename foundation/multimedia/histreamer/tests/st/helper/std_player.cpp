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
#ifndef OHOS_LITE
#include "test_player.hpp"

#include <iostream>
#include <thread>
#include <chrono>

#include "i_player_engine.h"
#include "foundation/log.h"

#include "i_engine_factory.h"

extern "C" {
__attribute__((visibility("default"))) OHOS::Media::IEngineFactory* CreateEngineFactory();
}

using namespace OHOS::Media;

namespace OHOS::Media::Test {
bool g_playFinished = false;
bool g_seekFinished = false;

class PlayerCallbackImpl : public IPlayerEngineObs {
public:
    void OnError(PlayerErrorType errorType, int32_t errorCode) override
    {
    }
    void OnInfo(PlayerOnInfoType type, int32_t extra, const Format& infoBody) override
    {
        if (type == INFO_TYPE_EOS) {
            g_playFinished = true;
        }
        if (type == INFO_TYPE_SEEKDONE) {
            g_seekFinished = true;
        }
    }
};
std::shared_ptr<IPlayerEngineObs> gCallback = std::make_shared<PlayerCallbackImpl>();

class TestPlayerImpl : public TestPlayer {
public:
    explicit TestPlayerImpl(std::unique_ptr<IPlayerEngine> player) : player_(std::move(player)) {}
    int32_t SetSource(const TestSource& source) override;
    int32_t SetSingleLoop(bool loop) override;
    bool IsPlaying() override;
    int32_t Prepare() override;
    int32_t Play() override;
    int32_t Pause() override;
    int32_t Stop() override;
    int32_t Reset() override;
    int32_t Release() override;
    int32_t Seek(int64_t timeMs) override;
    int32_t GetCurrentTime(int64_t& currentMs) override;
    int32_t GetDuration(int64_t& durationMs) override;
    int32_t SetVolume(float leftVolume, float rightVolume) override;
    int32_t GetAudioTrackInfo(std::vector<Format> &audioTrack) override;
private:
    std::unique_ptr<IPlayerEngine> player_;
};

std::unique_ptr<TestPlayer> TestPlayer::Create()
{
    auto engineFactory = std::unique_ptr<OHOS::Media::IEngineFactory>(CreateEngineFactory());
    auto player = engineFactory->CreatePlayerEngine(0, 0);
    player->SetObs(gCallback);
    g_playFinished = false;
    return std::make_unique<TestPlayerImpl>(std::move(player));
}

int32_t TestPlayerImpl::SetSource(const TestSource& source)
{
    return player_->SetSource(source.url_);
}

int32_t TestPlayerImpl::SetSingleLoop(bool loop)
{
    return player_->SetLooping(loop);
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

int32_t TestPlayerImpl::Stop()
{
    return player_->Stop();
}

int32_t TestPlayerImpl::Reset()
{
    return player_->Reset();
}

int32_t TestPlayerImpl::Release()
{
    player_ = nullptr;
    return ERR_OK;
}

int32_t TestPlayerImpl::Seek(int64_t timeMs)
{
    int32_t ret = player_->Seek(timeMs, PlayerSeekMode::SEEK_CLOSEST);
    NZERO_RETURN(ret);
    while (!g_seekFinished) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // 50
    }
    FALSE_RETURN_V(g_seekFinished, false);
    g_seekFinished = false;
    return ret;
}

int32_t TestPlayerImpl::GetCurrentTime(int64_t& currentMs)
{
    int32_t currentTimeMS = 0;
    int32_t ret = player_->GetCurrentTime(currentTimeMS);
    currentMs = currentTimeMS;
    return ret;
}

int32_t TestPlayerImpl::GetDuration(int64_t& durationMs)
{
    int32_t duration;
    int32_t ret = player_->GetDuration(duration);
    durationMs = duration;
    return ret;
}

int32_t TestPlayerImpl::SetVolume(float leftVolume, float rightVolume)
{
    int32_t ret = player_->SetVolume(leftVolume, rightVolume);
    return ret;
}

int32_t TestPlayerImpl::GetAudioTrackInfo(std::vector<Format> &audioTrack)
{
    int32_t ret = player_->GetAudioTrackInfo(audioTrack);
    return ret;
}
}
#endif