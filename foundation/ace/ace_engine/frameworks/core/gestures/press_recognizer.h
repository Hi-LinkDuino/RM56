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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_PRESS_RECOGNIZER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_PRESS_RECOGNIZER_H

#include "base/thread/cancelable_callback.h"
#include "core/gestures/gesture_recognizer.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

class PressInfo : public BaseEventInfo, public TouchLocationInfo {
    DECLARE_RELATIONSHIP_OF_CLASSES(PressInfo, BaseEventInfo, TouchLocationInfo);

public:
    explicit PressInfo(int32_t fingerId) : BaseEventInfo("onPress"), TouchLocationInfo(fingerId) {}
    ~PressInfo() override = default;
};

using OnPress = std::function<void(const PressInfo&)>;
using OnPressCancel = std::function<void()>;

class PressRecognizer : public GestureRecognizer {
    DECLARE_ACE_TYPE(PressRecognizer, GestureRecognizer);

public:
    explicit PressRecognizer(const WeakPtr<PipelineContext>& context) : context_(context) {}
    ~PressRecognizer() override = default;

    void OnAccepted(size_t touchId) override;
    void OnRejected(size_t touchId) override;

    void SetOnPress(const OnPress& onPress)
    {
        onPress_ = onPress;
    }

    void SetOnPressCancel(const OnPressCancel& onPressCancel)
    {
        onPressCancel_ = onPressCancel;
    }

private:
    void HandleTouchDownEvent(const TouchEvent& event) override;
    void HandleTouchUpEvent(const TouchEvent& event) override;
    void HandleTouchMoveEvent(const TouchEvent& event) override;
    void HandleTouchCancelEvent(const TouchEvent& event) override;
    void HandleOverdueDeadline();

    WeakPtr<PipelineContext> context_;
    TouchEvent trackPoint_;
    OnPress onPress_;
    CancelableCallback<void()> deadlineTimer_;
    OnPressCancel onPressCancel_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_GESTURES_PRESS_RECOGNIZER_H
