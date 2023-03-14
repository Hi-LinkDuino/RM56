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

#include "core/components_v2/inspector/flex_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/flex/render_flex.h"
#include "core/components/wrap/render_wrap.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string,
    std::function<std::unique_ptr<JsonValue>(const FlexComposedElement&)>> CREATE_JSON_MAP {
    { "constructor", [](const FlexComposedElement& inspector) { return inspector.GetConstructor(); } }
};

}

void FlexComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(
        std::string("direction: ").append(GetFlexDirection()));
    DumpLog::GetInstance().AddDesc(
        std::string("wrap: ").append(GetWrap()));
    DumpLog::GetInstance().AddDesc(
        std::string("justifyContent: ").append(GetJustifyContent()));
    DumpLog::GetInstance().AddDesc(
        std::string("alignItems: ").append(GetAlignItems()));
    DumpLog::GetInstance().AddDesc(
        std::string("alignContent: ").append(GetAlignContent()));
}

std::unique_ptr<JsonValue> FlexComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this));
    }
    return resultJson;
}

std::unique_ptr<JsonValue> FlexComposedElement::GetConstructor() const
{
    auto jsonValue = JsonUtil::Create(false);
    jsonValue->Put("direction", GetFlexDirection().c_str());
    jsonValue->Put("wrap", GetWrap().c_str());
    jsonValue->Put("justifyContent", GetJustifyContent().c_str());
    jsonValue->Put("alignItems", GetAlignItems().c_str());
    jsonValue->Put("alignContent", GetAlignContent().c_str());
    return jsonValue;
}

std::string FlexComposedElement::GetFlexDirection() const
{
    auto node = GetInspectorNode(FlexElement::TypeId());
    if (!node) {
        return "FlexDirection.Row";
    }
    auto renderFlex = AceType::DynamicCast<RenderFlex>(node);
    if (renderFlex) {
        return ConvertFlexDirectionToStirng(renderFlex->GetDirection());
    }
    return "FlexDirection.Row";
}

std::string FlexComposedElement::GetWrap() const
{
    return "FlexWrap.NoWrap";
}

std::string FlexComposedElement::GetJustifyContent() const
{
    auto node = GetInspectorNode(FlexElement::TypeId());
    if (!node) {
        return "FlexAlign.Start";
    }
    auto renderFlex = AceType::DynamicCast<RenderFlex>(node);
    if (renderFlex) {
        return ConvertFlexAlignToStirng(renderFlex->GetJustifyContent());
    }
    return "FlexAlign.Start";
}

std::string FlexComposedElement::GetAlignItems() const
{
    auto node = GetInspectorNode(FlexElement::TypeId());
    if (!node) {
        return "FlexAlign.Start";
    }
    auto renderFlex = AceType::DynamicCast<RenderFlex>(node);
    if (renderFlex) {
        return ConvertFlexAlignToStirng(renderFlex->GetAlignItems());
    }
    return "FlexAlign.Start";
}

std::string FlexComposedElement::GetAlignContent() const
{
    auto node = GetInspectorNode(FlexElement::TypeId());
    if (!node) {
        return "FlexAlign.Start";
    }
    return "FlexAlign.Start";
}

void FlexComposedElement::AddChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto flexElement = GetContentElement<FlexElement>(FlexElement::TypeId());
    if (!flexElement) {
        LOGE("get GetFlexElement failed");
        return;
    }
    flexElement->UpdateChildWithSlot(nullptr, newComponent, slot, slot);
    flexElement->MarkDirty();
}

void FlexComposedElement::UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto flexElement = GetContentElement<FlexElement>(FlexElement::TypeId());
    if (!flexElement) {
        LOGE("get GetFlexElement failed");
        return;
    }
    auto child = flexElement->GetChildBySlot(slot);
    flexElement->UpdateChildWithSlot(child, newComponent, slot, slot);
    flexElement->MarkDirty();
}

void FlexComposedElement::DeleteChildWithSlot(int32_t slot)
{
    auto flexElement = GetContentElement<FlexElement>(FlexElement::TypeId());
    if (!flexElement) {
        LOGE("get GetFlexElement failed");
        return;
    }
    auto child = flexElement->GetChildBySlot(slot);
    flexElement->UpdateChildWithSlot(child, nullptr, slot, slot);
    flexElement->MarkDirty();
}

} // namespace OHOS::Ace::V2