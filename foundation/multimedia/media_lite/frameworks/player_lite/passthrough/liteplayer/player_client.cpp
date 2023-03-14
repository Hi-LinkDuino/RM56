/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "player_impl.h"

namespace OHOS {
namespace Media {
Player::PlayerClient::PlayerClient()
    : impl_(new (std::nothrow) PlayerImpl())
{
    MEDIA_INFO_LOG("PlayerClient process");
}

Player::PlayerClient::~PlayerClient()
{
    MEDIA_INFO_LOG("PlayerClient out");
}


int32_t Player::PlayerClient::SetSource(const Source &source)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return impl_->SetSource(source);
}

int32_t Player::PlayerClient::Prepare()
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return impl_->Prepare();
}

int32_t Player::PlayerClient::Play()
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return impl_->Play();
}

bool Player::PlayerClient::IsPlaying()
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("ptr null");
        return false;
    }
    return impl_->IsPlaying();
}

int32_t Player::PlayerClient::Pause()
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return impl_->Pause();
}

int32_t Player::PlayerClient::Stop()
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return impl_->Stop();
}
int32_t Player::PlayerClient::Rewind(int64_t mSeconds, int32_t mode)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return impl_->Rewind(mSeconds, mode);
}

int32_t Player::PlayerClient::SetVolume(float leftVolume, float rightVolume)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return impl_->SetVolume(leftVolume, rightVolume);
}

int32_t Player::PlayerClient::SetSurface(Surface *surface)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return impl_->SetSurface(surface);
}

int32_t Player::PlayerClient::SetLoop(bool loop)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return impl_->SetLoop(loop);
}

bool Player::PlayerClient::IsSingleLooping()
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return false;
    }
    return impl_->IsSingleLooping();
}

int32_t Player::PlayerClient::GetCurrentPosition(int64_t &time) const
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return impl_->GetCurrentPosition(time);
}

int32_t Player::PlayerClient::GetDuration(int64_t &duration) const
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return impl_->GetDuration(duration);
}
int32_t Player::PlayerClient::GetVideoWidth(int32_t &videoWidth)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return impl_->GetVideoWidth(videoWidth);
}

int32_t Player::PlayerClient::GetVideoHeight(int32_t &videoHeight)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return impl_->GetVideoHeight(videoHeight);
}

int32_t Player::PlayerClient::Reset()
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return impl_->Reset();
}

int32_t Player::PlayerClient::Release()
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return  impl_->Release();
}

void Player::PlayerClient::SetPlayerCallback(const std::shared_ptr<PlayerCallback> &cb)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return;
    }
    impl_->SetPlayerCallback(cb);
}

int32_t Player::PlayerClient::GetPlayerState(int32_t &state) const
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return PLAYER_STATE_ERROR;
    }
    return impl_->GetPlayerState(state);
}

int32_t Player::PlayerClient::SetPlaybackSpeed(float speed)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return impl_->SetPlaybackSpeed(speed);
}

int32_t Player::PlayerClient::GetPlaybackSpeed(float &speed)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return impl_->GetPlaybackSpeed(speed);
}

int32_t Player::PlayerClient::SetParameter(const Format &params)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return impl_->SetParameter(params);
}

int32_t Player::PlayerClient::SetAudioStreamType(int32_t type)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return -1;
    }
    return impl_->SetAudioStreamType(type);
}

void Player::PlayerClient::GetAudioStreamType(int32_t &type)
{
    if (impl_ == nullptr) {
        MEDIA_ERR_LOG("impl_ null");
        return ;
    }
    impl_->GetAudioStreamType(type);
}
}
}
