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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_GRADIENT_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_GRADIENT_COMPONENT_H

#include "core/components/declaration/svg/svg_gradient_declaration.h"
#include "frameworks/core/pipeline/base/component_group.h"

namespace OHOS::Ace {

class SvgGradientComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(SvgGradientComponent, ComponentGroup);

public:
    SvgGradientComponent()
    {
        InitDeclaration();
    }

    explicit SvgGradientComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children)
    {
        InitDeclaration();
    }

    ~SvgGradientComponent() override = default;
    RefPtr<RenderNode> CreateRenderNode() override
    {
        return nullptr;
    }

    RefPtr<Element> CreateElement() override
    {
        return nullptr;
    }

    void InitDeclaration()
    {
        if (!declaration_) {
            declaration_ = AceType::MakeRefPtr<SvgGradientDeclaration>();
            declaration_->Init();
            declaration_->InitializeStyle();
        }
    }

    void SetDeclaration(const RefPtr<SvgGradientDeclaration>& declaration)
    {
        if (declaration) {
            declaration_ = declaration;
        }
    }

    const RefPtr<SvgGradientDeclaration>& GetDeclaration() const
    {
        return declaration_;
    }

    const Gradient& GetGradient() const
    {
        return declaration_->GetGradient();
    }

    void AddGradientColor(const GradientColor& gradientColor)
    {
        declaration_->AddGradientColor(gradientColor);
    }

private:
    RefPtr<SvgGradientDeclaration> declaration_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_SVG_GRADIENT_COMPONENT_H
