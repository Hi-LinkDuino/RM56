/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "core/components/video/resource/player.h"

#include <iomanip>
#include <sstream>

#include "base/log/log.h"

namespace OHOS::Ace {

const char PLAYER_PARAM_WIDTH[] = "width";
const char PLAYER_PARAM_HEIGHT[] = "height";
const char PLAYER_PARAM_DURATION[] = "duration";
const char PLAYER_PARAM_CURRENTPOS[] = "currentpos";
const char PLAYER_PARAM_ISPLAYING[] = "isplaying";
const char PLAYER_PARAM_SRC[] = "src";
const char PLAYER_PARAM_ISAUTOPLAY[] = "autoplay";
const char PLAYER_PARAM_ISMUTE[] = "mute";
const char PLAYER_PARAM_TEXTURE[] = "texture";
const char PLAYER_PARAM_NEEDFRESHFORCE[] = "needRefreshForce";
const char PLAYER_PARAM_LOOP[] = "loop";

const char PLAYER_METHOD_INIT[] = "init";
const char PLAYER_METHOD_GETPOSITION[] = "getposition";
const char PLAYER_METHOD_START[] = "start";
const char PLAYER_METHOD_PAUSE[] = "pause";
const char PLAYER_METHOD_SEEKTO[] = "seekto";
const char PLAYER_METHOD_STOP[] = "stop";
const char PLAYER_METHOD_SETVOLUME[] = "setvolume";
const char PLAYER_METHOD_FULLSCREEN[] = "fullscreen";
const char PLAYER_METHOD_ENABLE_LOOPING[] = "enablelooping";
const char PLAYER_METHOD_SETSPEED[] = "setspeed";
const char PLAYER_METHOD_SETDIRECTION[] = "setdirection";
const char PLAYER_METHOD_SETLANDSCAPE[] = "setlandscape";

const char PLAYER_EVENT_PREPARED[] = "prepared";
const char PLAYER_EVENT_COMPLETION[] = "completion";
const char PLAYER_EVENT_SEEKCOMPLETE[] = "seekcomplete";
const char PLAYER_EVENT_ONPLAYSTATUS[] = "onplaystatus";
const char PLAYER_EVENT_ONGETCURRENTTIME[] = "ongetcurrenttime";

const char PLAYER_ERROR_CODE_CREATEFAIL[] = "error_video_000001";
const char PLAYER_ERROR_MSG_CREATEFAIL[] = "Create player failed.";
const char PLAYER_ERROR_CODE_FILEINVALID[] = "error_video_000002";
const char PLAYER_ERROR_MSG_FILEINVALID[] = "File invalid.";

void Player::Create(const std::function<void(int64_t)>& onCreate)
{
    scheduler_ = SchedulerBuilder::Build(
        [weak = WeakClaim(this)](uint64_t timestamp) {
            auto player = weak.Upgrade();
            if (player) {
                player->OnTick(timestamp);
            }
        },
        context_);
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context to create player");
        return;
    }

    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);

    platformTaskExecutor.PostTask([weak = WeakClaim(this), onCreate] {
        auto player = weak.Upgrade();
        if (player) {
            player->CreatePlayer(onCreate);
        }
    });
}

