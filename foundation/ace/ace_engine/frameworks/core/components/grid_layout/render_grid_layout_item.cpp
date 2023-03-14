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

#include "core/components/grid_layout/render_grid_layout_item.h"

#include "base/utils/utils.h"
#include "core/components/grid_layout/grid_layout_item_component.h"
#include "core/components/grid_layout/render_grid_layout.h"

namespace OHOS::Ace {

RefPtr<RenderNode> RenderGridLayoutItem::Create()
{
    return AceType::MakeRefPtr<RenderGridLayoutItem>();
}

void RenderGridLayoutItem::Update(const RefPtr<Component>& component)
{
    const RefPtr<GridLayoutItemComponent> gridItem = AceType::DynamicCast<GridLayoutItemComponent>(component);
    if (!gridItem) {
        return;
    }
    SetColumnIndex(gridItem->GetColumnIndex());
    SetRowIndex(gridItem->GetRowIndex());
    SetColumnSpan(gridItem->GetColumnSpan());
    SetRowSpan(gridItem->GetRowSpan());
    SetForceRebuild(gridItem->ForceRebuild());
    InitAnimationController(GetContext());
    onSelectId_ = gridItem->GetOnSelectId();
    selectable_ = gridItem->GetSelectable();
    MarkNeedLayout();
}

void RenderGridLayoutItem::PerformLayout()
{
    if (GetChildren().empty()) {
        LOGE("RenderGridLayoutItem: no child found in RenderGridLayoutItem!");
    } else {
        LOGD("PerformLayout column:%{public}d row:%{public}d", columnIndex_, rowIndex_);
        auto child = GetChildren().front();
        child->Layout(GetLayoutParam());
        child->SetPosition(Offset::Zero());
        SetLayoutSize(child->GetLayoutSize());
    }
}

void RenderGridLayoutItem::HandleOnFocus()
{
    auto parent = GetParent().Upgrade();
    while (parent) {
        auto gridLayout = AceType::DynamicCast<RenderGridLayout>(parent);
        if (gridLayout) {
            gridLayout->UpdateFocusInfo(index_);
            break;
        }
        parent = parent->GetParent().Upgrade();
    }
}

void RenderGridLayoutItem::SetColumnIndex(int32_t columnIndex)
{
    if (columnIndex < 0) {
        LOGD("Invalid columnIndex %{public}d, use default", columnIndex);
        return;
    }
    columnIndex_ = columnIndex;
}

void RenderGridLayoutItem::SetRowIndex(int32_t rowIndex)
{
    if (rowIndex < 0) {
        LOGD("Invalid rowIndex %{public}d, use default", rowIndex);
        return;
    }
    rowIndex_ = rowIndex;
}

void RenderGridLayoutItem::SetColumnSpan(int32_t columnSpan)
{
    if (columnSpan <= 0) {
        LOGW("Invalid columnSpan %{public}d", columnSpan);
        return;
    }
    columnSpan_ = columnSpan;
}

void RenderGridLayoutItem::SetRowSpan(int32_t rowSpan)
{
    if (rowSpan <= 0) {
        LOGW("Invalid rowSpan %{public}d", rowSpan);
        return;
    }
    rowSpan_ = rowSpan;
}

void RenderGridLayoutItem::OnLongPressEvent()
{
    if (!OnItemLongPressed_) {
        LOGE("%{public}s OnItemLongPressed_ is null.", __PRETTY_FUNCTION__);
        return;
    }

    OnItemLongPressed_(index_, AceType::WeakClaim(this));
}

void RenderGridLayoutItem::SetOnItemLongPressed(const OnItemLongPressed& func)
{
    if (!func) {
        LOGE("%{public}s func is null.", __PRETTY_FUNCTION__);
    }
    if (!OnItemLongPressed_) {
        LOGI("%{public}s OnItemLongPressed_ is null before.", __PRETTY_FUNCTION__);
    }
    OnItemLongPressed_ = func;
    if (!OnItemLongPressed_) {
        LOGE("%{public}s OnItemLongPressed_ is null after.", __PRETTY_FUNCTION__);
    }
}

void RenderGridLayoutItem::InitAnimationController(const WeakPtr<PipelineContext>& context)
{
    if (!animationController_) {
        animationController_ = AceType::MakeRefPtr<Animator>(context);
        animationController_->SetDuration(ITEM_ANIMATION_DURATION);
    }
}

RefPtr<Animator> RenderGridLayoutItem::GetAnimationController()
{
    return animationController_;
}

bool RenderGridLayoutItem::AnimationAddInterpolator(const RefPtr<Animation<Point>>& animation)
{
    LOGD("%{public}s begin.", __PRETTY_FUNCTION__);
    if (animationController_) {
        if (animationController_->IsRunning()) {
            animationController_->ClearInterpolators();
            animationController_->ClearAllListeners();
            animationController_->Stop();
        }
        animationController_->AddInterpolator(animation);
        return true;
    }
    return false;
}

void RenderGridLayoutItem::AnimationPlay()
{
    LOGD("%{public}s begin.", __PRETTY_FUNCTION__);
    if (animationController_) {
        animationController_->Play();
    }
}
} // namespace OHOS::Ace
