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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_EXCLUSIVE_RECOGNIZER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_EXCLUSIVE_RECOGNIZER_H

#include <functional>
#include <set>

#include "core/gestures/gesture_info.h"
#include "core/gestures/multi_fingers_recognizer.h"

namespace OHOS::Ace {

// ExclusiveRecognizer identifies gesture exclusive.
class ACE_EXPORT ExclusiveRecognizer : public MultiFingersRecognizer {
    DECLARE_ACE_TYPE(ExclusiveRecognizer, MultiFingersRecognizer);

public:
    explicit ExclusiveRecognizer(const std::vector<RefPtr<GestureRecognizer>>& recognizers)
    {
        for (auto& recognizer : recognizers) {
            recognizer->SetGestureGroup(AceType::WeakClaim(this));
        }
        recognizers_ = recognizers;
    }
    ~ExclusiveRecognizer() override = default;
    void OnAccepted(size_t touchId) override;
    void OnRejected(size_t touchId) override;
    void OnPending(size_t touchId) override;
    bool HandleEvent(const TouchEvent& point) override;

private:
    void HandleTouchDownEvent(const TouchEvent& event) override {};
    void HandleTouchUpEvent(const TouchEvent& event) override {};
    void HandleTouchMoveEvent(const TouchEvent& event) override {};
    void HandleTouchCancelEvent(const TouchEvent& event) override {};
    void BatchAdjudicate(const std::set<size_t>& touchIds, const RefPtr<GestureRecognizer>& recognizer,
        GestureDisposal disposal) override;
    void AddToReferee(size_t touchId, const RefPtr<GestureRecognizer>& recognizer) override;
    bool ReconcileFrom(const RefPtr<GestureRecognizer>& recognizer) override;
    bool CheckNeedBlocked(const RefPtr<GestureRecognizer>& recognizer);
    void AcceptSubGesture(const RefPtr<GestureRecognizer>& recognizer);
    bool CheckAllFailed();
    void Reset();

    std::vector<RefPtr<GestureRecognizer>> recognizers_;
    RefPtr<GestureRecognizer> activeRecognizer_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_EXCLUSIVE_RECOGNIZER_H
