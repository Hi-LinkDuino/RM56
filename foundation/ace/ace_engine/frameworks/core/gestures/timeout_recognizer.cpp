/*
* Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "core/gestures/timeout_recognizer.h"

#include <unordered_set>

namespace OHOS::Ace {
SingleChildGestureRecognizer::SingleChildGestureRecognizer(WeakPtr<PipelineContext> context)
    : context_(std::move(context))
{
}

SingleChildGestureRecognizer::SingleChildGestureRecognizer(WeakPtr<PipelineContext> context,
    RefPtr<GestureRecognizer> child)
    : child_(std::move(child)),
    context_(std::move(context))
{
    child_->SetGestureGroup(AceType::WeakClaim(this));
    child_->SetIsExternalGesture(true);
}

SingleChildGestureRecognizer::~SingleChildGestureRecognizer()
{
}

void SingleChildGestureRecognizer::SetChild(RefPtr<GestureRecognizer> recognizer)
{
    child_ = recognizer;
    child_->SetGestureGroup(AceType::WeakClaim(this));
    child_->SetIsExternalGesture(true);
}

bool SingleChildGestureRecognizer::ReconcileFrom(const RefPtr<GestureRecognizer>& recognizer)
{
    auto rhs = AceType::DynamicCast<SingleChildGestureRecognizer>(recognizer);
    if (!rhs) {
        return false;
    }

    if (child_ == nullptr || rhs->child_ == nullptr) {
        return false;
    }

    if (!child_->ReconcileFrom(rhs->GetChild())) {
        return false;
    }

    return MultiFingersRecognizer::ReconcileFrom(recognizer);
}

void SingleChildGestureRecognizer::AddToReferee(size_t touchId, const RefPtr<GestureRecognizer>& recognizer)
{
    MultiFingersRecognizer::AddToReferee(touchId, AceType::Claim(this));
}

void SingleChildGestureRecognizer::DelFromReferee(size_t touchId, const RefPtr<GestureRecognizer>& recognizer)
{
    MultiFingersRecognizer::DelFromReferee(touchId, AceType::Claim(this));
}

RefPtr<GestureRecognizer> SingleChildGestureRecognizer::GetChild() const
{
    return child_;
}

TimeoutRecognizer::TimeoutRecognizer(WeakPtr<PipelineContext> context,
    std::chrono::duration<float> timeout)
    : SingleChildGestureRecognizer(context),
    context_(context),
    timeout_(timeout)
{
}

TimeoutRecognizer::TimeoutRecognizer(WeakPtr<PipelineContext> context,
    RefPtr<GestureRecognizer> child, std::chrono::duration<float> timeout)
    : SingleChildGestureRecognizer(context, std::move(child)),
    context_(context),
    timeout_(timeout)
{
}

void TimeoutRecognizer::OnAccepted(size_t touchId)
{
    start_.reset();
    if (GetChild()->GetDetectState() == DetectState::DETECTED) {
        GetChild()->SetCoordinateOffset(coordinateOffset_);
        GetChild()->OnAccepted(touchId);
    }

    SingleChildGestureRecognizer::OnAccepted(touchId);
}

void TimeoutRecognizer::OnRejected(size_t touchId)
{
    GetChild()->OnRejected(touchId);

    SingleChildGestureRecognizer::OnRejected(touchId);
}

bool TimeoutRecognizer::ReconcileFrom(const RefPtr<GestureRecognizer>& recognizer)
{
    auto rhs = AceType::DynamicCast<TimeoutRecognizer>(recognizer);
    if (!rhs) {
        return false;
    }

    timeout_ = rhs->timeout_;

    return SingleChildGestureRecognizer::ReconcileFrom(recognizer);
}

void TimeoutRecognizer::BatchAdjudicate(const std::set<size_t>& touchIds,
    const RefPtr<GestureRecognizer>& recognizer, GestureDisposal disposal)
{
    if (disposal == GestureDisposal::ACCEPT) {
        start_.reset();
        touchIds_.clear();
        GetChild()->SetRefereeState(RefereeState::SUCCEED);
        for (const auto& touchId : touchIds) {
            GetChild()->OnAccepted(touchId);
        }

        state_ = DetectState::DETECTED;
        Adjudicate(AceType::Claim(this), GestureDisposal::ACCEPT);
    } else if (disposal == GestureDisposal::REJECT) {
        touchIds_.clear();
        start_.reset();

        GetChild()->SetRefereeState(RefereeState::FAIL);
        for (const auto& touchId : touchIds) {
            GetChild()->OnRejected(touchId);
        }

        state_ = DetectState::READY;
        GetChild()->SetRefereeState(RefereeState::DETECTING);

        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
    } else if (disposal == GestureDisposal::PENDING) {
        touchIds_.clear();
        start_.reset();

        GetChild()->SetRefereeState(RefereeState::PENDING);
        Adjudicate(AceType::Claim(this), GestureDisposal::PENDING);
    }
}

void TimeoutRecognizer::HandleTouchDownEvent(const TouchEvent& event)
{
    if (!start_) {
        start_ = event.time;
        touchIds_.insert(event.id);
        GetChild()->HandleEvent(event);
        return;
    }

    if (!CheckTimeout(event.time)) {
        return;
    }
}

void TimeoutRecognizer::HandleTouchUpEvent(const TouchEvent& event)
{
    if (!CheckTimeout(event.time)) {
        return;
    }

    GetChild()->HandleEvent(event);
}

void TimeoutRecognizer::HandleTouchMoveEvent(const TouchEvent& event)
{
    if (!CheckTimeout(event.time)) {
        return;
    }

    GetChild()->HandleEvent(event);
}

void TimeoutRecognizer::HandleTouchCancelEvent(const TouchEvent& event)
{
    if (!CheckTimeout(event.time)) {
        return;
    }

    GetChild()->HandleEvent(event);
}

bool TimeoutRecognizer::CheckTimeout(TimeStamp time)
{
    if (start_ && time > (*start_ + timeout_)) {
        start_.reset();

        std::set<size_t> ids;
        std::swap(ids, touchIds_);
        GetChild()->SetRefereeState(RefereeState::FAIL);
        for (auto const& touchId : touchIds_) {
            GetChild()->OnRejected(touchId);
        }

        state_ = DetectState::READY;
        GetChild()->SetRefereeState(RefereeState::DETECTING);

        BatchAdjudicate(ids, GetChild(), GestureDisposal::REJECT);

        return false;
    }

    return true;
}
} // namespace OHOS::Ace
