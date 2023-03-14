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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOUCH_LISTENER_RENDER_TOUCH_LISTENER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOUCH_LISTENER_RENDER_TOUCH_LISTENER_H

#include "base/utils/event_callback.h"
#include "base/utils/macros.h"
#include "core/components/proxy/render_item_proxy.h"
#include "core/gestures/raw_recognizer.h"
#include "core/gestures/swipe_recognizer.h"
#include "core/pipeline/base/component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class ACE_EXPORT RenderTouchListener final : public RenderProxy, public TouchEventTarget {
    DECLARE_ACE_TYPE(RenderTouchListener, RenderProxy, TouchEventTarget);

public:
    RenderTouchListener();
    ~RenderTouchListener() override = default;
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;

    void SetOnTouchDown(const OnTouchEventCallback& onTouchDown) const
    {
        if (onTouchDown) {
            rawRecognizer_->SetOnTouchDown(onTouchDown);
        }
    }
    void SetOnTouchMove(const OnTouchEventCallback& onTouchMove) const
    {
        if (onTouchMove) {
            rawRecognizer_->SetOnTouchMove(onTouchMove);
        }
    }
    void SetOnTouchUp(const OnTouchEventCallback& onTouchUp) const
    {
        if (onTouchUp) {
            rawRecognizer_->SetOnTouchUp(onTouchUp);
        }
    }
    void SetOnTouchCancel(const OnTouchEventCallback& onTouchCancel) const
    {
        if (onTouchCancel) {
            rawRecognizer_->SetOnTouchCancel(onTouchCancel);
        }
    }
    void SetOnSwipe(const SwipeCallback& onSwipeCallback)
    {
        if (onSwipeCallback) {
            if (!swipeRecognizer_) {
                swipeRecognizer_ = AceType::MakeRefPtr<SwipeRecognizer>();
            }
            swipeRecognizer_->AddSwipeCallback(onSwipeCallback);
        }
    }

    bool GetVisible() const override;

    bool DispatchEvent(const TouchEvent& point) override;
    bool HandleEvent(const TouchEvent& point) override;
    bool TriggerTouchCallBack(const TouchEvent& changedPoint);

protected:
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;

private:
    RefPtr<RawRecognizer> rawRecognizer_;
    RefPtr<SwipeRecognizer> swipeRecognizer_;

    bool isVisible_ = true;
    Offset coordinateOffset_;
    std::function<void(const std::shared_ptr<TouchEventInfo>&)> onTouchEventCallback_;
    std::map<int, TouchEvent> touchPointMap_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOUCH_LISTENER_RENDER_TOUCH_LISTENER_H
