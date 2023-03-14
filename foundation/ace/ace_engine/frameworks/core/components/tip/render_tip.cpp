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

#include "core/components/tip/render_tip.h"

#include "core/components/tip/tip_component.h"

namespace OHOS::Ace {

namespace {

constexpr double HALF = 0.5;
constexpr Dimension TEXT_MIN_WIDTH = 60.0_vp;

} // namespace

void RenderTip::Update(const RefPtr<Component>& component)
{
    const RefPtr<TipComponent> tip = AceType::DynamicCast<TipComponent>(component);
    if (tip) {
        bgColor_ = tip->GetBgColor();
        direction_ = tip->GetDirection();
        MarkNeedLayout();
    }
}

void RenderTip::PerformLayout()
{
    auto context = GetContext().Upgrade();
    if (!context) {
        return;
    }
    Size layoutSize;
    if (!GetChildren().empty()) {
        auto child = GetChildren().front();
        LayoutParam innerLayout;
        Size maxLayoutSize = GetLayoutParam().GetMaxSize();
        if (maxLayoutSize.IsValid()) {
            innerLayout.SetMaxSize(maxLayoutSize);
            child->Layout(innerLayout);
            layoutSize += child->GetLayoutSize();
        }

        // adjust padding around child
        AdaptChildPadding(layoutSize, maxLayoutSize);

        // set text position
        double paddingLeft = NormalizeToPx(padding_.Left());
        double paddingTop = NormalizeToPx(padding_.Top());
        child->SetPosition(Offset(paddingLeft, paddingTop));
        childSize_ = layoutSize + padding_.GetLayoutSizeInPx(context->GetDipScale());
        border_.SetBorderRadius(direction_ == Axis::VERTICAL ? Radius(Dimension(childSize_.Width() * HALF,
            DimensionUnit::PX)) : Radius(Dimension(childSize_.Height() * HALF, DimensionUnit::PX)));
        SetLayoutSize(maxLayoutSize);
    }
}

void RenderTip::AdaptChildPadding(const Size& childSize, const Size& selfSize)
{
    if (direction_ == Axis::VERTICAL) {
        double widthChange = std::max(0.0, NormalizeToPx(TEXT_MIN_WIDTH) - childSize.Height());
        if (NearEqual(widthChange, 0.0)) {
            // convert to px
            padding_.SetTop(Dimension(NormalizeToPx(padding_.Top()), DimensionUnit::PX));
            padding_.SetBottom(Dimension(NormalizeToPx(padding_.Bottom()), DimensionUnit::PX));
        } else {
            padding_.SetTop(Dimension(widthChange * HALF, DimensionUnit::PX));
            padding_.SetBottom(Dimension(widthChange * HALF, DimensionUnit::PX));
        }

        double paddingTopBottom = (selfSize.Width() - NormalizeToPx(TIP_SPACING) - childSize.Width()) * HALF;
        padding_.SetLeft(Dimension(paddingTopBottom, DimensionUnit::PX));
        padding_.SetRight(Dimension(paddingTopBottom, DimensionUnit::PX));
    } else {
        double widthChange = std::max(0.0, NormalizeToPx(TEXT_MIN_WIDTH) - childSize.Width());
        if (NearEqual(widthChange, 0.0)) {
            // convert to px
            padding_.SetLeft(Dimension(NormalizeToPx(padding_.Left()), DimensionUnit::PX));
            padding_.SetRight(Dimension(NormalizeToPx(padding_.Right()), DimensionUnit::PX));
        } else {
            padding_.SetLeft(Dimension(widthChange * HALF, DimensionUnit::PX));
            padding_.SetRight(Dimension(widthChange * HALF, DimensionUnit::PX));
        }

        double paddingTopBottom = (selfSize.Height() - NormalizeToPx(TIP_SPACING) - childSize.Height()) * HALF;
        padding_.SetTop(Dimension(paddingTopBottom, DimensionUnit::PX));
        padding_.SetBottom(Dimension(paddingTopBottom, DimensionUnit::PX));
    }

}

} // namespace OHOS::Ace
