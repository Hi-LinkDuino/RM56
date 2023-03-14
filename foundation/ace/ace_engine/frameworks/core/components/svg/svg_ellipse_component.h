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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_ELLIPSE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_ELLIPSE_COMPONENT_H

#include "core/components/declaration/svg/svg_ellipse_declaration.h"
#include "frameworks/core/pipeline/base/component_group.h"

namespace OHOS::Ace {

class SvgEllipseComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(SvgEllipseComponent, ComponentGroup);

public:
    SvgEllipseComponent();
    explicit SvgEllipseComponent(const std::list<RefPtr<Component>>& children);
    ~SvgEllipseComponent() override = default;
    void InitDeclaration();
    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;
    void SetCx(const Dimension& cx);
    const Dimension& GetCx() const;
    void SetCy(const Dimension& cy);
    const Dimension& GetCy() const;
    void SetRx(const Dimension& rx);
    const Dimension& GetRx() const;
    void SetRy(const Dimension& ry);
    const Dimension& GetRy() const;
    void SetDeclaration(const RefPtr<SvgEllipseDeclaration>& declaration);

    const RefPtr<SvgEllipseDeclaration>& GetDeclaration() const
    {
        return declaration_;
    }

    void Inherit(const RefPtr<SvgBaseDeclaration>& parent)
    {
        declaration_->Inherit(parent);
    }

private:
    RefPtr<SvgEllipseDeclaration> declaration_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_ELLIPSE_COMPONENT_H
