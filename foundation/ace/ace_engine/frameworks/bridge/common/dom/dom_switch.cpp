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

#include "frameworks/bridge/common/dom/dom_switch.h"

#include "base/i18n/localization.h"
#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr Dimension BOX_HOVER_RADIUS = 8.0_vp;

} // namespace

DOMSwitch::DOMSwitch(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    switchChild_ = AceType::MakeRefPtr<SwitchComponent>(nullptr);
    switchChild_->SetTextOn(Localization::GetInstance()->GetEntryLetters("switch.on"));
    switchChild_->SetTextOff(Localization::GetInstance()->GetEntryLetters("switch.off"));
}

void DOMSwitch::InitializeStyle()
{
    ResetInitializedStyle();
}

void DOMSwitch::ResetInitializedStyle()
{
    RefPtr<SwitchTheme> theme = GetTheme<SwitchTheme>();
    if (theme) {
        switchChild_->ApplyTheme(theme);
    }
    switchChild_->SetTextOn(Localization::GetInstance()->GetEntryLetters("switch.on"));
    switchChild_->SetTextOff(Localization::GetInstance()->GetEntryLetters("switch.off"));
}

bool DOMSwitch::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SwitchComponent&, std::string&, std::string&)>
        switchAttrOperators[] = {
            { DOM_CHECKED, [](const std::string& val, SwitchComponent& textSwitch, std::string& textOn,
                               std::string& textOff) { textSwitch.SetValue(StringToBool(val)); } },
            { DOM_DISABLED, [](const std::string& val, SwitchComponent& textSwitch, std::string& textOn,
                                std::string& textOff) { textSwitch.SetDisabled(StringToBool(val)); } },
            { DOM_SHOW_TEXT, [](const std::string& val, SwitchComponent& textSwitch, std::string& textOn,
                                 std::string& textOff) { textSwitch.SetShowText(StringToBool(val)); } },
            { DOM_TEXT_OFF,
                [](const std::string& val, SwitchComponent& textSwitch, std::string& textOn, std::string& textOff) {
                    textSwitch.SetTextOff(val);
                    textOff = val;
                } },
            { DOM_TEXT_ON,
                [](const std::string& val, SwitchComponent& textSwitch, std::string& textOn, std::string& textOff) {
                    textSwitch.SetTextOn(val);
                    textOn = val;
                } },
        };
    auto operatorIter = BinarySearchFindIndex(
        switchAttrOperators, ArraySize(switchAttrOperators), attr.first.c_str());
    if (operatorIter != -1) {
        switchAttrOperators[operatorIter].value(attr.second, *switchChild_, textOn_, textOff_);
        return true;
    }
    return false;
}

bool DOMSwitch::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    static const LinearMapNode<void (*)(const std::string&, const DOMSwitch&, SwitchComponent&, TextStyle&)>
        switchTextOperators[] = {
            { DOM_TEXT_ALLOW_SCALE, [](const std::string& val, const DOMSwitch& node, SwitchComponent& textSwitch,
                                    TextStyle& textStyle) { textStyle.SetAllowScale(StringToBool(val)); } },
            { DOM_TEXT_FONT_FAMILY,
                [](const std::string& val, const DOMSwitch& node, SwitchComponent& textSwitch, TextStyle& textStyle) {
                    textStyle.SetFontFamilies(node.ParseFontFamilies(val));
                } },
            { DOM_TEXT_FONT_SIZE, [](const std::string& val, const DOMSwitch& node, SwitchComponent& textSwitch,
                                  TextStyle& textStyle) { textStyle.SetFontSize(node.ParseDimension(val)); } },
            { DOM_TEXT_FONT_STYLE, [](const std::string& val, const DOMSwitch& node, SwitchComponent& textSwitch,
                                   TextStyle& textStyle) { textStyle.SetFontStyle(ConvertStrToFontStyle(val)); } },
            { DOM_TEXT_FONT_WEIGHT, [](const std::string& val, const DOMSwitch& node, SwitchComponent& textSwitch,
                                    TextStyle& textStyle) { textStyle.SetFontWeight(ConvertStrToFontWeight(val)); } },
            { DOM_TEXT_LETTER_SPACING,
                [](const std::string& val, const DOMSwitch& node, SwitchComponent& textSwitch, TextStyle& textStyle) {
                    textStyle.SetLetterSpacing(node.ParseDimension(val)); } },
            { DOM_TEXT_DECORATION,
                [](const std::string& val, const DOMSwitch& node, SwitchComponent& textSwitch, TextStyle& textStyle) {
                    textStyle.SetTextDecoration(ConvertStrToTextDecoration(val));
                } },
            { DOM_TEXT_PADDING, [](const std::string& val, const DOMSwitch& node, SwitchComponent& textSwitch,
                                TextStyle& textStyle) { textSwitch.SetTextPadding(node.ParseDimension(val)); } },
            { DOM_TEXT_OFF_COLOR, [](const std::string& val, const DOMSwitch& node, SwitchComponent& textSwitch,
                                  TextStyle& textStyle) { textSwitch.SetTextColorOff(node.ParseColor(val)); } },
            { DOM_TEXT_ON_COLOR, [](const std::string& val, const DOMSwitch& node, SwitchComponent& textSwitch,
                                 TextStyle& textStyle) { textSwitch.SetTextColorOn(node.ParseColor(val)); } },
        };
    auto operatorIter = BinarySearchFindIndex(switchTextOperators, ArraySize(switchTextOperators), style.first.c_str());
    if (operatorIter != -1) {
        switchTextOperators[operatorIter].value(style.second, *this, *switchChild_, textStyle_);
        return true;
    }
    return false;
}

