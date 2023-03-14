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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GESTURE_LISTENER_RENDER_GESTURE_LISTENER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GESTURE_LISTENER_RENDER_GESTURE_LISTENER_H

#include "core/components/proxy/render_proxy.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/drag_recognizer.h"
#include "core/gestures/long_press_recognizer.h"
#include "core/gestures/pinch_recognizer.h"

namespace OHOS::Ace {

class GestureListenerComponent;

class ACE_EXPORT RenderGestureListener final : public RenderProxy {
    DECLARE_ACE_TYPE(RenderGestureListener, RenderProxy);

public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;

    bool GetVisible() const override;

    void SetRemoteMessageCallback(const ClickCallback& callback);
    void SetOnClickCallback(const ClickCallback& callback);
    void SetOnDoubleClickCallback(const ClickCallback& callback);
    void SetOnLongPressCallback(const OnLongPress& callback);
    void SetOnPinchStartCallback(const GestureEventFunc& callback);
    void SetOnPinchUpdateCallback(const GestureEventFunc& callback);
    void SetOnPinchEndCallback(const GestureEventFunc& callback);
    void SetOnPinchCancelCallback(const GestureEventNoParameter& callback);

protected:
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;

private:
    void SetRemoteMessageCallback(const RefPtr<GestureListenerComponent>& component);
    void SetOnClickCallback(const RefPtr<GestureListenerComponent>& component);
    void SetOnDoubleClickCallback(const RefPtr<GestureListenerComponent>& component);
    void SetOnLongPressCallback(const RefPtr<GestureListenerComponent>& component);
    void SetOnPinchStartCallback(const RefPtr<GestureListenerComponent>& component);
    void SetOnPinchUpdateCallback(const RefPtr<GestureListenerComponent>& component);
    void SetOnPinchEndCallback(const RefPtr<GestureListenerComponent>& component);
    void SetOnPinchCancelCallback(const RefPtr<GestureListenerComponent>& component);

    RefPtr<ClickRecognizer> clickRecognizer_;
    RefPtr<ClickRecognizer> doubleClickRecognizer_;
    RefPtr<LongPressRecognizer> longPressRecognizer_;
    RefPtr<PinchRecognizer> pinchRecognizer_;
    RefPtr<FreeDragRecognizer> freeDragRecognizer_;
    RefPtr<VerticalDragRecognizer> verticalDragRecognizer_;
    RefPtr<HorizontalDragRecognizer> horizontalDragRecognizer_;
    bool isVisible_ = true;
    TouchEvent touchpoint_;
    RefPtr<RenderNode> dragTarget_ = nullptr;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GESTURE_LISTENER_RENDER_GESTURE_LISTENER_H
