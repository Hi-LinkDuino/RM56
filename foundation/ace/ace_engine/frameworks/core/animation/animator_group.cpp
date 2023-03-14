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

#include "core/animation/animator_group.h"

namespace OHOS::Ace {

AnimatorGroup::~AnimatorGroup()
{
    for (auto& info : animators_) {
        if (info.first) {
            info.first->RemoveStopListener(info.second);
        }
    }
    animators_.clear();
}

void AnimatorGroup::AddAnimator(const RefPtr<Animator>& animator)
{
    if (animator) {
        if (animators_.find(animator) == animators_.end()) {
            BaseId::IdType id = animator->AddStopListener([weak = WeakClaim(this), animator] {
                                    auto animatorGroup = weak.Upgrade();
                                    if (animatorGroup) {
                                        animatorGroup->OnAnimatorStop(animator);
                                    }
                                });
            animators_.emplace(animator, id);
        }
    }
}

void AnimatorGroup::RemoveAnimator(const RefPtr<Animator>& animator)
{
    if (animator) {
        auto iter = animators_.find(animator);
        if (iter != animators_.end()) {
            animator->RemoveStopListener(iter->second);
            animators_.erase(iter);
        }
    }
}

void AnimatorGroup::Play()
{
    if (status_ == AnimatorGroupStatus::RUNNING) {
        return;
    }
    status_ = AnimatorGroupStatus::RUNNING;
    for (auto& info : animators_) {
        if (info.first) {
            runningAnimators_.emplace(info.first);
            info.first->Play();
        }
    }
}

void AnimatorGroup::Stop()
{
    if (status_ == AnimatorGroupStatus::STOPPED) {
        return;
    }
    status_ = AnimatorGroupStatus::STOPPED;
    for (auto& info : animators_) {
        if (info.first) {
            info.first->Stop();
        }
    }
}

void AnimatorGroup::OnAnimatorStop(const RefPtr<Animator>& animator)
{
    runningAnimators_.erase(animator);
    if (runningAnimators_.empty()) {
        if (status_ == AnimatorGroupStatus::RUNNING) {
            status_ = AnimatorGroupStatus::STOPPED;
            StatusListenable::NotifyStopListener();
        }
    }
}

} // namespace OHOS::Ace