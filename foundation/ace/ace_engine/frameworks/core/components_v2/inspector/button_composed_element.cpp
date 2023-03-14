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

#include "core/components_v2/inspector/button_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/button/button_element.h"
#include "core/components/common/layout/constants.h"
#include "core/components/text/render_text.h"
#include "core/components/text/text_element.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {

const std::unordered_map<std::string, std::function<std::string(const ButtonComposedElement&)>> CREATE_JSON_MAP {
    { "type", [](const ButtonComposedElement& inspector) { return inspector.GetButtonType(); } },
    { "stateEffect", [](const ButtonComposedElement& inspector) { return inspector.GetStateEffect(); } },
    { "label", [](const ButtonComposedElement& inspector) { return inspector.GetLabel(); } },
    { "fontSize", [](const ButtonComposedElement& inspector) { return inspector.GetFontSize(); } },
    { "fontWeight", [](const ButtonComposedElement& inspector) { return inspector.GetFontWeight(); } },
    { "fontColor", [](const ButtonComposedElement& inspector) { return inspector.GetFontColor(); } },
};

void ButtonComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("button_composed_element"));
    DumpLog::GetInstance().AddDesc(
        std::string("ButtonType: ").append(GetButtonType()));
    DumpLog::GetInstance().AddDesc(
        std::string("StateEffect: ").append(GetStateEffect()));
    DumpLog::GetInstance().AddDesc(
        std::string("Label: ").append(GetLabel()));
}

std::unique_ptr<JsonValue> ButtonComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string ButtonComposedElement::GetButtonType() const
{
    auto renderButton = GetRenderButton();
    auto type = renderButton ? renderButton->GetButtonType() : ButtonType::CAPSULE;
    return ConvertButtonTypeToString(type);
}

std::string ButtonComposedElement::GetStateEffect() const
{
    auto renderButton = GetRenderButton();
    auto stateEffect = renderButton ? renderButton->GetStateEffect() : true;
    return ConvertBoolToString(stateEffect);
}

std::string ButtonComposedElement::GetLabel() const
{
    auto node = GetInspectorNode(TextElement::TypeId());
    if (!node) {
        return "";
    }
    auto render = AceType::DynamicCast<RenderText>(node);
    if (!render) {
        return "";
    }
    return render->GetTextData();
}

std::string ButtonComposedElement::GetFontSize() const
{
    auto node = GetInspectorNode(TextElement::TypeId());
    if (!node) {
        return "";
    }
    auto render = AceType::DynamicCast<RenderText>(node);
    if (!render) {
        return "";
    }
    auto fontSize = render ? render->GetTextStyle().GetFontSize() : Dimension();
    return std::to_string(static_cast<int32_t>(fontSize.ConvertToVp()));
}

std::string ButtonComposedElement::GetFontWeight() const
{
    auto node = GetInspectorNode(TextElement::TypeId());
    if (!node) {
        return "";
    }
    auto render = AceType::DynamicCast<RenderText>(node);
    if (!render) {
        return "";
    }
    auto fontWeight = render ? render->GetTextStyle().GetFontWeight() : FontWeight::NORMAL;
    return ConvertWrapFontWeightToStirng(fontWeight);
}

std::string ButtonComposedElement::GetFontColor() const
{
    auto node = GetInspectorNode(TextElement::TypeId());
    if (!node) {
        return "";
    }
    auto render = AceType::DynamicCast<RenderText>(node);
    if (!render) {
        return "";
    }
    auto fontColor = render ? render->GetTextStyle().GetTextColor() : Color::BLACK;
    return fontColor.ColorToString();
}

std::string ButtonComposedElement::GetBackgroundColor() const
{
    auto renderButton = GetRenderButton();
    if (!renderButton) {
        return "None";
    }
    return renderButton->GetBackgroundColor().ColorToString();
}

RefPtr<RenderButton> ButtonComposedElement::GetRenderButton() const
{
    auto node = GetInspectorNode(ButtonElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderButton>(node);
    }
    return nullptr;
}

std::string ButtonComposedElement::ConvertButtonTypeToString(ButtonType buttonType) const
{
    std::string result = "";
    switch (buttonType) {
        case ButtonType::NORMAL:
            result = "ButtonType.Normal";
            break;
        case ButtonType::CAPSULE:
            result = "ButtonType.Capsule";
            break;
        case ButtonType::CIRCLE:
            result = "ButtonType.Circle";
            break;
        case ButtonType::ARC:
            result = "ButtonType.Arc";
            break;
        default:
            LOGD("input do not match any ButtonType");
    }
    return result;
}

void ButtonComposedElement::AddChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto buttonElement = GetContentElement<ButtonElement>(ButtonElement::TypeId());
    if (!buttonElement) {
        LOGE("get GetButtonElement failed");
        return;
    }
    buttonElement->UpdateChildWithSlot(nullptr, newComponent, slot, slot);
    buttonElement->MarkDirty();
}

void ButtonComposedElement::UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto buttonElement = GetContentElement<ButtonElement>(ButtonElement::TypeId());
    if (!buttonElement) {
        LOGE("get GetButtonElement failed");
        return;
    }
    auto child = buttonElement->GetChildBySlot(slot);
    buttonElement->UpdateChildWithSlot(child, newComponent, slot, slot);
    buttonElement->MarkDirty();
}

void ButtonComposedElement::DeleteChildWithSlot(int32_t slot)
{
    auto buttonElement = GetContentElement<ButtonElement>(ButtonElement::TypeId());
    if (!buttonElement) {
        LOGE("get GetButtonElement failed");
        return;
    }
    auto child = buttonElement->GetChildBySlot(slot);
    buttonElement->UpdateChildWithSlot(child, nullptr, slot, slot);
    buttonElement->MarkDirty();
}

} // namespace OHOS::Ace::V2