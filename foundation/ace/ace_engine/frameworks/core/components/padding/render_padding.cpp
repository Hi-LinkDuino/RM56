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

#include "core/components/padding/render_padding.h"

#include "core/components/padding/padding_component.h"

namespace OHOS::Ace {

RefPtr<RenderNode> RenderPadding::Create()
{
    return AceType::MakeRefPtr<RenderPadding>();
}

void RenderPadding::Update(const RefPtr<Component>& component)
{
    const RefPtr<PaddingComponent> padding = AceType::DynamicCast<PaddingComponent>(component);
    if (!padding) {
        return;
    }
    padding_ = padding->GetPadding();
    MarkNeedLayout();
}

void RenderPadding::PerformLayout()
{
    auto width = NormalizePercentToPx(padding_.Left(), false) + NormalizePercentToPx(padding_.Right(), false);
    auto height = NormalizePercentToPx(padding_.Top(), true) + NormalizePercentToPx(padding_.Bottom(), true);
    auto left = NormalizePercentToPx(padding_.Left(), false);
    auto top = NormalizePercentToPx(padding_.Top(), true);
    auto layoutSize = Size(width, height);
    if (!GetChildren().empty()) {
        auto child = GetChildren().front();
        LayoutParam innerLayout;
        Size maxLayoutSize = GetLayoutParam().GetMaxSize() - layoutSize;
        Size minLayoutSize = GetLayoutParam().GetMinSize() - layoutSize;
        if (maxLayoutSize.IsValid()) {
            // set child's constraints and layout child.
            innerLayout.SetMaxSize(maxLayoutSize);
            innerLayout.SetMinSize(minLayoutSize);
            child->Layout(innerLayout);

            // set position of child.
            child->SetPosition(Offset(left, top));

            layoutSize += child->GetLayoutSize();
        }
    }
    SetLayoutSize(layoutSize);
}

} // namespace OHOS::Ace