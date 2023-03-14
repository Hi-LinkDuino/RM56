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

#include "frameworks/core/components/svg/svg_rect_component.h"

#include "frameworks/core/components/svg/render_svg_rect.h"
#include "frameworks/core/components/svg/svg_rect_element.h"

namespace OHOS::Ace {

SvgRectComponent::SvgRectComponent()
{
    InitDeclaration();
}

SvgRectComponent::SvgRectComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children)
{
    InitDeclaration();
}

void SvgRectComponent::InitDeclaration()
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<SvgRectDeclaration>();
        declaration_->Init();
        declaration_->InitializeStyle();
    }
}

RefPtr<Element> SvgRectComponent::CreateElement()
{
    return AceType::MakeRefPtr<SvgRectElement>();
}

RefPtr<RenderNode> SvgRectComponent::CreateRenderNode()
{
    return RenderSvgRect::Create();
}

const Dimension& SvgRectComponent::GetX() const
{
    return declaration_->GetX();
}

const Dimension& SvgRectComponent::GetY() const
{
    return declaration_->GetY();
}

const Dimension& SvgRectComponent::GetRx() const
{
    return declaration_->GetRx();
}

const Dimension& SvgRectComponent::GetRy() const
{
    return declaration_->GetRy();
}

const Dimension& SvgRectComponent::GetWidth() const
{
    return declaration_->GetWidth();
}

const Dimension& SvgRectComponent::GetHeight() const
{
    return declaration_->GetHeight();
}

void SvgRectComponent::SetX(const Dimension& x)
{
    declaration_->SetX(x);
}

void SvgRectComponent::SetY(const Dimension& y)
{
    declaration_->SetY(y);
}

void SvgRectComponent::SetRx(const Dimension& rx)
{
    declaration_->SetRx(rx);

}

void SvgRectComponent::SetRy(const Dimension& ry)
{
    declaration_->SetRy(ry);
}

void SvgRectComponent::SetWidth(const Dimension& width)
{
    declaration_->SetWidth(width);
}

void SvgRectComponent::SetHeight(const Dimension& height)
{
    declaration_->SetHeight(height);
}

void SvgRectComponent::SetDeclaration(const RefPtr<SvgRectDeclaration>& declaration)
{
    if (declaration) {
        declaration_ = declaration;
    }
}

} // namespace OHOS::Ace
