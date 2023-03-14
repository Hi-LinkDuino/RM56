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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_LINE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_LINE_COMPONENT_H

#include "core/components/declaration/svg/svg_line_declaration.h"
#include "frameworks/core/pipeline/base/component_group.h"

namespace OHOS::Ace {

class SvgLineComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(SvgLineComponent, ComponentGroup);

public:
    SvgLineComponent();
    explicit SvgLineComponent(const std::list<RefPtr<Component>>& children);
    ~SvgLineComponent() override = default;
    void InitDeclaration();
    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;
    void SetX1(const Dimension& x1);
    void SetX2(const Dimension& x2);
    void SetY1(const Dimension& y1);
    void SetY2(const Dimension& y2);
    const Dimension& GetX1() const;
    const Dimension& GetX2() const;
    const Dimension& GetY1() const;
    const Dimension& GetY2() const;
    void SetDeclaration(const RefPtr<SvgLineDeclaration>& declaration);

    const RefPtr<SvgLineDeclaration>& GetDeclaration() const
    {
        return declaration_;
    }

    void Inherit(const RefPtr<SvgBaseDeclaration>& parent)
    {
        declaration_->Inherit(parent);
    }

private:
    RefPtr<SvgLineDeclaration> declaration_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_LINE_COMPONENT_H
