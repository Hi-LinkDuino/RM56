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

#include "frameworks/bridge/common/dom/dom_menu.h"

#include <string>

#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "core/components/common/properties/color.h"
#include "core/components/option/option_component.h"
#include "core/components/select/select_theme.h"
#include "core/components/text/text_component.h"
#include "core/components/theme/theme_manager.h"
#include "frameworks/bridge/common/dom/dom_text.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMMenu::DOMMenu(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    menuChild_ = AceType::MakeRefPtr<MenuComponent>(std::to_string(nodeId), nodeName);
}

DOMMenu::~DOMMenu()
{
    if (!clickMarkerId_.IsEmpty()) {
        BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(clickMarkerId_);
    }
    if (!focusMarkerId_.IsEmpty()) {
        BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(focusMarkerId_);
    }
    if (!longPressMarkerId_.IsEmpty()) {
        BackEndEventManager<void()>::GetInstance().RemoveBackEndEvent(longPressMarkerId_);
    }
}

void DOMMenu::InitializeStyle()
{
    ResetInitializedStyle();
}

void DOMMenu::ResetInitializedStyle()
{
    menuChild_->InitTheme(GetThemeManager());
    titleStyle_ = menuChild_->GetTitleStyle();
}

void DOMMenu::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    if (!child) {
        return;
    }

    auto option = AceType::DynamicCast<OptionComponent>(child->GetSpecializedComponent());
    if (!option) {
        return;
    }

    menuChild_->RemoveOption(option);
}

void DOMMenu::PrepareSpecializedComponent()
{
    menuChild_->SetTextDirection((IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR));
    menuChild_->SetTitleStyle(titleStyle_);
}

bool DOMMenu::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    if (attr.first == DOM_TYPE) {
        isClickType_ = (attr.second == DOM_CLICK);
        return true;
    }

    if (attr.first == DOM_TITLE && !attr.second.empty()) {
        menuChild_->SetTitle(attr.second);
    }

    return true;
}

bool DOMMenu::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    static const LinearMapNode<void (*)(TextStyle&, const std::string&, const DOMMenu&)> menuStyleOperators[] = {
        { DOM_TEXT_ALLOW_SCALE, [](TextStyle& textStyle, const std::string& val,
                                const DOMMenu&) { textStyle.SetAllowScale(StringToBool(val)); } },
        { DOM_TEXT_FONT_FAMILY, [](TextStyle& textStyle, const std::string& val,
                                const DOMMenu& node) { textStyle.SetFontFamilies(node.ParseFontFamilies(val)); } },
        { DOM_TEXT_FONT_SIZE, [](TextStyle& textStyle, const std::string& val,
                              const DOMMenu& node) { textStyle.SetFontSize(node.ParseDimension(val)); } },
        { DOM_TEXT_FONT_STYLE, [](TextStyle& textStyle, const std::string& val,
                               const DOMMenu&) { textStyle.SetFontStyle(ConvertStrToFontStyle(val)); } },
        { DOM_TEXT_FONT_WEIGHT, [](TextStyle& textStyle, const std::string& val,
                                const DOMMenu&) { textStyle.SetFontWeight(ConvertStrToFontWeight(val)); } },
        { DOM_TEXT_LETTER_SPACING, [](TextStyle& textStyle, const std::string& val,
                                   const DOMMenu& node) { textStyle.SetLetterSpacing(node.ParseDimension(val)); } },
        { DOM_PICKER_TEXT_COLOR, // use define of picker which is the same "text-color"
            [](TextStyle& textStyle, const std::string& val, const DOMMenu& node) {
                textStyle.SetTextColor(node.ParseColor(val));
            } },
        { DOM_TEXT_DECORATION, [](TextStyle& textStyle, const std::string& val,
                               const DOMMenu&) { textStyle.SetTextDecoration(ConvertStrToTextDecoration(val)); } },
    };
    auto operatorIter = BinarySearchFindIndex(menuStyleOperators, ArraySize(menuStyleOperators), style.first.c_str());
    if (operatorIter != -1) {
        menuStyleOperators[operatorIter].value(titleStyle_, style.second, *this);
        return true;
    }
    return false;
}

