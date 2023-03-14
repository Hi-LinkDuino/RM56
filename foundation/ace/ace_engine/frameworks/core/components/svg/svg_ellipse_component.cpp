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

#include "frameworks/core/components/svg/svg_ellipse_component.h"

#include "frameworks/core/components/svg/render_svg_ellipse.h"
#include "frameworks/core/components/svg/svg_ellipse_element.h"

namespace OHOS::Ace {

SvgEllipseComponent::SvgEllipseComponent()
{
    InitDeclaration();
}

SvgEllipseComponent::SvgEllipseComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children)
{
    InitDeclaration();
}

void SvgEllipseComponent::InitDeclaration()
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<SvgEllipseDeclaration>();
        declaration_->Init();
        declaration_->InitializeStyle();
    }
}

RefPtr<Element> SvgEllipseComponent::CreateElement()
{
    return AceType::MakeRefPtr<SvgEllipseElement>();
}

RefPtr<RenderNode> SvgEllipseComponent::CreateRenderNode()
{
    return RenderSvgEllipse::Create();
}

void SvgEllipseComponent::SetCx(const Dimension& cx)
{
    declaration_->SetCx(cx);
}

const Dimension& SvgEllipseComponent::GetCx() const
{
    return declaration_->GetCx();
}

void SvgEllipseComponent::SetCy(const Dimension& cy)
{
    declaration_->SetCy(cy);
}

const Dimension& SvgEllipseComponent::GetCy() const
{
    return declaration_->GetCy();
}

void SvgEllipseComponent::SetRx(const Dimension& rx)
{
    declaration_->SetRx(rx);
}

const Dimension& SvgEllipseComponent::GetRx() const
{
    return declaration_->GetRx();
}

void SvgEllipseComponent::SetRy(const Dimension& ry)
{
    declaration_->SetRy(ry);
}

const Dimension& SvgEllipseComponent::GetRy() const
{
    return declaration_->GetRy();
}

void SvgEllipseComponent::SetDeclaration(const RefPtr<SvgEllipseDeclaration>& declaration)
{
    if (declaration) {
        declaration_ = declaration;
    }
}

} // namespace OHOS::Ace
