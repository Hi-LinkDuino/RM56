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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MOUSE_LISTENER_RENDER_MOUSE_LISTENER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MOUSE_LISTENER_RENDER_MOUSE_LISTENER_H

#include "base/utils/event_callback.h"
#include "core/components/proxy/render_item_proxy.h"
#include "core/event/mouse_raw_recognizer.h"
#include "core/pipeline/base/component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class ACE_EXPORT RenderMouseListener final : public RenderProxy {
    DECLARE_ACE_TYPE(RenderMouseListener, RenderProxy);

public:
    RenderMouseListener();
    ~RenderMouseListener() override = default;
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;

    void SetOnMouse(const MouseCallback& onMouse)
    {
        if (onMouse) {
            recognizer_->SetOnMouse(onMouse);
        }
    }

    void SetOnMouseHover(const MouseHoverCallback& onMouseHover)
    {
        if (onMouseHover) {
            recognizer_->SetOnMouseHover(onMouseHover);
        }
    }

    void SetOnHover(const OnHoverCallback& onHover)
    {
        if (onHover) {
            recognizer_->SetOnHover(onHover);
        }
    }

    void SetOnMouseHoverExit(const MouseHoverCallback& onMouseHoverExit)
    {
        if (onMouseHoverExit) {
            recognizer_->SetOnMouseHoverExit(onMouseHoverExit);
        }
    }

    void HandleMouseHoverEvent(MouseState mouseState) override;

protected:
    void OnMouseTestHit(const Offset& coordinateOffset, MouseTestResult& result) override;

private:
    RefPtr<MouseRawRecognizer> recognizer_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_MOUSE_LISTENER_RENDER_MOUSE_LISTENER_H
