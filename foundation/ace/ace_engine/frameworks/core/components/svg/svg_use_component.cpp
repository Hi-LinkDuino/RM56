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

#include "frameworks/core/components/svg/svg_use_component.h"

#include "frameworks/core/components/svg/render_svg_use.h"
#include "frameworks/core/components/svg/svg_use_element.h"

namespace OHOS::Ace {

SvgUseComponent::SvgUseComponent()
{
    InitDeclaration();
}

SvgUseComponent::SvgUseComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children)
{
    InitDeclaration();
}

void SvgUseComponent::InitDeclaration()
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<SvgDeclaration>();
        declaration_->Init();
        declaration_->InitializeStyle();
    }
}

RefPtr<Element> SvgUseComponent::CreateElement()
{
    return AceType::MakeRefPtr<SvgUseElement>();
}

RefPtr<RenderNode> SvgUseComponent::CreateRenderNode()
{
    return RenderSvgUse::Create();
}

void SvgUseComponent::SetX(const Dimension& x)
{
    declaration_->SetX(x);
}

const Dimension& SvgUseComponent::GetX() const
{
    return declaration_->GetX();
}

void SvgUseComponent::SetY(const Dimension& y)
{
    declaration_->SetY(y);
}

const Dimension& SvgUseComponent::GetY() const
{
    return declaration_->GetY();
}

void SvgUseComponent::SetDeclaration(const RefPtr<SvgDeclaration>& declaration)
{
    if (declaration) {
        declaration_ = declaration;
    }
}

} // namespace OHOS::Ace
