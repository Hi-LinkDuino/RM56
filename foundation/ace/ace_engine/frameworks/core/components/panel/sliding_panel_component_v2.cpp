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

#include "core/components/panel/sliding_panel_component_v2.h"
#include "core/components/box/box_component.h"
#include "core/components/drag_bar/drag_bar_component.h"
#include "core/components/drop_filter/drop_filter_component.h"
#include "core/components/flex/flex_component.h"
#include "core/components/flex/flex_item_component.h"
#include "core/components/focus_collaboration/focus_collaboration_component.h"
#include "core/components/panel/render_sliding_panel.h"
#include "core/components/panel/sliding_panel_element.h"

namespace OHOS::Ace {
namespace {

const auto DRAG_BAR_RADIUS = 24.0_vp;
const Color BG_COLOR = Color(0xfff1f3f5);

} // namespace

void SlidingPanelComponentV2::OnChildAdded(const RefPtr<Component>& child)
{
    auto panel = AceType::MakeRefPtr<PanelComponent>("PanelComponent", "Wrapper", child);
    if (type_ == PanelType::TEMP_DISPLAY && mode_ == PanelMode::MINI) {
        mode_ = PanelMode::HALF;
    } else if (type_ == PanelType::MINI_BAR && mode_ == PanelMode::HALF) {
        mode_ = PanelMode::MINI;
    }
    panel->SetPanelMode(mode_);
    panel->SetPanelType(type_);
    panel->SetHasBoxStyle(hasBoxStyle_);
    BuildInnerChild();
}

// Build SlidingPanel structure
void SlidingPanelComponentV2::BuildInnerChild()
{
    auto boxForContent = AceType::MakeRefPtr<BoxComponent>();
    if (hasBoxStyle_) {
        boxForContent->SetPadding(boxStyle_->GetPadding());
        boxForContent->SetMargin(boxStyle_->GetMargin());
    }
    boxForContent->SetFlex(BoxFlex::FLEX_X);
    auto dragBar = AceType::MakeRefPtr<DragBarComponent>();
    dragBar->SetPanelMode(mode_);
    dragBar->SetHasDragBar(hasDragBar_);
    auto backDecoration = AceType::MakeRefPtr<Decoration>();
    auto frontDecoration = AceType::MakeRefPtr<Decoration>();
    if (hasDecorationStyle_) {
        if (boxStyle_->GetBackDecoration()) {
            backDecoration = boxStyle_->GetBackDecoration();
        }
        if (boxStyle_->GetFrontDecoration()) {
            frontDecoration = boxStyle_->GetFrontDecoration();
        }
    }
    if (!hasBorderStyle_) {
        Border border;
        border.SetTopLeftRadius(Radius(DRAG_BAR_RADIUS));
        border.SetTopRightRadius(Radius(DRAG_BAR_RADIUS));
        backDecoration->SetBorder(border);
    }
    if (!hasBgStyle_ && !backDecoration->GetImage()) {
        backDecoration->SetBackgroundColor(BG_COLOR);
    }
    auto column =
        AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, std::list<RefPtr<Component>>());
    column->SetCrossAxisSize(CrossAxisSize::MAX);
    column->SetTextDirection(GetTextDirection());
    auto flexItem = AceType::MakeRefPtr<FlexItemComponent>(1, 1, 0);
    flexItem->SetAlignSelf(FlexAlign::FLEX_START);
    flexItem->SetChild(GetChild());
    column->AppendChild(dragBar);
    column->AppendChild(flexItem);
    boxForContent->SetBackDecoration(backDecoration);
    boxForContent->SetFrontDecoration(frontDecoration);
    boxForContent->SetChild(column);
    SetChildDirectly(boxForContent);

    Component::MergeRSNode(boxForContent, column);
}

} // namespace OHOS::Ace