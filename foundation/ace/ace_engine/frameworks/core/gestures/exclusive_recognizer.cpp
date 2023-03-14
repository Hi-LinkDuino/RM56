/*
* Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/gestures/exclusive_recognizer.h"

#include <vector>

#include "base/geometry/offset.h"
#include "base/log/log.h"
#include "core/gestures/gesture_referee.h"

namespace OHOS::Ace {

void ExclusiveRecognizer::OnAccepted(size_t touchId)
{
    LOGD("the exclusive gesture recognizer has been accepted, touch id %{public}zu", touchId);
    if (refereePointers_.find(touchId) == refereePointers_.end()) {
        return;
    }

    refereePointers_.erase(touchId);

    if (!activeRecognizer_) {
        return;
    }

    activeRecognizer_->SetCoordinateOffset(coordinateOffset_);
    activeRecognizer_->OnAccepted(touchId);
    activeRecognizer_->SetRefereeState(RefereeState::SUCCEED);

    for (auto& recognizer : recognizers_) {
        if (recognizer != activeRecognizer_) {
            LOGD("the sub gesture %{public}s is rejected because one is accepted", AceType::TypeName(recognizer));
            recognizer->OnRejected(touchId);
            recognizer->SetRefereeState(RefereeState::FAIL);
        }
    }
}

void ExclusiveRecognizer::OnRejected(size_t touchId)
{
    if (refereePointers_.find(touchId) == refereePointers_.end()) {
        return;
    }

    refereePointers_.erase(touchId);

    LOGD("the exclusive gesture recognizer has been rejected! the touch id is %{public}zu", touchId);
    for (auto& recognizer : recognizers_) {
        if (recognizer->GetRefereeState() == RefereeState::FAIL) {
            continue;
        }

        recognizer->OnRejected(touchId);
        if (recognizer->GetDetectState() == DetectState::READY) {
            recognizer->SetRefereeState(RefereeState::FAIL);
        }
    }
}

void ExclusiveRecognizer::OnPending(size_t touchId)
{
    if (refereePointers_.find(touchId) == refereePointers_.end()) {
        return;
    }

    LOGD("the exclusive gesture recognizer is pending! the touch id is %{public}zu", touchId);
    for (auto& recognizer : recognizers_) {
        if (recognizer->GetRefereeState() == RefereeState::PENDING) {
            recognizer->OnPending(touchId);
        }
    }
}

bool ExclusiveRecognizer::HandleEvent(const TouchEvent& point)
{
    switch (point.type) {
        case TouchType::MOVE:
        case TouchType::DOWN:
        case TouchType::UP:
        case TouchType::CANCEL:
            if (activeRecognizer_) {
                activeRecognizer_->HandleEvent(point);
            } else {
                for (auto& recognizer : recognizers_) {
                    recognizer->HandleEvent(point);
                }
            }
            break;
        default:
            LOGW("exclusive recognizer received unknown touch type");
            break;
    }

    if (activeRecognizer_ && activeRecognizer_->GetDetectState() != DetectState::DETECTED) {
        LOGD("sub detected gesture has finished, change the exclusive recognizer to be ready");
        Reset();
    }

    return true;
}

void ExclusiveRecognizer::BatchAdjudicate(
    const std::set<size_t>& touchIds, const RefPtr<GestureRecognizer>& recognizer, GestureDisposal disposal)
{
    // any sub recognizer accept itself will drive to detected state
    if (state_ == DetectState::DETECTED) {
        LOGD("exclusive gesture recognizer is in detected state, do not process disposal");
        return;
    }

    if (disposal == GestureDisposal::ACCEPT) {
        AcceptSubGesture(recognizer);
        return;
    }

    if (disposal == GestureDisposal::PENDING) {
        if (CheckNeedBlocked(recognizer)) {
            recognizer->SetRefereeState(RefereeState::BLOCKED);
        } else {
            recognizer->SetRefereeState(RefereeState::PENDING);
            MultiFingersRecognizer::Adjudicate(AceType::Claim(this), GestureDisposal::PENDING);
        }
        return;
    }

    // process reject disposal
    LOGD("sub gesture recognizer %{public}s ask for reject", AceType::TypeName(recognizer));
    RefereeState prevState = recognizer->GetRefereeState();
    for (auto touchId : touchIds) {
        recognizer->OnRejected(touchId);
    }
    recognizer->SetRefereeState(RefereeState::FAIL);

    if (prevState == RefereeState::PENDING) {
        for (auto& tmpRecognizer : recognizers_) {
            if (tmpRecognizer->GetRefereeState() == RefereeState::BLOCKED &&
                tmpRecognizer->GetDetectState() == DetectState::DETECTED) {
                activeRecognizer_ = recognizer;
                state_ = DetectState::DETECTED;
                MultiFingersRecognizer::Adjudicate(AceType::Claim(this), GestureDisposal::ACCEPT);
                break;
            }
        }
    }

    if (CheckAllFailed()) {
        LOGD("all gesture recognizers are rejected, adjudicate reject");
        MultiFingersRecognizer::Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        Reset();
    }
}

void ExclusiveRecognizer::AddToReferee(size_t touchId, const RefPtr<GestureRecognizer>& recognizer)
{
    recognizer->SetRefereeState(RefereeState::DETECTING);

    if (state_ == DetectState::READY) {
        state_ = DetectState::DETECTING;
    }

    if (state_ == DetectState::DETECTING) {
        MultiFingersRecognizer::AddToReferee(touchId, AceType::Claim(this));
    }
}

bool ExclusiveRecognizer::CheckNeedBlocked(const RefPtr<GestureRecognizer>& recognizer)
{
    auto pendingMember = std::find_if(
        std::begin(recognizers_), std::end(recognizers_), [recognizer](const RefPtr<GestureRecognizer>& member) {
            return (member != recognizer) && (member->GetRefereeState() == RefereeState::PENDING);
        });
    if (pendingMember != recognizers_.end()) {
        LOGD("exclusive gesture recognizer detected pending gesture in members");
        return true;
    }

    return false;
}

void ExclusiveRecognizer::AcceptSubGesture(const RefPtr<GestureRecognizer>& recognizer)
{
    LOGD("sub gesture recognizer %{public}s ask for accept", AceType::TypeName(recognizer));
    if (CheckNeedBlocked(recognizer)) {
        recognizer->SetRefereeState(RefereeState::BLOCKED);
        return;
    }

    activeRecognizer_ = recognizer;
    state_ = DetectState::DETECTED;
    MultiFingersRecognizer::Adjudicate(AceType::Claim(this), GestureDisposal::ACCEPT);
}

bool ExclusiveRecognizer::CheckAllFailed()
{
    auto notFailMember = std::find_if(std::begin(recognizers_), std::end(recognizers_),
        [](const RefPtr<GestureRecognizer>& member) { return member->GetRefereeState() != RefereeState::FAIL; });

    return notFailMember == recognizers_.end();
}

void ExclusiveRecognizer::Reset()
{
    state_ = DetectState::READY;
    activeRecognizer_ = nullptr;
    for (auto& recognizer : recognizers_) {
        recognizer->SetRefereeState(RefereeState::DETECTING);
    }
}

bool ExclusiveRecognizer::ReconcileFrom(const RefPtr<GestureRecognizer>& recognizer)
{
    RefPtr<ExclusiveRecognizer> curr = AceType::DynamicCast<ExclusiveRecognizer>(recognizer);
    if (!curr) {
        Reset();
        return false;
    }

    if (recognizers_.size() != curr->recognizers_.size() || curr->priorityMask_ != priorityMask_) {
        Reset();
        return false;
    }

    for (size_t i = 0; i < recognizers_.size(); i++) {
        if (!recognizers_[i]->ReconcileFrom(curr->recognizers_[i])) {
            Reset();
            return false;
        }
    }

    return true;
}

} // namespace OHOS::Ace
