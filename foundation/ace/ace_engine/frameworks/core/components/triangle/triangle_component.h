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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRIANGLE_TRIANGLE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRIANGLE_TRIANGLE_COMPONENT_H

#include "core/pipeline/base/constants.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

class TriangleComponent : public RenderComponent {
    DECLARE_ACE_TYPE(TriangleComponent, RenderComponent);

public:
    TriangleComponent() = default;
    ~TriangleComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    const Dimension& GetWidth() const
    {
        return width_;
    }
    void SetWidth(const Dimension& width)
    {
        width_ = width;
    }

    const Dimension& GetHeight() const
    {
        return height_;
    }
    void SetHeight(const Dimension& height)
    {
        height_ = height;
    }

    const Color& GetColor() const
    {
        return color_;
    }
    void SetColor(const Color& color)
    {
        color_ = color;
    }

    const Dimension& GetPadding() const
    {
        return padding_;
    }
    void SetPadding(const Dimension& value)
    {
        padding_ = value;
    }

private:
    Dimension width_;
    Dimension height_;
    Dimension padding_;
    Color color_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRIANGLE_TRIANGLE_COMPONENT_H
