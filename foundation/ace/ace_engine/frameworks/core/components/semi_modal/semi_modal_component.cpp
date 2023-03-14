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

#include "core/components/semi_modal/semi_modal_component.h"

#include "core/components/box/box_component.h"
#include "core/components/clip/clip_component.h"
#include "core/components/drag_bar/drag_bar_component.h"
#include "core/components/flex/flex_component.h"
#include "core/components/flex/flex_item_component.h"
#include "core/components/padding/padding_component.h"
#include "core/components/semi_modal/render_semi_modal.h"
#include "core/components/semi_modal/semi_modal_element.h"

namespace OHOS::Ace {
namespace {

const auto DRAG_BAR_RADIUS = 24.0_vp;

} // namespace

RefPtr<Component> SemiModalComponent::Create(
    RefPtr<Component> child, bool isFullScreen, int32_t modalHeight, uint32_t color)
{
    auto semiModal = AceType::MakeRefPtr<SemiModalComponent>(isFullScreen);
    semiModal->SetModalHeight(modalHeight);
    semiModal->SetChild(child);
    semiModal->BuildInnerChild(color);
    return semiModal;
}

RefPtr<Element> SemiModalComponent::CreateElement()
{
    return AceType::MakeRefPtr<SemiModalElement>();
}

RefPtr<RenderNode> SemiModalComponent::CreateRenderNode()
{
    return RenderSemiModal::Create();
}

// Build SemiModal FA structure
void SemiModalComponent::BuildInnerChild(uint32_t color)
{
    auto boxForContent = AceType::MakeRefPtr<BoxComponent>();
    auto dragBar = AceType::MakeRefPtr<DragBarComponent>();
    auto boxForNavigation = AceType::MakeRefPtr<BoxComponent>();
    auto backDecoration = AceType::MakeRefPtr<Decoration>();
    backDecoration->SetBackgroundColor(Color(color));
    Border border;
    border.SetTopLeftRadius(Radius(DRAG_BAR_RADIUS));
    border.SetTopRightRadius(Radius(DRAG_BAR_RADIUS));
    backDecoration->SetBorder(border);
    auto clip = AceType::MakeRefPtr<ClipComponent>(GetChild());
    auto column =
        AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, std::list<RefPtr<Component>>());
    auto flexItem = AceType::MakeRefPtr<FlexItemComponent>(1, 1, 0);
    flexItem->SetChild(clip);
    column->AppendChild(dragBar);
    column->AppendChild(flexItem);
    column->AppendChild(boxForNavigation);
    boxForContent->SetBackDecoration(backDecoration);
    boxForContent->SetFlex(BoxFlex::FLEX_X);
    boxForContent->SetAlignment(Alignment::CENTER);
    boxForContent->SetChild(column);
    SetChild(boxForContent);
}

} // namespace OHOS::Ace