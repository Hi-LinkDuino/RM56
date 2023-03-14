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

#include "frameworks/core/components/svg/svg_filter_component.h"

#include "frameworks/core/components/svg/render_svg_filter.h"
#include "frameworks/core/components/svg/svg_filter_element.h"

namespace OHOS::Ace {

SvgFilterComponent::SvgFilterComponent()
{
    InitDeclaration();
}

SvgFilterComponent::SvgFilterComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children)
{
    InitDeclaration();
}

void SvgFilterComponent::InitDeclaration()
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<SvgFilterDeclaration>();
        declaration_->Init();
        declaration_->InitializeStyle();
    }
}

RefPtr<Element> SvgFilterComponent::CreateElement()
{
    return AceType::MakeRefPtr<SvgFilterElement>();
}

RefPtr<RenderNode> SvgFilterComponent::CreateRenderNode()
{
    return RenderSvgFilter::Create();
}

void SvgFilterComponent::SetDeclaration(const RefPtr<SvgFilterDeclaration>& declaration)
{
    if (declaration) {
        declaration_ = declaration;
    }
}

} // namespace OHOS::Ace
