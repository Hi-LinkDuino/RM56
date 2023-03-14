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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WEB_RENDER_WEB_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WEB_RENDER_WEB_H

#include "core/components/common/layout/constants.h"
#include "core/components/web/resource/web_delegate.h"
#include "core/components/web/web_component.h"
#include "core/gestures/raw_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

namespace {
#ifdef OHOS_STANDARD_SYSTEM
struct TouchInfo {
    double x = -1;
    double y = -1;
    int32_t id = -1;
};
#endif
}

class RenderWeb : public RenderNode {
    DECLARE_ACE_TYPE(RenderWeb, RenderNode);

public:
    static RefPtr<RenderNode> Create();

    RenderWeb();
    ~RenderWeb() override = default;

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void OnAttachContext() override;

#ifdef OHOS_STANDARD_SYSTEM
    void OnAppShow() override
    {
        RenderNode::OnAppShow();
        if (delegate_) {
            delegate_->ShowWebView();
        }
    }

    void OnAppHide() override
    {
        RenderNode::OnAppHide();
        if (delegate_) {
            delegate_->HideWebView();
        }
    }
#endif

    void SetDelegate(const RefPtr<WebDelegate>& delegate)
    {
        delegate_ = delegate;
    }

protected:
    RefPtr<WebDelegate> delegate_;
    RefPtr<WebComponent> web_;
    Size drawSize_;
    bool isUrlLoaded_ = false;

private:
#ifdef OHOS_STANDARD_SYSTEM
    void Initialize();
    void HandleTouchDown(const TouchEventInfo& info);
    void HandleTouchUp(const TouchEventInfo& info);
    void HandleTouchMove(const TouchEventInfo& info);
    void HandleTouchCancel(const TouchEventInfo& info);
    bool ParseTouchInfo(const TouchEventInfo& info, std::list<TouchInfo>& touchInfos, const TouchType& touchType);
    void OnTouchTestHit(const Offset& coordinateOffset, const TouchRestrict& touchRestrict,
        TouchTestResult& result) override;
    RefPtr<RawRecognizer> touchRecognizer_ = nullptr;
#endif

    Offset position_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WEB_RENDER_WEB_H