bool DOMSwitch::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    if (event == DOM_CHANGE) {
        changeEvent_ = EventMarker(GetNodeIdForEvent(), event, pageId);
        switchChild_->SetChangeEvent(changeEvent_);
        return true;
    } else if (event == DOM_CLICK) {
        EventMarker eventMarker(GetNodeIdForEvent(), event, pageId);
        eventMarker.SetCatchMode(false);
        switchChild_->SetClickEvent(eventMarker);
    } else if (event == DOM_CATCH_BUBBLE_CLICK) {
        EventMarker eventMarker(GetNodeIdForEvent(), event, pageId);
        eventMarker.SetCatchMode(true);
        switchChild_->SetClickEvent(eventMarker);
    }
    return false;
}

void DOMSwitch::PrepareCheckedListener()
{
    auto weak = AceType::WeakClaim(this);
    auto checkableChangeCallback = [weak](const std::string& checked) {
        auto domNode = weak.Upgrade();
        if (!domNode) {
            LOGE("get dom node failed!");
            return;
        }
        bool isChecked = checked.find("\"checked\":true") != std::string::npos;
        std::string checkedString = isChecked ? "true" : "false";
        domNode->SetSpecializedAttr(std::make_pair(DOM_CHECKED, checkedString));
        domNode->OnChecked(isChecked);
    };
    auto checkableChangeMarker = BackEndEventManager<void(const std::string&)>::GetInstance().GetAvailableMarker();
    BackEndEventManager<void(const std::string&)>::GetInstance().BindBackendEvent(
        checkableChangeMarker, checkableChangeCallback);
    switchChild_->SetDomChangeEvent(checkableChangeMarker);
}

void DOMSwitch::PrepareSpecializedComponent()
{
    if (boxComponent_) {
        boxComponent_->SetMouseAnimationType(HoverAnimationType::OPACITY);
        if (!boxComponent_->GetBackDecoration()) {
            RefPtr<Decoration> backDecoration = AceType::MakeRefPtr<Decoration>();
            backDecoration->SetBorderRadius(Radius(BOX_HOVER_RADIUS));
            boxComponent_->SetBackDecoration(backDecoration);
        }
        RefPtr<SwitchTheme> theme = GetTheme<SwitchTheme>();
        if (switchChild_->GetShowText()) {
            return;
        }
        if (boxComponent_->GetHeightDimension().Value() < 0.0 && theme) {
            boxComponent_->SetHeight(theme->GetHeight().Value(), theme->GetHeight().Unit());
        }
        if (boxComponent_->GetWidthDimension().Value() < 0.0 && theme) {
            boxComponent_->SetWidth(theme->GetWidth().Value(), theme->GetWidth().Unit());
        }
    }
    if (HasCheckedPseudo()) {
        PrepareCheckedListener();
    }
    switchChild_->SetTextDirection(IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    switchChild_->SetTextStyle(textStyle_);
    if (!textOn_.empty()) {
        switchChild_->SetTextOn(textOn_);
    }
    if (!textOff_.empty()) {
        switchChild_->SetTextOff(textOff_);
    }
#ifndef WEARABLE_PRODUCT
    if (declaration_) {
        auto& multimodalAttr =
            static_cast<CommonMultimodalAttribute&>(declaration_->GetAttribute(AttributeTag::COMMON_MULTIMODAL_ATTR));
        if (multimodalAttr.IsValid() && !multimodalAttr.IsUnavailable() && multimodalAttr.scene == SceneLabel::SWITCH) {
            switchChild_->SetMultimodalProperties(multimodalAttr);
        }
    }
#endif
}

} // namespace OHOS::Ace::Framework
