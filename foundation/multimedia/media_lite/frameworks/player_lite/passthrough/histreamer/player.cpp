/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "player.h"

#include <cinttypes>
#include <memory>
#include <sys/stat.h>
#include "histreamer/hiplayer.h"
#include "media_log.h"

using namespace std;

namespace OHOS {
namespace Media {
class Player;

namespace {
std::shared_ptr<Media::PlayerInterface> g_player;
}

Player::Player()
{
    MEDIA_INFO_LOG("Player constructor in");
    g_player = CreateHiPlayer();
    MEDIA_INFO_LOG("Player constructor out");
}

Player::~Player()
{
    MEDIA_INFO_LOG("~Player destructor");
}

int32_t Player::SetSource(const Source &source)
{
    MEDIA_INFO_LOG("Player SetSource in");
    int32_t ret;
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("player null");
        return -1;
    }
    ret = g_player->SetSource(source);
    MEDIA_INFO_LOG("Player SetSource out, ret: %d", ret);
    return ret;
}

int32_t Player::Prepare()
{
    MEDIA_INFO_LOG("Player Prepare in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("player null");
        return -1;
    }
    return g_player->Prepare();
}

int32_t Player::Play()
{
    MEDIA_INFO_LOG("Player Play in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("player null");
        return -1;
    }
    return g_player->Play();
}

bool Player::IsPlaying()
{
    MEDIA_INFO_LOG("Player IsPlaying in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("player null");
        return false;
    }
    return g_player->IsPlaying();
}

int32_t Player::Pause()
{
    MEDIA_INFO_LOG("Player Pause in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("player null");
        return -1;
    }
    return g_player->Pause();
}

int32_t Player::Stop()
{
    MEDIA_INFO_LOG("Player Stop in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("player null");
        return -1;
    }
    return g_player->Stop();
}

int32_t Player::Rewind(int64_t mSeconds, int32_t mode)
{
    MEDIA_INFO_LOG("Player Rewind in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("player null");
        return -1;
    }
    return g_player->Rewind(mSeconds, mode);
}

int32_t Player::SetVolume(float leftVolume, float rightVolume)
{
    MEDIA_INFO_LOG("Player SetVolume in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("player null");
        return -1;
    }
    return g_player->SetVolume(leftVolume, rightVolume);
}

bool Player::IsSingleLooping()
{
    MEDIA_INFO_LOG("Player IsSingleLooping in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("player null");
        return false;
    }
    return g_player->IsSingleLooping();
}

int32_t Player::GetCurrentTime(int64_t &time) const
{
    MEDIA_INFO_LOG("Player GetCurrentTime in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("player null");
        return -1;
    }
    return g_player->GetCurrentPosition(time);
}

int32_t Player::GetDuration(int64_t &durationMs) const
{
    MEDIA_INFO_LOG("Player GetDuration in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("player null");
        return -1;
    }
    return g_player->GetDuration(durationMs);
}

int32_t Player::GetVideoWidth(int32_t &videoWidth)
{
    MEDIA_INFO_LOG("Player GetVideoWidth in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("player null");
        return -1;
    }
    return g_player->GetVideoWidth(videoWidth);
}

int32_t Player::GetVideoHeight(int32_t &videoHeight)
{
    MEDIA_INFO_LOG("Player GetVideoHeight in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("player null");
        return -1;
    }
    return g_player->GetVideoHeight(videoHeight);
}

int32_t Player::Reset()
{
    MEDIA_INFO_LOG("Player Reset in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("player null");
        return -1;
    }
    return g_player->Reset();
}

int32_t Player::Release()
{
    MEDIA_INFO_LOG("Player Release in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("player null");
        return -1;
    }
    return g_player->Release();
}

void Player::SetPlayerCallback(const std::shared_ptr<PlayerCallback> &cb)
{
    MEDIA_INFO_LOG("Player SetPlayerCallback in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("ptr null");
        return;
    }
    g_player->SetPlayerCallback(cb);
}

int32_t Player::EnableSingleLooping(bool loop)
{
    MEDIA_INFO_LOG("Player EnableSingleLooping in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("player null");
        return -1;
    }
    return g_player->SetLoop(loop);
}

int32_t Player::GetPlayerState(int32_t &state) const
{
    MEDIA_INFO_LOG("Player GetPlayerState in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("player null");
        return PLAYER_STATE_ERROR;
    }
    return g_player->GetPlayerState(state);
}

int32_t Player::SetPlaybackSpeed(float speed)
{
    MEDIA_INFO_LOG("Player SetPlaybackSpeed in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("player null");
        return -1;
    }
    return g_player->SetPlaybackSpeed(speed);
}

int32_t Player::GetPlaybackSpeed(float &speed)
{
    MEDIA_INFO_LOG("Player GetPlaybackSpeed in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("player null");
        return -1;
    }
    return g_player->GetPlaybackSpeed(speed);
}

int32_t Player::SetAudioStreamType(int32_t type)
{
    MEDIA_INFO_LOG("Player SetAudioStreamType in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("player null");
        return -1;
    }
    return g_player->SetAudioStreamType(type);
}

void Player::GetAudioStreamType(int32_t &type)
{
    MEDIA_INFO_LOG("Player GetAudioStreamType in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("ptr null");
        return;
    }
    g_player->GetAudioStreamType(type);
}

int32_t Player::SetParameter(const Format &params)
{
    MEDIA_INFO_LOG("Player SetParameter in");
    if (g_player == nullptr) {
        MEDIA_ERR_LOG("player null");
        return -1;
    }
    return g_player->SetParameter(params);
}
} // namespace Media
} // namespace OHOS
