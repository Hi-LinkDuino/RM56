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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_BUBBLE_PROGRESS_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_BUBBLE_PROGRESS_COMPONENT_H

#include "base/geometry/dimension.h"
#include "base/geometry/offset.h"
#include "core/components/common/properties/color.h"
#include "core/pipeline/base/render_component.h"

namespace OHOS::Ace {

class BubbleProgressComponent : public RenderComponent {
    DECLARE_ACE_TYPE(BubbleProgressComponent, RenderComponent);

public:
    BubbleProgressComponent() = default;
    explicit BubbleProgressComponent(double diameter) : diameter_(diameter) {}
    ~BubbleProgressComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    const Dimension& GetDiameter() const
    {
        return diameter_;
    }

    void SetDiameter(const Dimension& diameter)
    {
        diameter_ = diameter;
    }

    void SetBubbleRadius(const Dimension& radius)
    {
        bubbleRadius_ = radius;
    }

    const Dimension& GetBubbleRadius() const
    {
        return bubbleRadius_;
    }

private:
    Dimension diameter_;
    Dimension bubbleRadius_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PROGRESS_BUBBLE_PROGRESS_COMPONENT_H