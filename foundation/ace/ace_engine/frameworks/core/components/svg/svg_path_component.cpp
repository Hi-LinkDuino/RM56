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

#include "frameworks/core/components/svg/svg_path_component.h"

#include "frameworks/core/components/svg/render_svg_path.h"
#include "frameworks/core/components/svg/svg_path_element.h"

namespace OHOS::Ace {

SvgPathComponent::SvgPathComponent()
{
    InitDeclaration();
}

SvgPathComponent::SvgPathComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children)
{
    InitDeclaration();
}

void SvgPathComponent::InitDeclaration()
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<SvgPathDeclaration>();
        declaration_->Init();
        declaration_->InitializeStyle();
    }
}

RefPtr<Element> SvgPathComponent::CreateElement()
{
    return AceType::MakeRefPtr<SvgPathElement>();
}

RefPtr<RenderNode> SvgPathComponent::CreateRenderNode()
{
    return RenderSvgPath::Create();
}

const std::string& SvgPathComponent::GetD() const
{
    return declaration_->GetD();
}

void SvgPathComponent::SetD(const std::string& d)
{
    declaration_->SetD(d);
}

void SvgPathComponent::SetDeclaration(const RefPtr<SvgPathDeclaration>& declaration)
{
    if (declaration) {
        declaration_ = declaration;
    }
}

} // namespace OHOS::Ace
