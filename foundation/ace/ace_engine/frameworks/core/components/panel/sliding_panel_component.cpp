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

#include "core/components/panel/sliding_panel_component.h"

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
const Color BLACK_BG = Color(0x33000000);

} // namespace

RefPtr<Element> SlidingPanelComponent::CreateElement()
{
    return AceType::MakeRefPtr<SlidingPanelElement>();
}

RefPtr<Component> SlidingPanelComponent::Create(const RefPtr<PanelComponent>& component)
{
    auto child = component->GetChild();
    auto hasBoxStyle = component->HasBoxStyle();
    auto boxStyle = component->GetBoxStyle();
    auto slidingPanel = AceType::MakeRefPtr<SlidingPanelComponent>();
    slidingPanel->SetOnSizeChanged(component->GetOnSizeChanged());
    slidingPanel->SetChild(child);
    slidingPanel->SetHasBoxStyle(hasBoxStyle);
    slidingPanel->SetType(component->GetPanelType());
    slidingPanel->SetFullHeight(component->GetFullHeight());
    slidingPanel->SetHalfHeight(component->GetHalfHeight());
    slidingPanel->SetMiniHeight(component->GetMiniHeight());
    slidingPanel->BuildInnerChild(boxStyle, component);
    slidingPanel->SetPanelId(StringUtils::StringToInt(component->GetId()));
    auto box = AceType::MakeRefPtr<BoxComponent>();
    box->SetChild(slidingPanel);
    Component::MergeRSNode(box, slidingPanel);
    auto decoration = AceType::MakeRefPtr<Decoration>();
    if (component->GetPanelType() == PanelType::TEMP_DISPLAY) {
        decoration->SetBackgroundColor(BLACK_BG);
    }
    box->SetBackDecoration(decoration);
    auto focusCollaboration = AceType::MakeRefPtr<FocusCollaborationComponent>();
    focusCollaboration->InsertChild(0, box);
    auto dropFilter = AceType::MakeRefPtr<DropFilterComponent>(focusCollaboration, 0, 0);
    if (component->GetPanelType() == PanelType::TEMP_DISPLAY) {
        dropFilter->SetSigmaX(0.5);
        dropFilter->SetSigmaY(0.5);
        dropFilter->SetInterceptEvent(true);
    }
    dropFilter->SetUsePanelTouchRect(true);
    return dropFilter;
}

RefPtr<RenderNode> SlidingPanelComponent::CreateRenderNode()
{
    return RenderSlidingPanel::Create();
}

// Build SlidingPanel structure
void SlidingPanelComponent::BuildInnerChild(const RefPtr<BoxComponent>& boxStyle, const RefPtr<PanelComponent>& panel)
{
    auto mode = panel->GetPanelMode();
    auto hasBoxStyle = panel->HasBoxStyle();
    auto hasDecoration = panel->HasDecorationStyle();
    auto textDirection = panel->GetTextDirection();
    auto hasDragBar = panel->HasDragBar();
    mode_ = mode;
    auto boxForContent = AceType::MakeRefPtr<BoxComponent>();
    if (hasBoxStyle) {
        boxForContent->SetPadding(boxStyle->GetPadding());
        boxForContent->SetMargin(boxStyle->GetMargin());
    }
    boxForContent->SetFlex(BoxFlex::FLEX_X);
    auto dragBar = AceType::MakeRefPtr<DragBarComponent>();
    dragBar->SetPanelMode(mode);
    dragBar->SetHasDragBar(hasDragBar);
    auto backDecoration = AceType::MakeRefPtr<Decoration>();
    auto frontDecoration = AceType::MakeRefPtr<Decoration>();
    if (hasDecoration) {
        if (boxStyle->GetBackDecoration()) {
            backDecoration = boxStyle->GetBackDecoration();
        }
        if (boxStyle->GetFrontDecoration()) {
            frontDecoration = boxStyle->GetFrontDecoration();
        }
    }
    if (!panel->HasBorderStyle()) {
        Border border;
        border.SetTopLeftRadius(Radius(DRAG_BAR_RADIUS));
        border.SetTopRightRadius(Radius(DRAG_BAR_RADIUS));
        backDecoration->SetBorder(border);
    }
    if (!panel->HasBackgroundColor() && !backDecoration->GetImage()) {
        backDecoration->SetBackgroundColor(BG_COLOR);
    }
    auto column =
        AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, std::list<RefPtr<Component>>());
    column->SetCrossAxisSize(CrossAxisSize::MAX);
    column->SetTextDirection(textDirection);
    auto flexItem = AceType::MakeRefPtr<FlexItemComponent>(1, 1, 0);
    flexItem->SetAlignSelf(FlexAlign::FLEX_START);
    flexItem->SetChild(GetChild());
    column->AppendChild(dragBar);
    column->AppendChild(flexItem);
    boxForContent->SetBackDecoration(backDecoration);
    boxForContent->SetFrontDecoration(frontDecoration);
    boxForContent->SetChild(column);
    SetChild(boxForContent);

    Component::MergeRSNode(boxForContent, column);
}

} // namespace OHOS::Ace