void Player::CreatePlayer(const std::function<void(int64_t)>& onCreate)
{
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    auto resRegister = context->GetPlatformResRegister();
    if (resRegister == nullptr) {
        return;
    }
    std::stringstream paramStream;
    paramStream << PLAYER_PARAM_TEXTURE << PARAM_EQUALS << textureId_;
    std::string param = paramStream.str();
    id_ = resRegister->CreateResource(type_, param);
    if (id_ == INVALID_ID) {
        if (onError_) {
            onError_(PLAYER_ERROR_CODE_CREATEFAIL, PLAYER_ERROR_MSG_CREATEFAIL);
        }
        return;
    }
    hash_ = MakeResourceHash();
    resRegister->RegisterEvent(
        MakeEventHash(PLAYER_EVENT_PREPARED), [weak = WeakClaim(this)](const std::string& param) {
            auto player = weak.Upgrade();
            if (player) {
                player->OnPrepared(param);
            }
        });
    resRegister->RegisterEvent(
        MakeEventHash(PLAYER_EVENT_COMPLETION), [weak = WeakClaim(this)](const std::string& param) {
            auto player = weak.Upgrade();
            if (player) {
                player->OnCompletion(param);
            }
        });
    resRegister->RegisterEvent(
        MakeEventHash(PLAYER_EVENT_SEEKCOMPLETE), [weak = WeakClaim(this)](const std::string& param) {
            auto player = weak.Upgrade();
            if (player) {
                player->OnSeekComplete(param);
            }
        });
    resRegister->RegisterEvent(
        MakeEventHash(PLAYER_EVENT_ONPLAYSTATUS), [weak = WeakClaim(this)](const std::string& param) {
            auto player = weak.Upgrade();
            if (player) {
                player->OnPlayStatus(param);
            }
        });
    resRegister->RegisterEvent(
        MakeEventHash(PLAYER_EVENT_ONGETCURRENTTIME), [weak = WeakClaim(this)](const std::string& param) {
            auto player = weak.Upgrade();
            if (player) {
                player->OnTimeGetted(param);
            }
        });
    if (onCreate) {
        onCreate(id_);
    }
    InitPlay();
}

void Player::InitPlay()
{
    std::stringstream paramStream;
    paramStream << PLAYER_PARAM_SRC << PARAM_EQUALS << src_ << PARAM_AND << PLAYER_PARAM_ISMUTE << PARAM_EQUALS
                << (isMute_ ? 1 : 0) << PARAM_AND << PLAYER_PARAM_ISAUTOPLAY << PARAM_EQUALS << (isAutoPlay_ ? 1 : 0);

    std::string param = paramStream.str();
    CallResRegisterMethod(MakeMethodHash(PLAYER_METHOD_INIT), param, [weak = WeakClaim(this)](std::string& result) {
        auto player = weak.Upgrade();
        if (player) {
            if (!player->IsResultSuccess(result)) {
                player->OnError(PLAYER_ERROR_CODE_FILEINVALID, PLAYER_ERROR_MSG_FILEINVALID);
            }
        }
    });
}

void Player::OnPrepared(const std::string& param)
{
    currentPos_ = 0;
    width_ = GetIntParam(param, PLAYER_PARAM_WIDTH);
    height_ = GetIntParam(param, PLAYER_PARAM_HEIGHT);
    duration_ = GetIntParam(param, PLAYER_PARAM_DURATION) / 1000;
    isPlaying_ = GetIntParam(param, PLAYER_PARAM_ISPLAYING) == 1;
    isNeedFreshForce_ = GetIntParam(param, PLAYER_PARAM_NEEDFRESHFORCE) == 1;
    isPrepared_ = true;
    if (width_ == 0 || height_ == 0 || duration_ == 0) {
        if (onError_) {
            onError_(PLAYER_ERROR_CODE_FILEINVALID, PLAYER_ERROR_MSG_FILEINVALID);
        }
        return;
    }
    getCurrentPosMethod_ = MakeMethodHash(PLAYER_METHOD_GETPOSITION);
    playMethod_ = MakeMethodHash(PLAYER_METHOD_START);
    pauseMethod_ = MakeMethodHash(PLAYER_METHOD_PAUSE);
    seekMethod_ = MakeMethodHash(PLAYER_METHOD_SEEKTO);
    stopMethod_ = MakeMethodHash(PLAYER_METHOD_STOP);
    setVolumeMethod_ = MakeMethodHash(PLAYER_METHOD_SETVOLUME);
    fullscreenMethod_ = MakeMethodHash(PLAYER_METHOD_FULLSCREEN);
    enableloopingMethod_ = MakeMethodHash(PLAYER_METHOD_ENABLE_LOOPING);
    setSpeedMethod_ = MakeMethodHash(PLAYER_METHOD_SETSPEED);
    setDirectionMethod_ = MakeMethodHash(PLAYER_METHOD_SETDIRECTION);
    setLandsacpeMethod_ = MakeMethodHash(PLAYER_METHOD_SETLANDSCAPE);

    if (isPlaying_) {
        SetTickActive(true);
    }

    for (const auto& listener : onPreparedListener_) {
        listener(width_, height_, isPlaying_, duration_, currentPos_, true);
    }
}

