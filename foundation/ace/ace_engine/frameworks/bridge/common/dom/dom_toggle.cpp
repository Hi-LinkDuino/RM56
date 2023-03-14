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

#include "frameworks/bridge/common/dom/dom_toggle.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMToggle::DOMToggle(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    toggleChild_ = AceType::MakeRefPtr<ToggleComponent>();
    textChild_ = AceType::MakeRefPtr<TextComponent>("");
    paddingChild_ = AceType::MakeRefPtr<PaddingComponent>();
    paddingChild_->SetChild(textChild_);
    toggleChild_->SetChild(paddingChild_);
}

void DOMToggle::InitializeStyle()
{
    ResetInitializedStyle();
}

void DOMToggle::ResetInitializedStyle()
{
    toggleTheme_ = GetTheme<ToggleTheme>();
    if (!toggleTheme_) {
        LOGE("toggleTheme is null");
        return;
    }
    paddingChild_->SetPadding(toggleTheme_->GetPadding());
    toggleChild_->SetBackgroundColor(toggleTheme_->GetBackgroundColor());
    toggleChild_->SetCheckedColor(toggleTheme_->GetCheckedColor());
    toggleChild_->SetPressedBlendColor(toggleTheme_->GetPressedBlendColor());
    textStyle_ = toggleTheme_->GetTextStyle();
    textStyle_.SetMaxLines(1);
    textStyle_.SetTextOverflow(TextOverflow::ELLIPSIS);
    textStyle_.SetTextAlign(TextAlign::LEFT);
    blendOpacity_ = toggleTheme_->GetDisabledAlpha();
}

bool DOMToggle::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(DOMToggle&, const std::string&)> toggleAttrOperators[] = {
        { DOM_TOGGLE_CHECKED_STATE,
            [](DOMToggle& toggle, const std::string& value) {
                toggle.toggleChild_->SetCheckedState(StringToBool(value));
            } },
        { DOM_DISABLED, [](DOMToggle& toggle, const std::string& value) { toggle.isDisabled_ = StringToBool(value); } },
        { DOM_TEXT_VALUE, [](DOMToggle& toggle, const std::string& value) { toggle.textChild_->SetData(value); } },
    };
    auto operatorIter = BinarySearchFindIndex(toggleAttrOperators, ArraySize(toggleAttrOperators), attr.first.c_str());
    if (operatorIter != -1) {
        toggleAttrOperators[operatorIter].value(*this, attr.second);
        return true;
    }
    return false;
}

bool DOMToggle::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    const static LinearMapNode<void (*)(DOMToggle&, const std::string&)> toggleStyleOperators[] = {
        { DOM_TEXT_ALLOW_SCALE,
            [](DOMToggle& toggle, const std::string& value) { toggle.textStyle_.SetAllowScale(StringToBool(value)); } },
        { DOM_TOGGLE_BACKGROUND_COLOR,
            [](DOMToggle& toggle, const std::string& value) {
                if (toggle.HasCheckedPseudo()) {
                    toggle.toggleChild_->SetCheckedColor(toggle.ParseColor(value));
                } else {
                    toggle.toggleChild_->SetBackgroundColor(toggle.ParseColor(value));
                }
            } },
        { DOM_TOGGLE_CHECKED_COLOR,
            [](DOMToggle& toggle, const std::string& value) {
                toggle.toggleChild_->SetCheckedColor(toggle.ParseColor(value));
            } },
        { DOM_TEXT_FONT_FAMILY,
            [](DOMToggle& toggle, const std::string& value) {
                toggle.textStyle_.SetFontFamilies(toggle.ParseFontFamilies(value));
            } },
        { DOM_TEXT_FONT_SIZE,
            [](DOMToggle& toggle, const std::string& value) {
                toggle.textStyle_.SetFontSize(toggle.ParseDimension(value));
                toggle.toggleChild_->SetFontDefinedState(true);
            } },
        { DOM_TEXT_FONT_STYLE,
            [](DOMToggle& toggle, const std::string& value) {
                toggle.textStyle_.SetFontStyle(ConvertStrToFontStyle(value));
            } },
        { DOM_TEXT_FONT_WEIGHT,
            [](DOMToggle& toggle, const std::string& value) {
                toggle.textStyle_.SetFontWeight(ConvertStrToFontWeight(value));
            } },
        { DOM_TOGGLE_HEIGHT,
            [](DOMToggle& toggle, const std::string& value) {
                toggle.toggleChild_->SetHeight(toggle.ParseDimension(value));
            } },
        { DOM_TOGGLE_TEXT_COLOR,
            [](DOMToggle& toggle, const std::string& value) {
                toggle.textStyle_.SetTextColor(toggle.ParseColor(value));
            } },
        { DOM_TOGGLE_WIDTH,
            [](DOMToggle& toggle, const std::string& value) {
                toggle.toggleChild_->SetWidth(toggle.ParseDimension(value));
            } },
    };
    auto operatorIter =
        BinarySearchFindIndex(toggleStyleOperators, ArraySize(toggleStyleOperators), style.first.c_str());
    if (operatorIter != -1) {
        toggleStyleOperators[operatorIter].value(*this, style.second);
        return true;
    }
    return false;
}

