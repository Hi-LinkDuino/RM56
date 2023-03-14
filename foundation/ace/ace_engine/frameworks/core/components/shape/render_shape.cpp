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

#include "core/components/shape/render_shape.h"

#include "base/log/event_report.h"

namespace OHOS::Ace {

void RenderShape::Update(const RefPtr<Component>& component)
{
    auto shapeComponent = AceType::DynamicCast<ShapeComponent>(component);
    if (!shapeComponent) {
        LOGE("RenderShape update with nullptr");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }
    shapeType_ = shapeComponent->GetShapeType();
    topLeftRadius_ = shapeComponent->GetTopLeftRadius();
    topRightRadius_ = shapeComponent->GetTopRightRadius();
    bottomRightRadius_ = shapeComponent->GetBottomRightRadius();
    bottomLeftRadius_ = shapeComponent->GetBottomLeftRadius();
    start_ = shapeComponent->GetStart();
    end_ = shapeComponent->GetEnd();
    fillState_ = shapeComponent->GetFillState();
    strokeState_ = shapeComponent->GetStrokeState();
    antiAlias_ = shapeComponent->GetAntiAlias();
    pathCmd_ = shapeComponent->GetPathCmd();
    points_ = shapeComponent->GetPoints();
    NormalToPxOfShape(shapeComponent->GetWidth(), width_);
    NormalToPxOfShape(shapeComponent->GetHeight(), height_);
    MarkNeedLayout();
}

void RenderShape::PerformLayout()
{
    auto size = CalcSize();
    if (size.IsValid()) {
        size = GetLayoutParam().Constrain(size);
    }
    SetLayoutSize(size);
}

void RenderShape::OnAnimationCallback()
{
    CalcSize();
}

void RenderShape::NormalToPxOfShape(AnimatableDimension sizeFromComponent, AnimatableDimension& sizeOfCurrent)
{
    if (sizeFromComponent.Unit() == DimensionUnit::PERCENT || sizeFromComponent.Unit() == DimensionUnit::PX) {
        sizeOfCurrent = sizeFromComponent;
    } else {
        auto context = context_.Upgrade();
        if (!context) {
            return;
        }
        sizeOfCurrent.SetValue(context->NormalizeToPx(sizeFromComponent));
        sizeOfCurrent.SetUnit(DimensionUnit::PX);
    }
}

void RenderShape::NotifySizeTransition(const AnimationOption& option, Size fromSize, Size toSize, int32_t nodeId)
{
    RenderNode::NotifySizeTransition(option, fromSize, toSize, nodeId);
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    if (option.IsValid()) {
        auto optionOrigin = context->GetExplicitAnimationOption();
        context->SaveExplicitAnimationOption(option);
        width_.MoveTo(fromSize.Width());
        height_.MoveTo(fromSize.Height());
        width_ = AnimatableDimension(toSize.Width());
        height_ = AnimatableDimension(toSize.Height());
        context->SaveExplicitAnimationOption(optionOrigin);
        CalcSize();
    }
}

} // namespace OHOS::Ace