void Player::OnCompletion(const std::string& param)
{
    isPlaying_ = false;
    currentPos_ = duration_;
    SetTickActive(isPlaying_);
    for (const auto& listener : onCompletionListener_) {
        listener();
    }
}

void Player::OnSeekComplete(const std::string& param)
{
    currentPos_ = GetIntParam(param, PLAYER_PARAM_CURRENTPOS);
    if (!onCurrentPosListener_.empty()) {
        onCurrentPosListener_.back()(currentPos_);
    }
}

void Player::OnPlayStatus(const std::string& param)
{
    isPlaying_ = GetIntParam(param, PLAYER_PARAM_ISPLAYING) == 1;
    for (const auto& listener : onPlayStatusListener_) {
        listener(isPlaying_);
    }
}

void Player::SetTickActive(bool isActive)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context to set tick active");
        return;
    }

    auto uiTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
    uiTaskExecutor.PostSyncTask([this, isActive] {
        if (scheduler_) {
            if (isActive) {
                scheduler_->Start();
            } else {
                scheduler_->Stop();
            }
        }
    });
}

void Player::OnTick(uint64_t timeStamp)
{
    timeStamp_ += timeStamp;
    if (isNeedFreshForce_) {
        if (!onRefreshRenderListener_.empty()) {
            onRefreshRenderListener_.back()();
        }
    }
    if (timeStamp_ > FREAUENCY_GET_CURRENT_TIME) {
        GetCurrentTime();
        timeStamp_ -= FREAUENCY_GET_CURRENT_TIME;
    }
}

void Player::GetCurrentTime()
{
    CallResRegisterMethod(getCurrentPosMethod_, PARAM_NONE);
}

void Player::OnTimeGetted(const std::string& result)
{
    currentPos_ = GetIntParam(result, PLAYER_PARAM_CURRENTPOS);
    if (!onCurrentPosListener_.empty()) {
        onCurrentPosListener_.back()(currentPos_);
    }
}

void Player::Start()
{
    CallResRegisterMethod(playMethod_, PARAM_NONE, [weak = WeakClaim(this)](std::string& result) {
        auto player = weak.Upgrade();
        if (player) {
            player->OnStarted();
        }
    });
}

void Player::OnStarted()
{
    isPlaying_ = true;
    SetTickActive(isPlaying_);

    for (const auto& listener : onPlayStatusListener_) {
        listener(isPlaying_);
    }
}

void Player::Pause()
{
    CallResRegisterMethod(pauseMethod_, PARAM_NONE, [weak = WeakClaim(this)](std::string& result) {
        auto player = weak.Upgrade();
        if (player) {
            player->OnPaused();
        }
    });
}

void Player::OnPaused()
{
    isPlaying_ = false;
    SetTickActive(isPlaying_);

    for (const auto& listener : onPlayStatusListener_) {
        listener(isPlaying_);
    }
}

void Player::Stop()
{
    // The destructor will be executed at platform thread, so destroy scheduler at here.
    if (scheduler_) {
        scheduler_->Stop();
        scheduler_.Reset();
    }

    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context");
        return;
    }
    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);
    if (platformTaskExecutor.IsRunOnCurrentThread()) {
        UnregisterEvent();
    } else {
        platformTaskExecutor.PostTask([weak = WeakClaim(this)] {
            auto player = weak.Upgrade();
            if (player) {
                player->UnregisterEvent();
            }
        });
    }
}

void Player::UnregisterEvent()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context");
        return;
    }
    auto resRegister = context->GetPlatformResRegister();
    if (resRegister == nullptr) {
        return;
    }
    resRegister->UnregisterEvent(MakeEventHash(PLAYER_EVENT_PREPARED));
    resRegister->UnregisterEvent(MakeEventHash(PLAYER_EVENT_COMPLETION));
    resRegister->UnregisterEvent(MakeEventHash(PLAYER_EVENT_SEEKCOMPLETE));
    resRegister->UnregisterEvent(MakeEventHash(PLAYER_EVENT_ONPLAYSTATUS));
}

void Player::EnterFullScreen()
{
    CallResRegisterMethod(fullscreenMethod_, PARAM_NONE);
}

void Player::SeekTo(uint32_t pos)
{
    std::stringstream paramStream;
    paramStream << PARAM_VALUE << PARAM_EQUALS << pos;

    std::string param = paramStream.str();
    CallResRegisterMethod(seekMethod_, param);
}

