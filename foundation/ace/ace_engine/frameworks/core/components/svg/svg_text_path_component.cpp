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

#include "frameworks/core/components/svg/svg_text_path_component.h"

#include "frameworks/core/components/svg/render_svg_text_path.h"
#include "frameworks/core/components/svg/svg_text_path_element.h"

namespace OHOS::Ace {

SvgTextPathComponent::SvgTextPathComponent()
{
    InitDeclaration();
}

SvgTextPathComponent::SvgTextPathComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children)
{
    InitDeclaration();
}

void SvgTextPathComponent::InitDeclaration()
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<SvgTextPathDeclaration>();
        declaration_->Init();
    }
}

RefPtr<Element> SvgTextPathComponent::CreateElement()
{
    return AceType::MakeRefPtr<SvgTextPathElement>();
}

RefPtr<RenderNode> SvgTextPathComponent::CreateRenderNode()
{
    return RenderSvgTextPath::Create();
}

const std::string& SvgTextPathComponent::GetTextData() const
{
    return declaration_->GetTextData();
}

void SvgTextPathComponent::SetTextData(const std::string& textData)
{
    declaration_->SetTextData(textData);
}

const std::string& SvgTextPathComponent::GetPath() const
{
    return declaration_->GetPath();
}

void SvgTextPathComponent::SetPath(const std::string& path)
{
    declaration_->SetPath(path);
}

const Dimension& SvgTextPathComponent::GetStartOffset() const
{
    return declaration_->GetStartOffset();
}

void SvgTextPathComponent::SetStartOffset(const Dimension& startOffset)
{
    declaration_->SetStartOffset(startOffset);
}

void SvgTextPathComponent::SetDeclaration(const RefPtr<SvgTextPathDeclaration>& declaration)
{
    if (declaration) {
        declaration_ = declaration;
    }
}

} // namespace OHOS::Ace
