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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHAPE_RENDER_SHAPE_CONTAINER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHAPE_RENDER_SHAPE_CONTAINER_H

#include "frameworks/core/components/shape/shape_container_component.h"
#include "frameworks/core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderShapeContainer : public RenderNode {
    DECLARE_ACE_TYPE(RenderShapeContainer, RenderNode);

public:
    static RefPtr<RenderNode> Create();
    ~RenderShapeContainer() override = default;

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;

    void OnAttachContext() override
    {
        const std::function<void()> callback = [weak = WeakClaim(this)] {
            auto renderShapeContainer = weak.Upgrade();
            if (renderShapeContainer) {
                renderShapeContainer->MarkNeedLayout();
            }
        };
        viewBox_.SetContextAndCallback(context_, callback);
    }

    FillState GetFillState() const
    {
        return fillState_;
    }

    StrokeState GetStrokeState() const
    {
        return strokeState_;
    }

    bool GetAntiAlias() const
    {
        return antiAlias_.first && antiAlias_.second;
    }

    ShapeViewBox GetShapeViewBox() const
    {
        return viewBox_;
    }

protected:
    ShapeViewBox viewBox_;
    bool hasDefineWidth_ = false;
    bool hasDefineHeight_ = false;
    FillState fillState_;
    StrokeState strokeState_;
    std::pair<bool, bool> antiAlias_ = std::make_pair(false, true);
    int32_t row_ = 0;
    int32_t column_ = 0;
    std::vector<double> mesh_;

};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SHAPE_RENDER_SHAPE_CONTAINER_H
