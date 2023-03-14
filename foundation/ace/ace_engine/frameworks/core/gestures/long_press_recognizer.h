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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_LONG_PRESS_RECOGNIZER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_LONG_PRESS_RECOGNIZER_H

#include "base/thread/cancelable_callback.h"
#include "core/gestures/multi_fingers_recognizer.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

class LongPressInfo : public BaseEventInfo, public TouchLocationInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(LongPressInfo, BaseEventInfo, TouchLocationInfo);

public:
    explicit LongPressInfo(int32_t fingerId) : BaseEventInfo("onLongPress"), TouchLocationInfo(fingerId) {}
    ~LongPressInfo() override = default;
};

using OnLongPress = std::function<void(const LongPressInfo&)>;
using LongPressCallback = std::function<void(const GestureEvent&)>;
using LongPressNoParamCallback = std::function<void()>;

class LongPressRecognizer : public MultiFingersRecognizer {
    DECLARE_ACE_TYPE(LongPressRecognizer, MultiFingersRecognizer);

public:
    explicit LongPressRecognizer(const WeakPtr<PipelineContext>& context) : context_(context) {}
    LongPressRecognizer(const WeakPtr<PipelineContext>& context, int32_t duration, int32_t fingers, bool repeat)
        : context_(context), duration_(duration), repeat_(repeat)
    {
        fingers_ = fingers;
    }
    ~LongPressRecognizer() override = default;

    void OnAccepted() override;
    void OnRejected() override;

    void SetOnLongPress(const OnLongPress& onLongPress)
    {
        onLongPress_ = onLongPress;
    }

private:
    void HandleTouchDownEvent(const TouchEvent& event) override;
    void HandleTouchUpEvent(const TouchEvent& event) override;
    void HandleTouchMoveEvent(const TouchEvent& event) override;
    void HandleTouchCancelEvent(const TouchEvent& event) override;
    bool ReconcileFrom(const RefPtr<GestureRecognizer>& recognizer) override;
    void HandleOverdueDeadline();
    void DeadlineTimer(int32_t time);
    void DoRepeat();
    void StartRepeatTimer();
    void SendCallbackMsg(const std::unique_ptr<GestureEventFunc>& callback, bool isRepeat);
    void Reset();
    double ConvertPxToVp(double offset) const;

    WeakPtr<PipelineContext> context_;
    OnLongPress onLongPress_;
    CancelableCallback<void()> deadlineTimer_;
    CancelableCallback<void()> timer_;
    int32_t duration_ = 500;
    int32_t fingers_ = 1;
    bool repeat_ = false;
    std::map<int32_t, TouchEvent> touchMap_;
    int32_t pointsCount_ = 0;
    TimeStamp time_;
    bool pendingEnd_ = false;
    bool pendingCancel_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_LONG_PRESS_RECOGNIZER_H
