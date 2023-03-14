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

#include "frameworks/core/components/svg/svg_circle_component.h"

#include "frameworks/core/components/svg/render_svg_circle.h"
#include "frameworks/core/components/svg/svg_circle_element.h"

namespace OHOS::Ace {

SvgCircleComponent::SvgCircleComponent()
{
    InitDeclaration();
}

SvgCircleComponent::SvgCircleComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children)
{
    InitDeclaration();
}

void SvgCircleComponent::InitDeclaration()
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<SvgCircleDeclaration>();
        declaration_->Init();
        declaration_->InitializeStyle();
    }
}

RefPtr<Element> SvgCircleComponent::CreateElement()
{
    return AceType::MakeRefPtr<SvgCircleElement>();
}

RefPtr<RenderNode> SvgCircleComponent::CreateRenderNode()
{
    return RenderSvgCircle::Create();
}

void SvgCircleComponent::SetCx(const Dimension& cx)
{
    declaration_->SetCx(cx);
}

const Dimension& SvgCircleComponent::GetCx() const
{
    return declaration_->GetCx();
}

void SvgCircleComponent::SetCy(const Dimension& cy)
{
    declaration_->SetCy(cy);
}

const Dimension& SvgCircleComponent::GetCy() const
{
    return declaration_->GetCy();
}

void SvgCircleComponent::SetR(const Dimension& r)
{
    declaration_->SetR(r);
}

const Dimension& SvgCircleComponent::GetR() const
{
    return declaration_->GetR();
}

void SvgCircleComponent::SetDeclaration(const RefPtr<SvgCircleDeclaration>& declaration)
{
    if (declaration) {
        declaration_ = declaration;
    }
}

} // namespace OHOS::Ace
