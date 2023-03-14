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

#include "core/gestures/sequenced_recognizer.h"

#include <vector>

#include "base/thread/task_executor.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

namespace {
constexpr int32_t SEQUENCE_GESTURE_TIMEOUT = 300;
}

void SequencedRecognizer::OnAccepted()
{
    LOGD("the sequenced gesture recognizer has been accepted.");
    if (activeIndex >= recognizers_.size()) {
        return;
    }

    for (size_t touchId : refereePointers_) {
        recognizers_[activeIndex]->SetCoordinateOffset(coordinateOffset_);
        recognizers_[activeIndex]->OnAccepted(touchId);
    }
    recognizers_[activeIndex]->SetRefereeState(RefereeState::SUCCEED);
}

void SequencedRecognizer::OnRejected()
{
    LOGD("the sequenced gesture recognizer has been rejected!");
    if (activeIndex >= recognizers_.size()) {
        return;
    }

    for (size_t touchId : refereePointers_) {
        recognizers_[activeIndex]->OnRejected(touchId);
    }
    recognizers_[activeIndex]->SetRefereeState(RefereeState::FAIL);

    if (activeIndex != 0) {
        SendCancelMsg();
    }

    Reset();
}

void SequencedRecognizer::OnPending(size_t touchId)
{
    LOGD("the sequenced gesture recognizer is in pending! the touch id is %{public}zu", touchId);
    if (activeIndex >= recognizers_.size()) {
        return;
    }
    if (recognizers_[activeIndex]->GetDetectState() == DetectState::DETECTED) {
        recognizers_[activeIndex]->OnAccepted(touchId);
        recognizers_[activeIndex]->SetRefereeState(RefereeState::SUCCEED);
    } else {
        recognizers_[activeIndex]->OnPending(touchId);
    }
}

bool SequencedRecognizer::HandleEvent(const TouchEvent& point)
{
    RefPtr<GestureRecognizer> curRecognizer = recognizers_[activeIndex];
    LOGD("dispatch to the %{public}zu gesture recognizer, event type is %{public}zu", activeIndex, point.type);
    switch (point.type) {
        case TouchType::MOVE:
            curPoints_[point.id] = point;
            curRecognizer->HandleEvent(point);
            break;
        case TouchType::DOWN:
            if (curPoints_.empty()) {
                deadlineTimer_.Cancel();
            }
            curPoints_[point.id] = point;
            curRecognizer->HandleEvent(point);
            break;
        case TouchType::UP:
            curPoints_.erase(point.id);
            curRecognizer->HandleEvent(point);
            break;
        case TouchType::CANCEL:
            curPoints_.erase(point.id);
            curRecognizer->HandleEvent(point);
            break;
        default:
            LOGW("unknown touch type");
            break;
    }

    DetectState currState = curRecognizer->GetDetectState();
    if (curRecognizer->GetRefereeState() == RefereeState::SUCCEED && currState != DetectState::DETECTED) {
        // The current recognizer has already been detected, but the touch event destroy the state.
        // Should pass the touch point to the next recognizer.
        activeIndex++;
        if (activeIndex >= recognizers_.size()) {
            LOGD("gesture reset to the initial state ");
            Reset();
            return true;
        }

        if (curPoints_.empty()) {
            DeadlineTimer();
        } else {
            auto iter = curPoints_.begin();
            while (iter != curPoints_.end()) {
                iter->second.type = TouchType::DOWN;
                recognizers_[activeIndex]->HandleEvent(iter->second);
                iter++;
            }
        }
    }
    return true;
}

void SequencedRecognizer::BatchAdjudicate(
    const std::set<size_t>& touchIds, const RefPtr<GestureRecognizer>& recognizer, GestureDisposal disposal)
{
    if (disposal == GestureDisposal::ACCEPT) {
        LOGD("the sub recognizer %{public}s ask for accept", AceType::TypeName(recognizer));
        if (activeIndex == (recognizers_.size() - 1)) {
            state_ = DetectState::DETECTED;
            Adjudicate(AceType::Claim(this), GestureDisposal::ACCEPT);
        } else {
            Adjudicate(AceType::Claim(this), GestureDisposal::PENDING);
        }
    } else if (disposal == GestureDisposal::REJECT) {
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
    } else {
        Adjudicate(AceType::Claim(this), GestureDisposal::PENDING);
    }
}

void SequencedRecognizer::AddToReferee(size_t touchId, const RefPtr<GestureRecognizer>& recognizer)
{
    if (state_ == DetectState::READY) {
        state_ = DetectState::DETECTING;
    }

    recognizer->SetRefereeState(RefereeState::DETECTING);
    MultiFingersRecognizer::AddToReferee(touchId, AceType::Claim(this));
}

void SequencedRecognizer::Reset()
{
    activeIndex = 0;
    state_ = DetectState::READY;

    for (auto& recognizer : recognizers_) {
        recognizer->SetRefereeState(RefereeState::DETECTING);
    }
    curPoints_.clear();
}

void SequencedRecognizer::DeadlineTimer()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("fail to detect tap gesture due to context is nullptr");
        return;
    }
    auto&& callback = [weakPtr = AceType::WeakClaim(this)]() {
        auto refPtr = weakPtr.Upgrade();
        if (refPtr) {
            refPtr->HandleOverdueDeadline();
        } else {
            LOGE("fail to handle overdue deadline due to context is nullptr");
        }
    };

    deadlineTimer_.Reset(callback);
    auto taskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
    taskExecutor.PostDelayedTask(deadlineTimer_, SEQUENCE_GESTURE_TIMEOUT);
}

void SequencedRecognizer::HandleOverdueDeadline()
{
    LOGI("sequence gesture recognizer does not receive touch down in time");
    Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
}

bool SequencedRecognizer::ReconcileFrom(const RefPtr<GestureRecognizer>& recognizer)
{
    RefPtr<SequencedRecognizer> curr = AceType::DynamicCast<SequencedRecognizer>(recognizer);
    if (!curr) {
        Reset();
        return false;
    }

    if (recognizers_.size() != curr->recognizers_.size() || priorityMask_ != curr->priorityMask_) {
        Reset();
        return false;
    }

    for (size_t i = 0; i < recognizers_.size(); i++) {
        if (!recognizers_[i]->ReconcileFrom(curr->recognizers_[i])) {
            Reset();
            return false;
        }
    }

    context_ = curr->context_;
    onActionCancel_ = std::move(curr->onActionCancel_);

    return true;
}

} // namespace OHOS::Ace