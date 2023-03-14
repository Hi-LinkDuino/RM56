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

#include "frameworks/core/components/svg/svg_line_component.h"

#include "frameworks/core/components/svg/render_svg_line.h"
#include "frameworks/core/components/svg/svg_line_element.h"

namespace OHOS::Ace {

SvgLineComponent::SvgLineComponent()
{
    InitDeclaration();
}

SvgLineComponent::SvgLineComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children)
{
    InitDeclaration();
}

void SvgLineComponent::InitDeclaration()
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<SvgLineDeclaration>();
        declaration_->Init();
        declaration_->InitializeStyle();
    }
}

RefPtr<Element> SvgLineComponent::CreateElement()
{
    return AceType::MakeRefPtr<SvgLineElement>();
}

void SvgLineComponent::SetX1(const Dimension& x1)
{
    declaration_->SetX1(x1);
}

void SvgLineComponent::SetX2(const Dimension& x2)
{
    declaration_->SetX2(x2);
}

void SvgLineComponent::SetY1(const Dimension& y1)
{
    declaration_->SetY1(y1);
}

void SvgLineComponent::SetY2(const Dimension& y2)
{
    declaration_->SetY2(y2);
}

const Dimension& SvgLineComponent::GetX1() const
{
    return declaration_->GetX1();
}

const Dimension& SvgLineComponent::GetX2() const
{
    return declaration_->GetX2();
}

const Dimension& SvgLineComponent::GetY1() const
{
    return declaration_->GetY1();
}

const Dimension& SvgLineComponent::GetY2() const
{
    return declaration_->GetY2();
}

RefPtr<RenderNode> SvgLineComponent::CreateRenderNode()
{
    return RenderSvgLine::Create();
}

void SvgLineComponent::SetDeclaration(const RefPtr<SvgLineDeclaration>& declaration)
{
    if (declaration) {
        declaration_ = declaration;
    }
}

} // namespace OHOS::Ace
