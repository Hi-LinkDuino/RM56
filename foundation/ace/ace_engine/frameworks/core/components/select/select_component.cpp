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

#include "core/components/select/select_component.h"

#include "base/utils/string_utils.h"
#include "core/common/container.h"
#include "core/components/clip/clip_component.h"
#include "core/components/flex/flex_item_component.h"
#include "core/components/list/list_component.h"
#include "core/components/select/render_select.h"
#include "core/components/select/select_element.h"
#include "core/components/triangle/triangle_component.h"

namespace OHOS::Ace {
namespace {

constexpr uint32_t SELECT_ITSELF_TEXT_LINES = 1;

}

SelectComponent::SelectComponent() : SoleChildComponent()
{
    popup_ = AceType::MakeRefPtr<SelectPopupComponent>();
    // the round radius is half of height which is 40dp.
    border_.SetBorderRadius(Radius(20.0_vp));
}

void SelectComponent::InitTheme(const RefPtr<ThemeManager>& themeManager)
{
    if (!themeManager) {
        return;
    }
    popup_->InitTheme(themeManager);
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    popup_->SetSelectPopupId(GetNodeId());
#endif
    auto selectTheme = themeManager->GetTheme<SelectTheme>();
    if (!selectTheme) {
        return;
    }
    theme_ = selectTheme->clone();
    theme_->SetFontWeight(FontWeight::W500);

    innerPadding_ =
        Edge(8.0, 9.25, 8.0, 9.25, DimensionUnit::VP); // the best effect for round triangle.
}

bool SelectComponent::Initialize()
{
    if (!tipText_) {
        LOGE("can not initialize, tip text is null.");
        return false;
    }

    RefPtr<TriangleComponent> icon = AceType::MakeRefPtr<TriangleComponent>();
    icon->SetWidth(10.0_vp); // the with is 10dp from doc.
    icon->SetHeight(6.0_vp); // the height is 7dp from doc. reduce 1dp for triangle inner.

    RefPtr<BoxComponent> space = AceType::MakeRefPtr<BoxComponent>();
    space->SetDeliverMinToChild(false);
    space->SetAlignment(Alignment::CENTER);
    // the space between text and triangle is 8dp from doc.
    Edge spaceEdge(8.0, 0.0, 0.0, 0.0, DimensionUnit::VP);
    if (GetTextDirection() == TextDirection::RTL) {
        spaceEdge.SetRight(spaceEdge.Left());
        spaceEdge.SetLeft(0.0_vp);
    }
    space->SetPadding(spaceEdge);
    space->SetChild(icon);

    auto container = Container::Current();
    if (!container) {
        return false;
    }
    auto context = container->GetPipelineContext();
    if (!context) {
        return false;
    }
    TextStyle textStyle;
    if (context->GetIsDeclarative()) {
        textStyle = GetSelectStyle();
        if (disabled_) {
            textStyle.SetTextColor(theme_->GetDisabledColor());
        }
    } else {
        textStyle = tipText_->GetTextStyle();
        textStyle.SetFontSize(theme_->GetFontSize());
        textStyle.SetFontWeight(theme_->GetFontWeight());
        if (disabled_) {
            textStyle.SetTextColor(theme_->GetDisabledColor());
        } else {
            textStyle.SetTextColor(theme_->GetFontColor());
        }
        std::vector<std::string> fontFamilies;
        StringUtils::StringSpliter(theme_->GetFontFamily(), ',', fontFamilies);
        if (!fontFamilies.empty()) {
            textStyle.SetFontFamilies(fontFamilies);
        }
    }

    textStyle.SetAllowScale(theme_->IsAllowScale());
    textStyle.SetTextDecoration(theme_->GetTextDecoration());
    textStyle.SetMaxLines(SELECT_ITSELF_TEXT_LINES);
    textStyle.SetTextOverflow(TextOverflow::ELLIPSIS);
    tipText_->SetTextStyle(textStyle);
    tipText_->SetFocusColor(textStyle.GetTextColor());
    icon->SetColor(textStyle.GetTextColor());
    RefPtr<FlexItemComponent> textItem = AceType::MakeRefPtr<FlexItemComponent>(0.0, 1.0, 0.0, tipText_);

    RefPtr<RowComponent> row =
        AceType::MakeRefPtr<RowComponent>(FlexAlign::CENTER, FlexAlign::CENTER, std::list<RefPtr<Component>>());
    row->SetMainAxisSize(MainAxisSize::MIN);
    if (GetTextDirection() == TextDirection::RTL) {
        row->AppendChild(space);
        row->AppendChild(textItem);
    } else {
        row->AppendChild(textItem);
        row->AppendChild(space);
    }

    RefPtr<Decoration> backDecoration = AceType::MakeRefPtr<Decoration>();
    backDecoration->SetBackgroundColor(backgroundColor_);
    backDecoration->SetBorder(border_);

    RefPtr<BoxComponent> box = AceType::MakeRefPtr<BoxComponent>();
    box->SetDeliverMinToChild(false);
    box->SetAlignment(Alignment::CENTER);
    box->SetBackDecoration(backDecoration);
    box->SetPadding(innerPadding_);
    box->SetWidth(width_.Value(), width_.Unit());
    box->SetHeight(height_.Value(), height_.Unit());
    box->SetChild(AceType::MakeRefPtr<ClipComponent>(row));
    boxComponent_ = box;
    boxComponent_->SetMouseAnimationType(HoverAnimationType::OPACITY);
    SetChild(box);
    return true;
}

RefPtr<RenderNode> SelectComponent::CreateRenderNode()
{
    return AceType::MakeRefPtr<RenderSelect>();
}

RefPtr<Element> SelectComponent::CreateElement()
{
    LOGD("select: create element.");
    auto refPtr = AceType::MakeRefPtr<SelectElement>();
    flushRefreshCallback_ = [weakPtr = WeakPtr<SelectElement>(refPtr)]() {
        auto element = weakPtr.Upgrade();
        if (!element) {
            return;
        }
        element->FlushRefresh();
    };
    return refPtr;
}

void SelectComponent::SetClicked(bool clicked, const Color& pressColor)
{
    clicked_ = clicked;
    if (!boxComponent_) {
        LOGE("select: box is null, can not set background color when click changed.");
        return;
    }

    auto backDecoration = boxComponent_->GetBackDecoration();
    if (!backDecoration) {
        backDecoration = AceType::MakeRefPtr<Decoration>();
    }
    backDecoration->SetBackgroundColor(backgroundColor_.BlendColor(pressColor));
    boxComponent_->SetBackDecoration(backDecoration);
}

RefPtr<SelectPopupComponent> SelectComponent::GetPopup() const
{
    if (!popup_) {
        LOGE("select: popup is null now.");
        return nullptr;
    }

    return popup_;
}

RefPtr<BoxComponent> SelectComponent::GetBoxComponent() const
{
    if (!boxComponent_) {
        LOGE("select: box component of background color is null.");
        return nullptr;
    }

    return boxComponent_;
}

} // namespace OHOS::Ace
