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


#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_TIMEOUT_RECOGNIZER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_TIMEOUT_RECOGNIZER_H

#include "core/gestures/multi_fingers_recognizer.h"

#include <set>
#include <chrono>

namespace OHOS::Ace {
class SingleChildGestureRecognizer : public MultiFingersRecognizer {
    DECLARE_ACE_TYPE(SingleChildGestureRecognizer, GestureRecognizer);

public:
    explicit SingleChildGestureRecognizer(WeakPtr<PipelineContext> context);
    SingleChildGestureRecognizer(WeakPtr<PipelineContext> context, RefPtr<GestureRecognizer> child);
    ~SingleChildGestureRecognizer();

    void SetChild(RefPtr<GestureRecognizer> recognizer);
    bool ReconcileFrom(const RefPtr<GestureRecognizer>& recognizer) override;

    void AddToReferee(size_t touchId, const RefPtr<GestureRecognizer>& recognizer) override;
    void DelFromReferee(size_t touchId, const RefPtr<GestureRecognizer>& recognizer) override;

protected:
    RefPtr<GestureRecognizer> GetChild() const;

private:
    RefPtr<GestureRecognizer> child_;
    WeakPtr<PipelineContext> context_;
};

class TimeoutRecognizer : public SingleChildGestureRecognizer {
    DECLARE_ACE_TYPE(TimeoutRecognizer, SingleChildGestureRecognizer);
public:
    explicit TimeoutRecognizer(WeakPtr<PipelineContext> context, std::chrono::duration<float> timeout);
    TimeoutRecognizer(WeakPtr<PipelineContext> context, RefPtr<GestureRecognizer> child,
        std::chrono::duration<float> timeout);

    void OnAccepted(size_t touchId) override;

    void OnRejected(size_t touchId) override;

    bool ReconcileFrom(const RefPtr<GestureRecognizer>& recognizer) override;

    void BatchAdjudicate(
        const std::set<size_t>& touchIds,
        const RefPtr<GestureRecognizer>& recognizer,
        GestureDisposal disposal) override;

    void HandleTouchDownEvent(const TouchEvent& event) override;
    void HandleTouchUpEvent(const TouchEvent& event) override;
    void HandleTouchMoveEvent(const TouchEvent& event) override;
    void HandleTouchCancelEvent(const TouchEvent& event) override;

private:
    bool CheckTimeout(TimeStamp time);

private:
    WeakPtr<PipelineContext> context_;
    std::chrono::duration<float> timeout_;
    std::set<size_t> touchIds_;
    std::optional<TimeStamp> start_;
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_TIMEOUT_RECOGNIZER_H

