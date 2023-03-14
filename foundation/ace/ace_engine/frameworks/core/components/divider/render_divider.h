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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIVIDER_RENDER_DIVIDER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIVIDER_RENDER_DIVIDER_H

#include "core/components/divider/divider_component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderDivider : public RenderNode {
    DECLARE_ACE_TYPE(RenderDivider, RenderNode);

public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;

    bool GetIsVertical() const
    {
        return vertical_;
    }

    Color GetDividerColor() const
    {
        return dividerColor_;
    }

    Dimension GetStrokeWidth() const
    {
        return strokeWidth_;
    }

    LineCap GetLineCap() const
    {
        return lineCap_;
    }

protected:
    bool vertical_ = false;
    double dividerLength_ = 0.0;
    double constrainStrokeWidth_ = 0.0;
    Dimension strokeWidth_;
    LineCap lineCap_ = LineCap::BUTT;
    Color dividerColor_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIVIDER_RENDER_DIVIDER_H