bool DOMToggle::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    static const LinearMapNode<void (*)(DOMToggle&, const EventMarker&)> toggleEventOperators[] = {
        { DOM_CATCH_BUBBLE_CLICK,
            [](DOMToggle& toggle, const EventMarker& event) {
                EventMarker eventMarker(event);
                eventMarker.SetCatchMode(true);
                toggle.toggleChild_->SetClickEvent(eventMarker);
            } },
        { DOM_CHANGE, [](DOMToggle& toggle, const EventMarker& event) { toggle.toggleChild_->SetChangeEvent(event); } },
        { DOM_CLICK,
            [](DOMToggle& toggle, const EventMarker& event) {
                EventMarker eventMarker(event);
                eventMarker.SetCatchMode(false);
                toggle.toggleChild_->SetClickEvent(eventMarker);
            } },
    };
    auto operatorIter = BinarySearchFindIndex(toggleEventOperators, ArraySize(toggleEventOperators), event.c_str());
    if (operatorIter != -1) {
        toggleEventOperators[operatorIter].value(*this, EventMarker(GetNodeIdForEvent(), event, pageId));
        return true;
    }
    return false;
}

void DOMToggle::PrepareSpecializedComponent()
{
    textChild_->SetTextDirection(IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    ResetColorStyle();
    if (isDisabled_) {
        PrepareDisabledStyle();
    }
    textStyle_.SetAdaptTextSize(textStyle_.GetFontSize(), textStyle_.GetFontSize());
    textChild_->SetTextStyle(textStyle_);
    textChild_->SetFocusColor(textStyle_.GetTextColor());
    if (!boxComponent_) {
        return;
    }
    boxComponent_->SetDeliverMinToChild(true);
    if (!NearZero(toggleChild_->GetWidth().Value())) {
        boxComponent_->SetWidth(toggleChild_->GetWidth().Value(), toggleChild_->GetWidth().Unit());
    }
    if (!NearZero(toggleChild_->GetHeight().Value())) {
        boxComponent_->SetHeight(toggleChild_->GetHeight().Value(), toggleChild_->GetHeight().Unit());
    } else {
        if (!toggleChild_->GetFontDefinedState()) {
            boxComponent_->SetHeight(toggleTheme_->GetHeight().Value(), toggleTheme_->GetHeight().Unit());
        }
    }
    auto backDecoration = boxComponent_->GetBackDecoration();
    if (backDecoration) {
        if (backDecoration->GetImage()) {
            toggleChild_->SetBackgroundColor(Color::TRANSPARENT);
        }
        backDecoration->SetBorderRadius(Radius(boxComponent_->GetHeightDimension() / 2.0));
    }

    // Set the padding on the box to the toggle
    auto edge = boxComponent_->GetPadding();
    if (edge == Edge::NONE) {
        return;
    }
    paddingChild_->SetPadding(edge);
    boxComponent_->SetPadding(Edge());
}

void DOMToggle::ResetColorStyle()
{
    if (!colorChanged_) {
        textColor_ = textStyle_.GetTextColor();
        backgroundColor_ = toggleChild_->GetBackgroundColor();
        checkedColor_ = toggleChild_->GetCheckedColor();
    }
    textStyle_.SetTextColor(textColor_);
    toggleChild_->SetBackgroundColor(backgroundColor_);
    toggleChild_->SetCheckedColor(checkedColor_);
}

void DOMToggle::PrepareDisabledStyle()
{
    if (HasDisabledPseudo()) {
        return;
    }
    textStyle_.SetTextColor(textStyle_.GetTextColor().BlendOpacity(blendOpacity_));
    toggleChild_->SetBackgroundColor(toggleChild_->GetBackgroundColor().BlendOpacity(blendOpacity_));
    toggleChild_->SetCheckedColor(toggleChild_->GetCheckedColor().BlendOpacity(blendOpacity_));
    colorChanged_ = true;
}

} // namespace OHOS::Ace::Framework
