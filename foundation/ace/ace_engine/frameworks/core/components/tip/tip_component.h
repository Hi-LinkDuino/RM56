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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TIP_TIP_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TIP_TIP_COMPONENT_H

#include "base/geometry/dimension.h"
#include "core/components/common/properties/color.h"
#include "core/components/text/text_component.h"
#include "core/pipeline/base/render_component.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

class TipComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(TipComponent, SoleChildComponent);

public:
    explicit TipComponent(const RefPtr<Component>& child) : SoleChildComponent(child) {}

    ~TipComponent() override = default;
    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;

    const Color& GetBgColor() const
    {
        return bgColor_;
    }

    void SetBgColor(const Color& color)
    {
        bgColor_ = color;
    }

    Axis GetDirection() const
    {
        return axis_;
    }

    void SetDirection(Axis axis)
    {
        axis_ = axis;
    }

private:
    Color bgColor_ = Color::BLACK;
    Axis axis_ = Axis::HORIZONTAL;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TIP_TIP_COMPONENT_H
