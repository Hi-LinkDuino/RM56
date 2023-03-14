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

#include "frameworks/core/components/svg/render_svg_defs.h"

#include "frameworks/core/components/svg/svg_defs_component.h"

namespace OHOS::Ace {

RefPtr<RenderNode> RenderSvgDefs::Create()
{
    return AceType::MakeRefPtr<RenderSvgDefs>();
}

void RenderSvgDefs::Update(const RefPtr<Component>& component)
{
    const RefPtr<SvgDefsComponent> svgDefsComponent = AceType::DynamicCast<SvgDefsComponent>(component);
    if (!svgDefsComponent) {
        LOGW("component is null");
        return;
    }
    RenderSvgBase::SetPresentationAttrs(svgDefsComponent->GetDeclaration());
    MarkNeedLayout();
}

void RenderSvgDefs::PerformLayout()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("context is null");
        return;
    }
    const auto& children = GetChildren();
    for (const auto& child : children) {
        child->Layout(GetLayoutParam());
    }
    SetLayoutSize(GetLayoutParam().GetMaxSize());
}

} // namespace OHOS::Ace
