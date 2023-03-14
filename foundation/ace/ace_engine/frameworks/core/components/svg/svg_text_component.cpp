/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "frameworks/core/components/svg/svg_text_component.h"

#include "frameworks/core/components/svg/flutter_render_svg_text.h"
#include "frameworks/core/components/svg/svg_text_element.h"

namespace OHOS::Ace {

SvgTextComponent::SvgTextComponent()
{
    InitDeclaration();
}

SvgTextComponent::SvgTextComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children)
{
    InitDeclaration();
}

void SvgTextComponent::InitDeclaration()
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<SvgTextDeclaration>();
        declaration_->Init();
    }
}

RefPtr<Element> SvgTextComponent::CreateElement()
{
    return AceType::MakeRefPtr<SvgTextElement>();
}

RefPtr<RenderNode> SvgTextComponent::CreateRenderNode()
{
    return RenderSvgText::Create();
}

const std::string& SvgTextComponent::GetTextData() const
{
    return declaration_->GetTextData();
}

void SvgTextComponent::SetTextData(const std::string& textData)
{
    declaration_->SetTextData(textData);
}

void SvgTextComponent::SetX(const Dimension& x)
{
    declaration_->SetX(x);
}

const Dimension& SvgTextComponent::GetX() const
{
    return declaration_->GetX();
}

void SvgTextComponent::SetY(const Dimension& y)
{
    declaration_->SetY(y);
}

const Dimension& SvgTextComponent::GetY() const
{
    return declaration_->GetY();
}

void SvgTextComponent::SetDx(const Dimension& dx)
{
    declaration_->SetDx(dx);
}

const Dimension& SvgTextComponent::GetDx() const
{
    return declaration_->GetDx();
}

void SvgTextComponent::SetDy(const Dimension& dy)
{
    declaration_->SetDy(dy);
}

const Dimension& SvgTextComponent::GetDy() const
{
    return declaration_->GetDy();
}

void SvgTextComponent::SetHasX(bool hasX)
{
    declaration_->SetHasX(hasX);
}

bool SvgTextComponent::GetHasX() const
{
    return declaration_->HasX();
}

void SvgTextComponent::SetHasY(bool hasY)
{
    declaration_->SetHasY(hasY);
}

bool SvgTextComponent::GetHasY() const
{
    return declaration_->HasY();
}

void SvgTextComponent::SetRotate(double rotate)
{
    declaration_->SetRotate(rotate);
}

double SvgTextComponent::GetRotate() const
{
    return declaration_->GetRotate();
}

void SvgTextComponent::SetDeclaration(const RefPtr<SvgTextDeclaration>& declaration)
{
    if (declaration) {
        declaration_ = declaration;
    }
}

} // namespace OHOS::Ace
