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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_RECT_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_RECT_COMPONENT_H

#include "core/components/declaration/svg/svg_rect_declaration.h"
#include "frameworks/core/pipeline/base/component_group.h"

namespace OHOS::Ace {

class SvgRectComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(SvgRectComponent, ComponentGroup);

public:
    SvgRectComponent();
    explicit SvgRectComponent(const std::list<RefPtr<Component>>& children);
    ~SvgRectComponent() override = default;
    void InitDeclaration();
    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;
    const Dimension& GetX() const;
    const Dimension& GetY() const;
    const Dimension& GetRx() const;
    const Dimension& GetRy() const;
    const Dimension& GetWidth() const;
    const Dimension& GetHeight() const;
    void SetX(const Dimension& x);
    void SetY(const Dimension& y);
    void SetRx(const Dimension& rx);
    void SetRy(const Dimension& ry);
    void SetWidth(const Dimension& width);
    void SetHeight(const Dimension& height);
    void SetDeclaration(const RefPtr<SvgRectDeclaration>& declaration);

    const RefPtr<SvgRectDeclaration>& GetDeclaration() const
    {
        return declaration_;
    }

    void Inherit(const RefPtr<SvgBaseDeclaration>& parent)
    {
        declaration_->Inherit(parent);
    }

private:
    RefPtr<SvgRectDeclaration> declaration_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_RECT_COMPONENT_H
