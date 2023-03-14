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

#include "core/components/drop_filter/render_drop_filter.h"

#include "core/components/drop_filter/drop_filter_component.h"

namespace OHOS::Ace {

RenderDropFilter::RenderDropFilter() : RenderNode(true) {}

void RenderDropFilter::Update(const RefPtr<Component>& component)
{
    const RefPtr<DropFilterComponent> dropFilter = AceType::DynamicCast<DropFilterComponent>(component);
    if (!dropFilter) {
        return;
    }
    sigmaX_ = dropFilter->GetSigmaX();
    sigmaY_ = dropFilter->GetSigmaY();
    usePanelTouchRect_ = dropFilter->UsePanelTouchRect();
    MarkNeedLayout();
}

void RenderDropFilter::PerformLayout()
{
    if (!GetChildren().empty()) {
        GetChildren().front()->Layout(GetLayoutParam());
        SetLayoutSize(GetChildren().front()->GetLayoutSize());
    } else {
        SetLayoutSize(Size(0, 0));
    }
}

void RenderDropFilter::UpdateTouchRect()
{
    if (!usePanelTouchRect_) {
        RenderNode::UpdateTouchRect();
        return;
    }
    touchRect_ = GetPaintRect();
    touchRectList_.emplace_back(touchRect_);
    SetTouchRectList(touchRectList_);
    auto box = GetChildren().front();
    if (!box) {
        return;
    }
    auto focus = box->GetChildren().front();
    if (!focus) {
        return;
    }
    auto panel = focus->GetChildren().front();
    if (!panel) {
        return;
    }
    panel->UpdateTouchRect();
    touchRect_ = panel->GetTouchRect();
    touchRectList_ = panel->GetTouchRectList();
}

const Rect& RenderDropFilter::GetTouchRect()
{
    if (!usePanelTouchRect_) {
        return RenderNode::GetTouchRect();
    }
    UpdateTouchRect();
    return touchRect_;
}

const std::vector<Rect>& RenderDropFilter::GetTouchRectList()
{
    if (!usePanelTouchRect_) {
        return RenderNode::GetTouchRectList();
    }
    UpdateTouchRect();
    return touchRectList_;
}

} // namespace OHOS::Ace
