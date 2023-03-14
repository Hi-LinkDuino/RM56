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

#include "frameworks/core/components/svg/svg_defs_component.h"

#include "frameworks/core/components/svg/render_svg_defs.h"
#include "frameworks/core/components/svg/svg_defs_element.h"

namespace OHOS::Ace {

SvgDefsComponent::SvgDefsComponent()
{
    InitDeclaration();
}

SvgDefsComponent::SvgDefsComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children)
{
    InitDeclaration();
}

void SvgDefsComponent::InitDeclaration()
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<SvgBaseDeclaration>();
        declaration_->Init();
        declaration_->InitializeStyle();
    }
}

RefPtr<Element> SvgDefsComponent::CreateElement()
{
    return AceType::MakeRefPtr<SvgDefsElement>();
}

RefPtr<RenderNode> SvgDefsComponent::CreateRenderNode()
{
    return RenderSvgDefs::Create();
}

void SvgDefsComponent::SetDeclaration(const RefPtr<SvgBaseDeclaration>& declaration)
{
    if (declaration) {
        declaration_ = declaration;
    }
}

} // namespace OHOS::Ace
