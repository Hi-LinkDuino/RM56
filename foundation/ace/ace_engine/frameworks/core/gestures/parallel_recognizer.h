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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_PARALLEL_RECOGNIZER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_PARALLEL_RECOGNIZER_H

#include <functional>

#include "core/gestures/gesture_info.h"
#include "core/gestures/multi_fingers_recognizer.h"

namespace OHOS::Ace {

// ParallelRecognizer identifies events in parallel recognizers.
// For long press and double click, see: LongPressRecognizer and DoubleClickRecognizer.
class ACE_EXPORT ParallelRecognizer : public MultiFingersRecognizer {
    DECLARE_ACE_TYPE(ParallelRecognizer, MultiFingersRecognizer);

public:
    explicit ParallelRecognizer(const std::vector<RefPtr<GestureRecognizer>>& recognizers)
    {
        for (auto& recognizer : recognizers) {
            recognizer->SetGestureGroup(AceType::WeakClaim(this));
        }
        recognizers_ = recognizers;
    }
    ~ParallelRecognizer() override = default;
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
    void Reset();
    void DelFromReferee(size_t touchId, const RefPtr<GestureRecognizer>& recognizer) override {}
    bool IsRecognizeEnd(const RefPtr<GestureRecognizer>& recognizer);

    std::vector<RefPtr<GestureRecognizer>> recognizers_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_PARALLEL_RECOGNIZER_H

