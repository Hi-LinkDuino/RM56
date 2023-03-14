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

#include "core/components_v2/inspector/hyperlink_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/hyperlink/render_hyperlink.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const HyperlinkComposedElement&)>> CREATE_JSON_MAP {
    { "address", [](const HyperlinkComposedElement& inspector) { return inspector.GetAddress(); } },
    { "content", [](const HyperlinkComposedElement& inspector) { return inspector.GetContent(); } },
    { "color", [](const HyperlinkComposedElement& inspector) { return inspector.GetColor(); } }
};

}

void HyperlinkComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(
        std::string("address: ").append(GetAddress()));
    DumpLog::GetInstance().AddDesc(
        std::string("content: ").append(GetContent()));
    DumpLog::GetInstance().AddDesc(
        std::string("color: ").append(GetColor()));
}

std::unique_ptr<JsonValue> HyperlinkComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string HyperlinkComposedElement::GetAddress() const
{
    auto node = GetHyperlinkComponent();
    if (node) {
        return node->GetAddress();
    }
    return "-";
}

std::string HyperlinkComposedElement::GetContent() const
{
    auto node = GetHyperlinkComponent();
    if (node) {
        return node->GetSummary();
    }
    return "-";
}

std::string HyperlinkComposedElement::GetColor() const
{
    auto node = GetHyperlinkComponent();
    if (node) {
        return node->GetColor().ColorToString();
    }
    return "-";
}
RefPtr<HyperlinkComponent> HyperlinkComposedElement::GetHyperlinkComponent() const
{
    auto node = GetInspectorNode(HyperlinkElement::TypeId());
    if (node) {
        auto renderHyperlink = AceType::DynamicCast<RenderHyperlink>(node);
        return renderHyperlink->GetHyperlinkComponent();
    }
    return nullptr;
}

void HyperlinkComposedElement::AddChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto hyperlinkElement = GetContentElement<HyperlinkElement>(HyperlinkElement::TypeId());
    if (!hyperlinkElement) {
        LOGE("get HyperlinkElement failed");
        return;
    }
    hyperlinkElement->UpdateChildWithSlot(nullptr, newComponent, slot, slot);
    hyperlinkElement->MarkDirty();
}

void HyperlinkComposedElement::UpdateChildWithSlot(int32_t slot, const RefPtr<Component>& newComponent)
{
    auto hyperlinkElement = GetContentElement<HyperlinkElement>(HyperlinkElement::TypeId());
    if (!hyperlinkElement) {
        LOGE("get HyperlinkElement failed");
        return;
    }
    auto child = hyperlinkElement->GetChildBySlot(slot);
    if (!child) {
        LOGE("hyperlinkElement get GetChildBySlot failed");
        return;
    }
    hyperlinkElement->UpdateChildWithSlot(child, newComponent, slot, slot);
    hyperlinkElement->MarkDirty();
}

void HyperlinkComposedElement::DeleteChildWithSlot(int32_t slot)
{
    auto hyperlinkElement = GetContentElement<HyperlinkElement>(HyperlinkElement::TypeId());
    if (!hyperlinkElement) {
        LOGE("get HyperlinkElement failed");
        return;
    }
    auto child = hyperlinkElement->GetChildBySlot(slot);
    hyperlinkElement->UpdateChildWithSlot(child, nullptr, slot, slot);
    hyperlinkElement->MarkDirty();
}

} // namespace OHOS::Ace::V2