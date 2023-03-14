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

#include "core/components/tool_bar/render_tool_bar.h"

#include "core/components/tool_bar/tool_bar_component.h"

namespace OHOS::Ace {

RenderToolBar::RenderToolBar() : RenderNode(true) {}

void RenderToolBar::Update(const RefPtr<Component>& component)
{
    RefPtr<ToolBarComponent> toolBar = AceType::DynamicCast<ToolBarComponent>(component);
    if (!toolBar) {
        LOGE("ToolBarComponent is nullptr");
        return;
    }

    toolBarsSize_ = static_cast<int32_t>(toolBar->GetChildren().size());
}

void RenderToolBar::PerformLayout()
{
    toolBarWidth_ = GetLayoutParam().GetMaxSize().Width();
    toolBarHeight_ = 0.0;
    actualWidth_ = 0.0;

    LayoutChildren();
    Size layoutSize = GetLayoutParam().Constrain(Size(toolBarWidth_, toolBarHeight_));
    SetLayoutSize(layoutSize);
}

void RenderToolBar::LayoutChildren()
{
    if (toolBarsSize_ < 1) {
        return;
    }
    const std::list<RefPtr<RenderNode>>& children = GetChildren();
    LayoutParam innerLayoutParam = MakeInnerLayoutParam();

    auto item = children.begin();
    for (; item != children.end(); ++item) {
        (*item)->Layout(innerLayoutParam);
        toolBarHeight_ = std::max((*item)->GetLayoutSize().Height(), toolBarHeight_);
        (*item)->SetPosition(Offset(actualWidth_, (*item)->GetPosition().GetY()));
        actualWidth_ += (*item)->GetLayoutSize().Width();
    }
}

LayoutParam RenderToolBar::MakeInnerLayoutParam() const
{
    LayoutParam innerLayout = GetLayoutParam();
    auto gridSizeType = GridSystemManager::GetInstance().GetCurrentSize();
    auto columnInfo = GridSystemManager::GetInstance().GetInfoByType(GridColumnType::TOOL_BAR);
    columnInfo->GetParent()->BuildColumnWidth(innerLayout.GetMaxSize().Width());
    double toolBarMaxWidth = GetMaxWidthBasedOnGridType(columnInfo, gridSizeType);
    double toolBarMinWidth = std::max(0.0, innerLayout.GetMinSize().Width());
    if (toolBarsSize_ > 1) {
        toolBarMinWidth = toolBarMinWidth / toolBarsSize_;
        toolBarMaxWidth = toolBarMaxWidth / toolBarsSize_;
    }
    innerLayout.SetMinSize(Size(std::max(toolBarMinWidth, 0.0), std::max(0.0, 0.0)));
    innerLayout.SetMaxSize(Size(std::max(toolBarMaxWidth, 0.0), std::max(GetLayoutParam().GetMaxSize().Height(), 0.0)));

    return innerLayout;
}

double RenderToolBar::GetMaxWidthBasedOnGridType(
    const RefPtr<GridColumnInfo>& info, GridSizeType type) const
{
    double margin = NormalizeToPx(info->GetParent()->GetMarginLeft());
    if (type == GridSizeType::SM) {
        double width = info->GetWidth(4);
        if (toolBarsSize_ > 3) {
            width += margin * 2;
        } else {
            actualWidth_ += margin;
        }
        return width;
    } else if (type == GridSizeType::MD) {
        actualWidth_ += margin;
        if (toolBarsSize_ <= 3) {
            double dipScale = NormalizeToPx(info->GetParent()->GetGutterWidth());
            actualWidth_ += info->GetWidth(1) + dipScale;
            return info->GetWidth(6);
        } else {
            return info->GetWidth(8);
        }
    } else {
        LOGI("GetMaxWidthBasedOnGridType is undefined");
        return info->GetMaxWidth();
    }
}

} // namespace OHOS::Ace
