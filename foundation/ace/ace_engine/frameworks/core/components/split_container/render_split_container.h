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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SPLIT_CONTAINER_RENDER_SPLIT_CONTAINER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SPLIT_CONTAINER_RENDER_SPLIT_CONTAINER_H

#include "core/pipeline/base/render_node.h"
#include "core/gestures/drag_recognizer.h"

namespace OHOS::Ace {

class RenderSplitContainer : public RenderNode {
    DECLARE_ACE_TYPE(RenderSplitContainer, RenderNode);

public:
    RenderSplitContainer() = default;
    ~RenderSplitContainer() override = default;

    virtual void LayoutChildren() = 0;
    virtual void HandleDragStart(const Offset& startPoint) = 0;
    virtual void HandleDragUpdate(const Offset& currentPoint) = 0;
    virtual void HandleDragEnd(const Offset& endPoint, double velocity) = 0;
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void InitializeRecognizer();

    bool GetResizeable() const
    {
        return resizeable_;
    }

protected:
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;
    virtual void ResetComponentMember();

private:
    void UpdateComponentAttr(const RefPtr<Component>& component);

protected:
    RefPtr<DragRecognizer> dragDetector_;
    bool resizeable_ = false;
    double layoutWidth_ = 0.0;
    double layoutHeight_ = 0.0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SPLIT_CONTAINER_RENDER_SPLIT_CONTAINER_H