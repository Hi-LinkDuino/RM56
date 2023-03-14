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
#include <sys/stat.h>
#include "media_log.h"
#include "player_client.h"
#include "pms_interface.h"

using namespace std;

namespace OHOS {
namespace Media {
class Player;

#define CHK_NULL_RETURN(ptr) \
do { \
    if (ptr == nullptr) { \
        MEDIA_ERR_LOG("ptr null"); \
        return -1; \
    } \
} while (0)

Player::Player()
{
    MEDIA_INFO_LOG("Player process");
    player_ = PlayerClient::GetInstance();
}

Player::~Player()
{
    MEDIA_INFO_LOG("~Player process");
}

int32_t Player::SetSource(const Source &source)
{
    MEDIA_INFO_LOG("process in");
    int32_t ret;
    CHK_NULL_RETURN(player_);
    ret = player_->SetSource(source);
    MEDIA_INFO_LOG("process out");
    return ret;
}

int32_t Player::Prepare()
{
    MEDIA_INFO_LOG("process in");
    if (CheckSelfPermission("ohos.permission.MODIFY_AUDIO_SETTINGS") != GRANTED) {
        MEDIA_WARNING_LOG("Process can not access audio-setting.");
        return MEDIA_PERMISSION_DENIED;
    }
    if (CheckSelfPermission("ohos.permission.READ_MEDIA") != GRANTED) {
        MEDIA_WARNING_LOG("Process can not read media.");
        return MEDIA_PERMISSION_DENIED;
    }
    CHK_NULL_RETURN(player_);
    return player_->Prepare();
}

int32_t Player::Play()
{
    MEDIA_INFO_LOG("process in");
    CHK_NULL_RETURN(player_);
    return player_->Play();
}

bool Player::IsPlaying()
{
    MEDIA_INFO_LOG("process in");
    if (player_ == nullptr) {
        MEDIA_ERR_LOG("ptr null");
        return false;
    }
    return player_->IsPlaying();
}

int32_t Player::Pause()
{
    MEDIA_INFO_LOG("process in");
    CHK_NULL_RETURN(player_);
    return player_->Pause();
}

int32_t Player::Stop()
{
    MEDIA_INFO_LOG("process in");
    CHK_NULL_RETURN(player_);
    return player_->Stop();
}

int32_t Player::Rewind(int64_t mSeconds, int32_t mode)
{
    MEDIA_INFO_LOG("process in");
    CHK_NULL_RETURN(player_);
    return player_->Rewind(mSeconds, mode);
}

int32_t Player::SetVolume(float leftVolume, float rightVolume)
{
    MEDIA_INFO_LOG("process in");
    CHK_NULL_RETURN(player_);
    return player_->SetVolume(leftVolume, rightVolume);
}

int32_t Player::SetVideoSurface(Surface *surface)
{
    MEDIA_INFO_LOG("process in");
    CHK_NULL_RETURN(player_);
    return player_->SetSurface(surface);
}

bool Player::IsSingleLooping()
{
    MEDIA_INFO_LOG("process in");
    if (player_ == nullptr) {
        MEDIA_ERR_LOG("ptr null");
        return false;
    }
    return player_->IsSingleLooping();
}

int32_t Player::GetCurrentTime(int64_t &time) const
{
    CHK_NULL_RETURN(player_);
    return player_->GetCurrentPosition(time);
}

int32_t Player::GetDuration(int64_t &durationMs) const
{
    MEDIA_INFO_LOG("process in");
    CHK_NULL_RETURN(player_);
    return player_->GetDuration(durationMs);
}

int32_t Player::GetVideoWidth(int32_t &videoWidth)
{
    MEDIA_INFO_LOG("process in");
    CHK_NULL_RETURN(player_);
    return player_->GetVideoWidth(videoWidth);
}

int32_t Player::GetVideoHeight(int32_t &videoHeight)
{
    MEDIA_INFO_LOG("process in");
    CHK_NULL_RETURN(player_);
    return player_->GetVideoHeight(videoHeight);
}

int32_t Player::Reset()
{
    MEDIA_INFO_LOG("process in");
    CHK_NULL_RETURN(player_);
    return player_->Reset();
}

int32_t Player::Release()
{
    MEDIA_INFO_LOG("process in");
    CHK_NULL_RETURN(player_);
    return player_->Release();
}

void Player::SetPlayerCallback(const std::shared_ptr<PlayerCallback> &cb)
{
    MEDIA_INFO_LOG("process in");
    if (player_ == nullptr) {
        MEDIA_ERR_LOG("ptr null");
        return;
    }
    player_->SetPlayerCallback(cb);
}

int32_t Player::EnableSingleLooping(bool loop)
{
    MEDIA_INFO_LOG("process in");
    CHK_NULL_RETURN(player_);
    return player_->SetLoop(loop);
}

int32_t Player::GetPlayerState(int32_t &state) const
{
    MEDIA_INFO_LOG("process in");
    CHK_NULL_RETURN(player_);
    return player_->GetPlayerState(state);
}

int32_t Player::SetPlaybackSpeed(float speed)
{
    MEDIA_INFO_LOG("process in");
    CHK_NULL_RETURN(player_);
    return player_->SetPlaybackSpeed(speed);
}

int32_t Player::GetPlaybackSpeed(float &speed)
{
    MEDIA_INFO_LOG("process in");
    CHK_NULL_RETURN(player_);
    return player_->GetPlaybackSpeed(speed);
}

int32_t Player::SetAudioStreamType(int32_t type)
{
    MEDIA_INFO_LOG("process in");
    CHK_NULL_RETURN(player_);
    return player_->SetAudioStreamType(type);
}

void Player::GetAudioStreamType(int32_t &type)
{
    MEDIA_INFO_LOG("process in");
    if (player_ == nullptr) {
        MEDIA_ERR_LOG("player_ null");
        return;
    }
    player_->GetAudioStreamType(type);
}

int32_t Player::SetParameter(const Format &params)
{
    MEDIA_INFO_LOG("process in");
    CHK_NULL_RETURN(player_);
    return player_->SetParameter(params);
}
}  // namespace Media
}  // namespace OHOS
