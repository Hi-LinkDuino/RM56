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

#include "frameworks/core/components/shape/render_shape_container.h"

namespace OHOS::Ace {

void RenderShapeContainer::Update(const RefPtr<Component>& component)
{
    auto shapeContainerComponent = AceType::DynamicCast<ShapeContainerComponent>(component);
    if (!shapeContainerComponent) {
        LOGW("component is null");
        return;
    }
    viewBox_ = shapeContainerComponent->GetViewBox();
    hasDefineWidth_ = shapeContainerComponent->GetWidthFlag();
    hasDefineHeight_ = shapeContainerComponent->GetHeightFlag();
    fillState_ = shapeContainerComponent->GetFillState();
    strokeState_ = shapeContainerComponent->GetStrokeState();
    antiAlias_ = shapeContainerComponent->GetAntiAlias();
    column_ = shapeContainerComponent->GetMeshColumn();
    row_ = shapeContainerComponent->GetMeshRow();
    mesh_ = shapeContainerComponent->GetMesh();
    MarkNeedLayout();
}

void RenderShapeContainer::PerformLayout()
{
    for (const auto& child : GetChildren()) {
        child->SetPosition(Offset(0, 0));
    }

    if (!hasDefineWidth_ && !hasDefineHeight_) {
        if (viewBox_.IsValid()) {
            double viewBoxWidth = NormalizePercentToPx(viewBox_.Width(), false);
            double viewBoxHeight = NormalizePercentToPx(viewBox_.Height(), true);
            Size newSize = GetLayoutParam().Constrain(Size(viewBoxWidth, viewBoxHeight));
            SetLayoutSize(newSize);
            const auto& children = GetChildren();
            for (const auto& child : children) {
                child->Layout(LayoutParam(newSize, GetLayoutParam().GetMinSize()));
            }
            return;
        }
    }

    const auto& children = GetChildren();
    for (const auto& child : children) {
        child->Layout(GetLayoutParam());
    }
    if (!hasDefineWidth_ || !hasDefineHeight_) {
        double maxX = 0.0;
        double maxY = 0.0;
        for (const auto& item : GetChildren()) {
            if (!hasDefineWidth_) {
                maxX = std::max(maxX, item->GetLayoutSize().Width() + NormalizePercentToPx(item->GetLeft(), false));
            }
            if (!hasDefineHeight_) {
                maxY = std::max(maxY, item->GetLayoutSize().Height() + NormalizePercentToPx(item->GetTop(), true));
            }
        }
        if (hasDefineWidth_) {
            maxX = GetLayoutParam().GetMaxSize().Width();
        }
        if (hasDefineHeight_) {
            maxY = GetLayoutParam().GetMaxSize().Height();
        }
        SetLayoutSize(GetLayoutParam().Constrain(Size(maxX, maxY)));
    } else {
        SetLayoutSize(GetLayoutParam().GetMaxSize());
    }
}

} // namespace OHOS::Ace
