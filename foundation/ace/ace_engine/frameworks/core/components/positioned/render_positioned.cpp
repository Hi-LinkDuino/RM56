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

#include "core/components/positioned/render_positioned.h"

#include "core/components/positioned/positioned_component.h"

namespace OHOS::Ace {

void RenderPositioned::Update(const RefPtr<Component>& component)
{
    const auto positioned = AceType::DynamicCast<PositionedComponent>(component);
    if (!positioned) {
        return;
    }
    bottom_ = positioned->GetBottom();
    top_ = positioned->GetTop();
    left_ = positioned->GetLeft();
    right_ = positioned->GetRight();
    width_ = positioned->GetWidth();
    height_ = positioned->GetHeight();
    hasLeft_ = positioned->HasLeft();
    hasRight_ = positioned->HasRight();
    hasTop_ = positioned->HasTop();
    hasBottom_ = positioned->HasBottom();
    updatePositionFunc_ = positioned->GetUpdatePositionFuncId();

    auto updatePosition = [weak = AceType::WeakClaim(this)](const Dimension& x, const Dimension& y) {
        auto renderPosition = weak.Upgrade();
        if (!renderPosition) {
            return;
        }
        renderPosition->SetTop(y);
        renderPosition->SetLeft(x);
    };

    if (updatePositionFunc_) {
        updatePositionFunc_(updatePosition);
    }
    MarkNeedLayout();
}

void RenderPositioned::SetLeft(const Dimension& left)
{
    if (NearEqual(left_.Value(), left.Value()) && (left_.Unit() == left.Unit())) {
        return;
    }
    left_ = left;
    hasLeft_ = true;
    MarkNeedLayout();
}

void RenderPositioned::SetTop(const Dimension& top)
{
    if (NearEqual(top_.Value(), top.Value()) && (top_.Unit() == top.Unit())) {
        return;
    }
    top_ = top;
    hasTop_ = true;
    MarkNeedLayout();
}

void RenderPositioned::SetRight(const Dimension& right)
{
    if (NearEqual(right_.Value(), right.Value()) && (right_.Unit() == right.Unit())) {
        return;
    }
    right_ = right;
    hasRight_ = true;
    MarkNeedLayout();
}

void RenderPositioned::SetBottom(const Dimension& bottom)
{
    if (NearEqual(bottom_.Value(), bottom.Value()) && (bottom_.Unit() == bottom.Unit())) {
        return;
    }
    bottom_ = bottom;
    hasBottom_ = true;
    MarkNeedLayout();
}

} // namespace OHOS::Ace