bool DOMMenu::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    if (event == DOM_SELECTED) {
        menuChild_->SetOnSuccess(EventMarker(GetNodeIdForEvent(), event, pageId));
    } else if (event == DOM_CANCEL) {
        menuChild_->SetOnCancel(EventMarker(GetNodeIdForEvent(), event, pageId));
    } else {
        LOGE("unsupported event: %{public}s.", event.c_str());
        return false;
    }
    return true;
}

RefPtr<Component> DOMMenu::GetSpecializedComponent()
{
    return menuChild_;
}

void DOMMenu::BindIdNode(const RefPtr<DOMNode>& idNode)
{
    if (SystemProperties::GetDeviceType() == DeviceType::TV) {
        focusMarkerId_ = BackEndEventManager<void()>::GetInstance().GetAvailableMarker();
        BackEndEventManager<void()>::GetInstance().BindBackendEvent(focusMarkerId_, [weak = WeakClaim(this), idNode]() {
            if (!idNode || idNode->IsNodeDisabled()) {
                return;
            }
            auto domMenu = weak.Upgrade();
            if (!domMenu) {
                return;
            }
            const auto& targetCallback = domMenu->menuChild_->GetTargetCallback();
            const auto& targetId = idNode->GetRootComponent()->GetId();
            if (targetCallback) {
                targetCallback(targetId, Offset(0, 0));
            }
        });
        idNode->SetOnFocusClick(focusMarkerId_);
    }
    if (isClickType_) {
        clickMarkerId_ = BackEndEventManager<void(const ClickInfo&)>::GetInstance().GetAvailableMarker();
        BackEndEventManager<void(const ClickInfo&)>::GetInstance().BindBackendEvent(
            clickMarkerId_, [weak = WeakClaim(this), idNode](const ClickInfo& clickInfo) {
                auto domMenu = weak.Upgrade();
                if (!domMenu || !idNode) {
                    return;
                }
                const auto& targetCallback = domMenu->menuChild_->GetTargetCallback();
                const auto& targetId = idNode->GetRootComponent()->GetId();
                if (targetCallback) {
                    targetCallback(targetId, clickInfo.GetGlobalLocation());
                }
            });
        idNode->SetOnClick(clickMarkerId_);
    } else {
        longPressMarkerId_ =
            BackEndEventManager<void(const LongPressInfo&)>::GetInstance().GetAvailableMarker();
        BackEndEventManager<void(const LongPressInfo&)>::GetInstance().BindBackendEvent(
            longPressMarkerId_, [weak = WeakClaim(this), idNode](const LongPressInfo& longPressInfo) {
                auto domMenu = weak.Upgrade();
                if (!domMenu || !idNode) {
                    return;
                }
                const auto& targetCallback = domMenu->menuChild_->GetTargetCallback();
                const auto& targetId = idNode->GetRootComponent()->GetId();
                if (targetCallback) {
                    targetCallback(targetId, longPressInfo.GetGlobalLocation());
                }
            });
        idNode->SetOnLongPress(longPressMarkerId_);
    }
}

double DOMMenu::GetJsonDouble(
    const std::unique_ptr<JsonValue>& json, const std::string& name, double defaultValue) const
{
    if (json && json->IsArray() && json->GetArraySize() > 0) {
        std::unique_ptr<JsonValue> itemValue = json->GetArrayItem(0)->GetValue(name);
        if (itemValue && itemValue->IsNumber()) {
            return itemValue->GetDouble();
        }
    }

    return defaultValue;
}

void DOMMenu::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    if (method == "show") {
        std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(args);
        double x = GetJsonDouble(argsValue, "x", 0);
        double y = GetJsonDouble(argsValue, "y", 0);
        const auto& targetCallback = menuChild_->GetTargetCallback();
        if (targetCallback) {
            targetCallback("", Offset(x, y));
        }
    }
}

void DOMMenu::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    if (!child) {
        return;
    }

    auto option = AceType::DynamicCast<OptionComponent>(child->GetSpecializedComponent());
    if (!option) {
        return;
    }

    if (slot < 0) {
        menuChild_->AppendOption(option);
    } else {
        menuChild_->InsertOption(option, static_cast<uint32_t>(slot));
    }
}

} // namespace OHOS::Ace::Framework
