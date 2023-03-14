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

#include "frameworks/core/components/svg/svg_component.h"

#include "frameworks/core/components/svg/render_svg.h"
#include "frameworks/core/components/svg/svg_element.h"

namespace OHOS::Ace {

SvgComponent::SvgComponent()
{
    InitDeclaration();
}

SvgComponent::SvgComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children)
{
    InitDeclaration();
}

void SvgComponent::InitDeclaration()
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<SvgDeclaration>();
        declaration_->Init();
        declaration_->InitializeStyle();
    }
}

RefPtr<Element> SvgComponent::CreateElement()
{
    return AceType::MakeRefPtr<SvgElement>();
}

RefPtr<RenderNode> SvgComponent::CreateRenderNode()
{
    return RenderSvg::Create();
}

void SvgComponent::SetX(const Dimension& x)
{
    declaration_->SetX(x);
}

const Dimension& SvgComponent::GetX() const
{
    return declaration_->GetX();
}

void SvgComponent::SetY(const Dimension& y)
{
    declaration_->SetY(y);
}

const Dimension& SvgComponent::GetY() const
{
    return declaration_->GetY();
}

void SvgComponent::SetWidth(const Dimension& width)
{
    declaration_->SetWidth(width);
}

const Dimension& SvgComponent::GetWidth() const
{
    return declaration_->GetWidth();
}

void SvgComponent::SetHeight(const Dimension& height)
{
    declaration_->SetHeight(height);
}

const Dimension& SvgComponent::GetHeight() const
{
    return declaration_->GetHeight();
}

void SvgComponent::SetViewBox(const Rect& viewBox)
{
    declaration_->SetViewBox(viewBox);
}

const Rect& SvgComponent::GetViewBox() const
{
    return declaration_->GetViewBox();
}

void SvgComponent::SetAutoMirror(bool autoMirror)
{
    declaration_->SetAutoMirror(autoMirror);
}

bool SvgComponent::GetAutoMirror() const
{
    return declaration_->GetAutoMirror();
}

void SvgComponent::SetDeclaration(const RefPtr<SvgDeclaration>& declaration)
{
    if (declaration) {
        declaration_ = declaration;
    }
}

} // namespace OHOS::Ace
