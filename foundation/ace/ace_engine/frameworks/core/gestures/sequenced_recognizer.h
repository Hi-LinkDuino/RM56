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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_SEQUENCED_RECOGNIZER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_SEQUENCED_RECOGNIZER_H

#include <functional>

#include "base/thread/cancelable_callback.h"
#include "core/gestures/multi_fingers_recognizer.h"

namespace OHOS::Ace {

// SequencedRecognizer identifies only single click events.
// For long press and double click, see: LongPressRecognizer and DoubleClickRecognizer.
class ACE_EXPORT SequencedRecognizer : public MultiFingersRecognizer {
    DECLARE_ACE_TYPE(SequencedRecognizer, MultiFingersRecognizer);

public:
    SequencedRecognizer(
        const WeakPtr<PipelineContext>& context, const std::vector<RefPtr<GestureRecognizer>>& recognizers)
    {
        for (auto& recognizer : recognizers) {
            recognizer->SetGestureGroup(AceType::WeakClaim(this));
        }
        recognizers_ = recognizers;
        context_ = context;
    }
    ~SequencedRecognizer() override = default;
    void OnAccepted() override;
    void OnRejected() override;
    bool HandleEvent(const TouchEvent& point) override;
    void OnPending(size_t touchId) override;

private:
    void HandleTouchDownEvent(const TouchEvent& event) override {};
    void HandleTouchUpEvent(const TouchEvent& event) override {};
    void HandleTouchMoveEvent(const TouchEvent& event) override {};
    void HandleTouchCancelEvent(const TouchEvent& event) override {};
    void BatchAdjudicate(const std::set<size_t>& touchIds, const RefPtr<GestureRecognizer>& recognizer,
        GestureDisposal disposal) override;
    void AddToReferee(size_t touchId, const RefPtr<GestureRecognizer>& recognizer) override;
    bool ReconcileFrom(const RefPtr<GestureRecognizer>& recognizer) override;
    void Reset();
    void DeadlineTimer();
    void HandleOverdueDeadline();

    std::vector<RefPtr<GestureRecognizer>> recognizers_;
    std::map<int32_t, TouchEvent> curPoints_;
    size_t activeIndex = 0;
    CancelableCallback<void()> deadlineTimer_;
    WeakPtr<PipelineContext> context_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_SEQUENCED_RECOGNIZER_H
