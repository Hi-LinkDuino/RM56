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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIVIDER_DIVIDER_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIVIDER_DIVIDER_COMPONENT_H

#include "core/components/common/properties/color.h"
#include "core/components/divider/divider_element.h"
#include "core/pipeline/base/render_component.h"

namespace OHOS::Ace {

class ACE_EXPORT DividerComponent : public RenderComponent {
    DECLARE_ACE_TYPE(DividerComponent, RenderComponent);

public:
    RefPtr<RenderNode> CreateRenderNode() override;

    RefPtr<Element> CreateElement() override
    {
        return AceType::MakeRefPtr<DividerElement>();
    }

    const Color& GetDividerColor() const
    {
        return dividerColor_;
    }

    bool IsVertical() const
    {
        return vertical_;
    }

    void SetDividerColor(const Color& color)
    {
        dividerColor_ = color;
    }

    void SetVertical(bool vertical)
    {
        vertical_ = vertical;
    }

    const Dimension& GetStrokeWidth() const
    {
        return strokeWidth_;
    }

    void SetStrokeWidth(const Dimension& strokeWidth)
    {
        strokeWidth_ = strokeWidth;
    }

    LineCap GetLineCap() const
    {
        return lineCap_;
    }

    void SetLineCap(LineCap lineCap)
    {
        lineCap_ = lineCap;
    }

private:
    Color dividerColor_;
    bool vertical_ = false;
    LineCap lineCap_ = LineCap::BUTT;
    Dimension strokeWidth_ = Dimension(1.0);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIVIDER_DIVIDER_COMPONENT_H
