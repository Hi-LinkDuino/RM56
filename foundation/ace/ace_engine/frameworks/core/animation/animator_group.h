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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATOR_GROUP_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATOR_GROUP_H

#include "core/animation/animator.h"
#include "core/animation/status_listener.h"

#include <map>
#include <set>

namespace OHOS::Ace {

class ACE_EXPORT AnimatorGroup : public AceType, public StatusListenable {
    DECLARE_ACE_TYPE(AnimatorGroup, AceType);
public:
    enum class AnimatorGroupStatus {
        RUNNING,
        STOPPED
    };
    AnimatorGroup() = default;
    ~AnimatorGroup();

    void AddAnimator(const RefPtr<Animator>& animator);
    void RemoveAnimator(const RefPtr<Animator>& animator);
    void Play();
    void Stop();
private:
    void OnAnimatorStop(const RefPtr<Animator>& animator);
    std::map<RefPtr<Animator>, BaseId::IdType> animators_;
    std::set<RefPtr<Animator>> runningAnimators_;
    AnimatorGroupStatus status_ = AnimatorGroupStatus::STOPPED;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_ANIMATOR_GROUP_H