void Player::SetVolume(float volume)
{
    std::stringstream paramStream;
    paramStream << PARAM_VALUE << PARAM_EQUALS << volume;

    std::string param = paramStream.str();
    CallResRegisterMethod(setVolumeMethod_, param);
}

void Player::AddPreparedListener(PreparedListener&& listener)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context");
        return;
    }

    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);
    platformTaskExecutor.PostTask([weak = WeakClaim(this), listener = std::move(listener)]() mutable {
        auto player = weak.Upgrade();
        if (player) {
            player->OnAddPreparedListener(std::move(listener));
        }
    });
}

void Player::OnAddPreparedListener(PreparedListener&& listener)
{
    onPreparedListener_.push_back(std::move(listener));
    if (isPrepared_) {
        onPreparedListener_.back()(width_, height_, isPlaying_, duration_, currentPos_, false);
    }
}

void Player::AddPlayStatusListener(PlayStatusListener&& listener)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context");
        return;
    }

    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);
    platformTaskExecutor.PostTask([weak = WeakClaim(this), listener = std::move(listener)]() mutable {
        auto player = weak.Upgrade();
        if (player) {
            player->OnAddPlayStatusListener(std::move(listener));
        }
    });
}

void Player::OnAddPlayStatusListener(PlayStatusListener&& listener)
{
    onPlayStatusListener_.push_back(std::move(listener));
}

void Player::AddCurrentPosListener(CurrentPosListener&& listener)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context");
        return;
    }

    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);
    platformTaskExecutor.PostTask([weak = WeakClaim(this), listener = std::move(listener)]() mutable {
        auto player = weak.Upgrade();
        if (player) {
            player->OnAddCurrentPosListener(std::move(listener));
        }
    });
}

void Player::OnAddCurrentPosListener(CurrentPosListener&& listener)
{
    onCurrentPosListener_.push_back(std::move(listener));
}

void Player::AddCompletionListener(CompletionListener&& listener)
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context");
        return;
    }

    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);
    platformTaskExecutor.PostTask([weak = WeakClaim(this), listener = std::move(listener)]() mutable {
        auto player = weak.Upgrade();
        if (player) {
            player->OnAddCompletionListener(std::move(listener));
        }
    });
}

void Player::OnAddCompletionListener(CompletionListener&& listener)
{
    onCompletionListener_.push_back(std::move(listener));
}

void Player::PopListener()
{
    onRefreshRenderListener_.pop_back();
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to get context");
        return;
    }

    auto platformTaskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::PLATFORM);
    platformTaskExecutor.PostTask([weak = WeakClaim(this)] {
        auto player = weak.Upgrade();
        if (player) {
            player->OnPopListener();
        }
    });
}

void Player::AddRefreshRenderListener(RefreshRenderListener&& listener)
{
    onRefreshRenderListener_.push_back(std::move(listener));
}

void Player::EnableLooping(bool loop)
{
    std::stringstream paramStream;
    paramStream << PLAYER_PARAM_LOOP << PARAM_EQUALS << (loop ? "1" : "0");

    std::string param = paramStream.str();
    CallResRegisterMethod(enableloopingMethod_, param);
}

void Player::SetSpeed(float speed)
{
    std::stringstream paramStream;
    paramStream << PARAM_VALUE << PARAM_EQUALS << speed;

    std::string param = paramStream.str();
    CallResRegisterMethod(setSpeedMethod_, param);
}

void Player::SetDirection(std::string& direction)
{
    std::stringstream paramStream;
    paramStream << PARAM_VALUE << PARAM_EQUALS << direction;

    std::string param = paramStream.str();
    CallResRegisterMethod(setDirectionMethod_, param);
}

void Player::SetLandscape()
{
    CallResRegisterMethod(setLandsacpeMethod_, PARAM_NONE);
}

void Player::OnPopListener()
{
    onPreparedListener_.pop_back();
    onPlayStatusListener_.pop_back();
    onCurrentPosListener_.pop_back();
    onCompletionListener_.pop_back();
    if (!onCurrentPosListener_.empty()) {
        onCurrentPosListener_.back()(currentPos_);
    }
}

} // namespace OHOS::Ace