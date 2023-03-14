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

#include "animator/animator.h"

#include "animator/animator_manager.h"
#include "hal_tick.h"

namespace OHOS {
Animator::~Animator()
{
    AnimatorManager::GetInstance()->Remove(this);
}

void Animator::Start()
{
    SetState(START);
    runTime_ = 0;
    lastRunTime_ = HALTick::GetInstance().GetTime();
    AnimatorManager::GetInstance()->Add(this);
}

void Animator::Stop()
{
    SetState(STOP);
    if (callback_ != nullptr) {
        callback_->OnStop(*view_);
    }
    AnimatorManager::GetInstance()->Remove(this);
}

void Animator::Pause()
{
    SetState(PAUSE);
    AnimatorManager::GetInstance()->Remove(this);
}

void Animator::Resume()
{
    SetState(START);
    lastRunTime_ = HALTick::GetInstance().GetTime();
    AnimatorManager::GetInstance()->Add(this);
}

void Animator::Run()
{
    uint32_t elapse = HALTick::GetInstance().GetElapseTime(lastRunTime_);
    lastRunTime_ = HALTick::GetInstance().GetTime();
    runTime_ = (UINT32_MAX - elapse > runTime_) ? (runTime_ + elapse) : period_;

    if (!repeat_ && (runTime_ >= period_)) {
        runTime_ = period_;
        if (callback_ != nullptr) {
            callback_->Callback(view_);
        }
        Stop();
        return;
    }

    if (callback_ != nullptr) {
        callback_->Callback(view_);
    }
}
} // namespace OHOS
